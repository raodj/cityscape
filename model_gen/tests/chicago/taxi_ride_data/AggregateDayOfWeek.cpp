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

int main() {
    std::ifstream tsv("Taxi_Trips.tsv");
    std::string line;
    // Read and discard the first line that has header
    std::getline(tsv, line);
    // Process each entry and update occurrence of each timestamp
    std::unordered_map<std::string, int> tsOccurs;
    while (std::getline(tsv, line)) {
        // Extract the trip start timestamp
        const std::string timestamp = getColumn(line, 2);
        // Track number of occurrences
        tsOccurs[timestamp]++;
    }
    // Now print the occurrence information.
    std::cout << "#TimeStamp\tDayOfWk\tTime(mins)\tCount\n";    
    for (const auto& entry : tsOccurs) {
        // Convert it to a struct tm
        const struct tm timeDetails = toTimestamp(entry.first);
        // Convert hour and min to minutes.
        const int mins = (timeDetails.tm_hour * 60) + timeDetails.tm_min;
        std::cout << entry.first << '\t' << timeDetails.tm_wday << '\t'
                  << mins << "\t" << entry.second << "\n";
    }
    return 0;
}
