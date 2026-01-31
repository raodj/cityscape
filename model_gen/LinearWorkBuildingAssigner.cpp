#ifndef LINEAR_WORK_BUILDING_ASSIGNER_CPP
#define LINEAR_WORK_BUILDING_ASSIGNER_CPP

#include <omp.h>
#include <ctime>
#include <unistd.h>
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <tuple>
#include <string>
#include <vector>

#include "Utilities.h"
#include "OSMData.h"
#include "PathFinder.h"
#include "MPIHelper.h"
#include "Stopwatch.h"
#include "LinearWorkBuildingAssigner.h"

// Static MPI window
MPI_Win LinearWorkBuildingAssigner::bldIdxWin;

constexpr int RANK_0 = 0;

// ------------------------------------------------------------

static std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return buffer;
}

// ------------------------------------------------------------
// Linear model sample container
// ------------------------------------------------------------
struct LMSample {
    double time;
    double distance;
    long homeID;
    long workID;
};

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
LinearWorkBuildingAssigner::LinearWorkBuildingAssigner(
        const OSMData& model,
        const int jwtrnsIdx,
        const int jwmnpIdx,
        const int offSqFtPer,
        const int avgSpeed,
        int lmNumSamples)
    : model(model),
      jwtrnsIdx(jwtrnsIdx),
      jwmnpIdx(jwmnpIdx),
      offSqFtPer(offSqFtPer),
      avgSpeed(avgSpeed),
      lmNumSamples(lmNumSamples),
      nextBldIndex(0),
      modelSlope(0.0),
      modelIntercept(0.0),
      modelR2(0.0) {

    std::string slurmID =
        getenv("SLURM_JOB_ID") ? getenv("SLURM_JOB_ID") : "";
    std::string rank = std::to_string(MPI_GET_RANK());

    stats.open("stats_job" + slurmID + "_rank" + rank + ".txt");
}

// ------------------------------------------------------------
// Atomic building index fetch
// ------------------------------------------------------------
long LinearWorkBuildingAssigner::getNextBldIndex() {
    long nextIndex = -1;

#pragma omp critical(bldCS)
    {
#ifdef HAVE_LIBMPI
        const long inc = 1;
        MPI_Win_lock(MPI_LOCK_SHARED, RANK_0, 0, bldIdxWin);
        MPI_Fetch_and_op(&inc, &nextIndex, MPI_LONG,
                         RANK_0, 0, MPI_SUM, bldIdxWin);
        MPI_Win_unlock(RANK_0, bldIdxWin);
#else
        nextIndex = nextBldIndex++;
#endif
    }
    return nextIndex;
}

// ------------------------------------------------------------
// Generate linear distance ~ time model
// ------------------------------------------------------------
void LinearWorkBuildingAssigner::generateLinearModel(
        int lmNumSamples,
        const BuildingMap& homeBuildings,
        const BuildingMap& nonHomeBuildings,
        const std::vector<size_t>& homeBldIdList) {

    // Random engine
    std::random_device rd;
    std::mt19937 gen(rd());

    // Build list of non-home building IDs for random sampling
    std::vector<long> nonHomeIds;
    nonHomeIds.reserve(nonHomeBuildings.size());
    for (const auto& p : nonHomeBuildings) {
        nonHomeIds.push_back(p.first);
    }

    // Quick guards
    if (homeBldIdList.empty() || nonHomeIds.empty()) {
        std::cerr << "Insufficient buildings to generate LM samples." 
                  << std::endl;
        // keep modelSlope/intercept as defaults (0)
        modelSlope = 0.0;
        modelIntercept = 0.0;
        modelR2 = 0.0;
        return;
    }

    std::uniform_int_distribution<size_t> homeDist(0, homeBldIdList.size() - 1);
    std::uniform_int_distribution<size_t> nonHomeDist(0, nonHomeIds.size() - 1);

    std::vector<LMSample> samples;
    samples.reserve(lmNumSamples);

    // Limit attempts to avoid infinite loops if many pairs have no path
    const int maxAttempts = std::max(10000, lmNumSamples * 10);
    int attempts = 0;

    // Collect samples
    while ((int)samples.size() < lmNumSamples && attempts < maxAttempts) {
        attempts++;

        // pick random home and random non-home
        size_t hIdx = homeDist(gen);
        long homeId = homeBldIdList[hIdx];

        size_t nhIdx = nonHomeDist(gen);
        long workId = nonHomeIds[nhIdx];

        // avoid selecting same ID (just in case)
        if (homeId == workId) continue;

        // Get building objects
        auto hbIt = homeBuildings.find(homeId);
        auto wbIt = nonHomeBuildings.find(workId);
        if (hbIt == homeBuildings.end() || wbIt == nonHomeBuildings.end()) 
            continue;

        const Building& hb = hbIt->second;
        const Building& wb = wbIt->second;

        // haversine distance
        double distMiles = getDistance(hb.wayLat, hb.wayLon, 
                                       wb.wayLat, wb.wayLon);

        // use PathFinder to compute actual travel time (slowish)
        PathFinder pf(model);
        const Path path = pf.findBestPath(hb.id, wb.id, true, 0.25, 0.1);
        if (path.size() == 0) {
            // no valid path found; skip
            continue;
        }
        
        // as in other code
        double travelTimeMin = std::round(path.back().distance * 60.0); 

        // Accept sample
        LMSample s;
        s.time = travelTimeMin;
        s.distance = distMiles;
        s.homeID = hb.id;
        s.workID = wb.id;
        samples.push_back(s);
    }

    if (samples.empty()) {
        std::cerr << "Unable to generate LM samples (no valid paths found)." 
                  << std::endl;
        modelSlope = 0.0;
        modelIntercept = 0.0;
        modelR2 = 0.0;
        return;
    }

    // ---------------------------
    // Prepare vectors for regression (time -> distance)
    // ---------------------------
    std::vector<double> x_vals; x_vals.reserve(samples.size());  // time
    std::vector<double> y_vals; y_vals.reserve(samples.size());  // distance

    for (const auto &s : samples) {
        x_vals.push_back(s.time);
        y_vals.push_back(s.distance);
    }

    // ---------------------------
    // Compute means
    // ---------------------------
    double sum_x = std::accumulate(x_vals.begin(), x_vals.end(), 0.0);
    double sum_y = std::accumulate(y_vals.begin(), y_vals.end(), 0.0);
    double mean_x = sum_x / x_vals.size();
    double mean_y = sum_y / y_vals.size();

    // ---------------------------
    // Compute slope (β1) and intercept (β0)
    // ---------------------------
    double num = 0.0, den = 0.0;
    for (size_t i = 0; i < x_vals.size(); ++i) {
        num += (x_vals[i] - mean_x) * (y_vals[i] - mean_y);
        den += (x_vals[i] - mean_x) * (x_vals[i] - mean_x);
    }

    double slope = 0.0;
    double intercept = 0.0;
    if (den != 0.0) {
        slope = num / den;
        intercept = mean_y - slope * mean_x;
    } else {
        // degenerate case
        slope = 0.0;
        intercept = mean_y;
    }

    // ---------------------------
    // Compute R^2
    // ---------------------------
    double ss_tot = 0.0, ss_res = 0.0;
    for (size_t i = 0; i < x_vals.size(); ++i) {
        double pred = intercept + slope * x_vals[i];
        ss_res += (y_vals[i] - pred) * (y_vals[i] - pred);
        ss_tot += (y_vals[i] - mean_y) * (y_vals[i] - mean_y);
    }

    double r2 = (ss_tot == 0.0) ? 0.0 : (1.0 - (ss_res / ss_tot));

    // Store the model in the class (only once)
    modelSlope = slope;
    modelIntercept = intercept;
    modelR2 = r2;

    // ---------------------------
    // Print the LM summary
    // ---------------------------
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\nLinear regression model generated from " 
                  << samples.size() << " samples:\n";
    std::cout << "Independent variable: commute_time_minutes\n";
    std::cout << "Dependent variable: travel_distance_miles\n";
    std::cout << "Slope (β1):      " << slope << "\n";
    std::cout << "Intercept (β0):  " << intercept << "\n";
    std::cout << "R^2:             " << r2 << "\n";

    // -------------------------------------------
    // Print sample data used for model generation
    // -------------------------------------------
    std::cout << "\nSample data used for LM generation:\n";
    std::cout << "index,time_min,distance_miles,home_id,work_id\n";

    // Change between samples.size() or desired value
    int maxToPrint = samples.size();   
    maxToPrint = std::min(maxToPrint, static_cast<int>(samples.size()));

    for (int i = 0; i < maxToPrint; i++) {
        const auto& s = samples[i];
        std::cout << i << ","
                  << s.time << ","
                  << s.distance << ","
                  << s.homeID << ","
                  << s.workID << "\n";
    }

    // done
}

// ------------------------------------------------------------
// Process a single candidate work building for each person
// ------------------------------------------------------------
void LinearWorkBuildingAssigner::processBuilding(
        long /*idx*/, long bldId,
        Building& bld,
        BuildingMap& nonHomeBuildings) {

    for (auto& hld : bld.households) {
        for (auto& person : hld.getPeopleInfo()) {

            if (person.getIntegerInfo(jwtrnsIdx) != 1)
                continue;

            const int travelTime = person.getIntegerInfo(jwmnpIdx);
            if (travelTime < 0)
                continue;

            Stopwatch timer;
            timer.start();

            // Linear prediction
            const double predictedDist =
                modelIntercept + modelSlope * travelTime;

            const double minDist = predictedDist * 0.7;
            const double maxDist = predictedDist * 1.3;

            BuildingList candidates =
                getCandidateWorkBuildings(
                    bld,
                    nonHomeBuildings,
                    travelTime,
                    travelTime,
                    3);

            int checked = 0;
            bool assigned = false;

            for (const Building& cand : candidates) {
                checked++;

                if (cand.population < 1)
                    continue;

                const double dist =
                    getDistance(bld.wayLat, bld.wayLon,
                                cand.wayLat, cand.wayLon);

                if (dist < minDist || dist > maxDist)
                    continue;

                // Assign immediately
                nonHomeBuildings[cand.id].population--;
                person.setWorkBuilding(bldId, cand.id);

#pragma omp critical (cout)
                stats << "    Assign Building " << cand.id
                      << "(in ring: " << cand.attributes << ") to person "
                      << person.getPerID()
                      << " in building " << bld.id
                      << "(in ring: " << bld.attributes << ") after checking "
                      << checked << " out of "
                      << candidates.size() << " buildings. "
                      << "Person needs time: " << travelTime
                      << ", predicted distance: " << predictedDist
                      << ", actual straight-line distance: " << dist
                      << ".  Compute time: "
                      << timer.elapsedTime()
                      << " milliseconds (linear model)"
                      << std::endl;

                assigned = true;
                break;
            }

            if (!assigned) {
#pragma omp critical(cout)
                {
                    stats << "    Unable to find building for person "
                          << person.getPerID()
                          << " in building " << bld.id
                          << "(in ring: " << bld.attributes << ") after checking "
                          << candidates.size()
                          << " buildings. Person needs time: "
                          << travelTime
                          << ".  Compute time: "
                          << timer.elapsedTime()
                          << " milliseconds (linear model)"
                          << std::endl;
                }
            }
        }
    }
}


// ------------------------------------------------------------
// Main entry
// ------------------------------------------------------------
void LinearWorkBuildingAssigner::assignWorkBuilding(int, char**) {

    std::cout << "Generating Schedule...\n";

#ifdef HAVE_LIBMPI
    nextBldIndex = 0;
    MPI_Win_create(&nextBldIndex, sizeof(long), sizeof(long),
                   MPI_INFO_NULL, MPI_COMM_WORLD, &bldIdxWin);
#endif

    auto [homeBuildings, nonHomeBuildings, homeIds] =
        getHomeAndNonHomeBuildings(model.buildingMap);

    generateLinearModel(lmNumSamples,
                        homeBuildings,
                        nonHomeBuildings,
                        homeIds);

#pragma omp parallel
    {
        const long maxIdx = homeIds.size();
        for (long idx = getNextBldIndex();
             idx < maxIdx;
             idx = getNextBldIndex()) {

            long bid = homeIds[idx];
            processBuilding(idx, bid,
                            homeBuildings.at(bid),
                            nonHomeBuildings);
        }
    }

#ifdef HAVE_LIBMPI
    MPI_Win_free(&bldIdxWin);
#endif
}

std::tuple<BuildingMap, BuildingMap, std::vector<size_t>>
LinearWorkBuildingAssigner::getHomeAndNonHomeBuildings(
        const BuildingMap& buildingMap) const {

    BuildingMap homeBuildings;
    BuildingMap nonHomeBuildings;
    std::vector<size_t> homeIds;

    for (const auto& [id, bld] : buildingMap) {
        if (bld.isHome) {
            if (!bld.households.empty()) {
                homeBuildings[id] = bld;
                homeIds.push_back(id);
            }
        } else {
            Building b = bld;
            b.population = b.getArea() / offSqFtPer;
            nonHomeBuildings[id] = b;
        }
    }

    std::cout << "# of home buildings: "
              << homeBuildings.size() << std::endl;
    std::cout << "# of non-home buildings: "
              << nonHomeBuildings.size() << std::endl;

    return { homeBuildings, nonHomeBuildings, homeIds };
}

// ------------------------------------------------------------
// Candidate generation (unchanged)
// ------------------------------------------------------------
BuildingList LinearWorkBuildingAssigner::getCandidateWorkBuildings(
        const Building& src,
        const BuildingMap& nonHome,
        int minT, int maxT,
        int margin) const {

    BuildingList out;
    for (auto& [id, b] : nonHome) {
        const double d =
            getDistance(src.wayLat, src.wayLon,
                        b.wayLat, b.wayLon);
        const int t =
            std::round(d * 60 / avgSpeed);

        if (t >= minT - margin &&
            t <= maxT + margin &&
            b.population > 0)
            out.push_back(b);
    }
    return out;
}

// ------------------------------------------------------------
// Assign FIRST valid building
// ------------------------------------------------------------
long LinearWorkBuildingAssigner::assignWorkBuilding(
        const OSMData&,
        const Building&,
        BuildingMap& nonHome,
        BuildingList& candidates,
        const PUMSPerson&,
        int) {

    for (Building& b : candidates) {
        if (b.population < 1) continue;

        b.population--;
        nonHome[b.id].population--;
        return b.id;
    }
    return -1;
}


#endif
