#include <iostream>
#include <vector>

// The sensor writes -999 when it is offline. Those readings must be ignored.
const int OFFLINE = -999;

struct Summary {
    double average;
    int minimum;
    int maximum;
    int skipped;
};

// One reading per minute for a whole day (1440 minutes).
// The temperature climbs slowly from 15 to 23 degrees.
// The sensor was offline for five minutes, starting at minute 600.
std::vector<int> loadReadings() {
    std::vector<int> readings;
    for (int minute = 0; minute < 1440; minute++) {
        if (minute >= 600 && minute < 605) {
            readings.push_back(OFFLINE);
        } else {
            readings.push_back(15 + minute / 160);
        }
    }
    return readings;
}

// FIXED: compare against the OFFLINE constant instead of a typed-in number.
Summary summarize(const std::vector<int>& readings) {
    Summary result = {0.0, 1000, -1000, 0};
    long sum = 0;
    int count = 0;
    for (std::size_t i = 0; i < readings.size(); i++) {
        int r = readings[i];
        if (r == OFFLINE) {  // skip offline readings
            result.skipped++;
            continue;
        }
        sum += r;
        count++;
        if (r < result.minimum) result.minimum = r;
        if (r > result.maximum) result.maximum = r;
    }
    result.average = static_cast<double>(sum) / count;
    return result;
}

int main() {
    std::vector<int> readings = loadReadings();
    Summary s = summarize(readings);

    std::cout << "Readings: " << readings.size() << std::endl;
    std::cout << "Skipped:  " << s.skipped << "   (should be 5)" << std::endl;
    std::cout << "Minimum:  " << s.minimum << "   (should be 15)" << std::endl;
    std::cout << "Maximum:  " << s.maximum << "   (should be 23)" << std::endl;
    std::cout << "Average:  " << s.average << std::endl;
    return 0;
}
