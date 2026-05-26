#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <unordered_map>

std::string getColumn(const std::string& line, const int column,
                      const char delim = '\t') {
    int currCol = 0, startPos = 0;
    while (currCol < column) {
        startPos = line.find(delim, startPos + 1);
        currCol++;
    }
    int nextTab = line.find(delim, startPos + 1);
    return line.substr(startPos + 1, nextTab - startPos - 1);
}

struct tm toTimestamp(std::string timestamp) {
    struct tm time;
    const char *eos = timestamp.c_str() + timestamp.size();
    const char *chr = strptime(timestamp.c_str(), "%m/%d/%Y %r", &time);
    // Ensure all the characters have been processed successfully.
    if (chr != eos) {
        std::cout << "chr = " << (long) chr << ", eos = "
                  << (long) eos << std::endl;
        std::cerr << "Invalid time stamp: " << timestamp << std::endl;
    }
    return time;
}

int main_ts_pickup() {
    std::ifstream tsv("Taxi_Trips.tsv");
    std::string line;
    // Read and discard the first line that has header
    std::getline(tsv, line);
    // Process each entry and update occurrence of each timestamp
    std::unordered_map<std::string, int> tsOccurs;
    while (std::getline(tsv, line)) {
        // Extract the trip start timestamp & pickup community area id
        const std::string key = getColumn(line, 2) + '\t' +
            getColumn(line, 8);
        // Track number of occurrences
        tsOccurs[key]++;
    }
    // Now print the occurrence information.
    std::cout << "#TimeStamp\tPickupAreaID\tDayOfWk\tPickUpTime(mins)\tCount\n";
    for (const auto& entry : tsOccurs) {
        // Convert it to a struct tm
        const std::string ts = entry.first.substr(0, entry.first.find('\t'));
        const struct tm timeDetails = toTimestamp(ts);
        // Convert hour and min to minutes.
        const int mins = (timeDetails.tm_hour * 60) + timeDetails.tm_min;
        std::cout << entry.first << '\t' << timeDetails.tm_wday << '\t'
                  << mins << "\t" << entry.second << "\n";
    }
    return 0;
}


int main_pick_drop() {
    std::ifstream tsv("Taxi_Trips.tsv");
    std::string line;
    // Read and discard the first line that has header
    std::getline(tsv, line);
    // Process each entry and update occurrence of each timestamp
    std::unordered_map<std::string, int> pickupOccurs, dropOffOccurs;
    while (std::getline(tsv, line)) {
        // Extract the trip start community area id
        const std::string pickupComm = getColumn(line, 8); 
        // Track the drop off community 
        const std::string dropComm   = getColumn(line, 9);

        // Only track thoes with both pick-up and drop-off is within
        // chicago metro area.
        if (!pickupComm.empty() && !dropComm.empty()) {
            pickupOccurs[pickupComm]++;
            dropOffOccurs[dropComm]++;
        }
    }
    // Now print the occurrence information.
    std::cout << "#PickupAreaID\tDropAreaID\tCount\n";
    for (const auto& entry : pickupOccurs) {
        std::cout << entry.first << "\t-1\t" << entry.second << "\n";
    }
    for (const auto& entry : dropOffOccurs) {
        std::cout << "-1\t" << entry.first << '\t' << entry.second << "\n";
    }
    return 0;
}

// int main_pick_drop_matrix() {
int main() {
    std::ifstream tsv("Taxi_Trips.tsv");
    std::string line;
    // Read and discard the first line that has header
    std::getline(tsv, line);
    // Process each entry and update occurrence of each timestamp
    std::unordered_map<std::string, int> pickDropOccurs;
    while (std::getline(tsv, line)) {
        // Extract the trip start community area id
        const std::string pickupComm = getColumn(line, 8); 
        // Track the drop off community 
        const std::string dropComm   = getColumn(line, 9);
        // Only track thoes with both pick-up and drop-off is within
        // chicago metro area.
        if (!pickupComm.empty() && !dropComm.empty()) {
            const std::string key = pickupComm + '\t' + dropComm;
            pickDropOccurs[key]++;
        }
    }
    // Here we assume we have exactly 77 pick-up and drop-off areas
    // for now and print a matrix of values.
    for (int pick = 1; (pick < 78); pick++) {
        for (int drop = 1; (drop < 78); drop++) {
            const std::string key = std::to_string(pick) + '\t' +
                std::to_string(drop);
            std::cout << (pickDropOccurs[key] / 1000000.0) << '\t';
        }
        std::cout << std::endl;
    }
    return 0;
}
