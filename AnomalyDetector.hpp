#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <deque>
#include <chrono>

class AnomalyDetector {
private:
    // A hash map where key is IP and value is a deque of request timestamps
    std::unordered_map<std::string, std::deque<long long>> ip_requests;
    const int window_seconds;
    const int max_requests;

public:
    AnomalyDetector(int window = 60, int max_req = 100) 
        : window_seconds(window), max_requests(max_req) {}

    // Checks if an IP has exceeded the request limit. Returns true if it's an anomaly.
    bool check_ip(const std::string& ip) {
        long long current_time = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        
        // Add current request timestamp
        ip_requests[ip].push_back(current_time);

        // Remove timestamps older than the window
        while (!ip_requests[ip].empty() && ip_requests[ip].front() < current_time - window_seconds) {
            ip_requests[ip].pop_front();
        }

        // Return true if request count exceeds the max limit
        return ip_requests[ip].size() > max_requests;
    }
};
