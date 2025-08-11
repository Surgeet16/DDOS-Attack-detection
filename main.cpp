#if defined(_WIN32)
#define _WIN32_WINNT 0x0A00 // Target Windows 10
#endif


#include "httplib.h"
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "AhoCorasick.hpp"
#include "AnomalyDetector.hpp"
#include "PacketSimulator.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

// --- Shared Resources (Protected by Mutex) ---
std::mutex mtx;
std::vector<json> alerts;
long long total_packets_processed = 0;
long long patterns_detected = 0;
long long ddos_alerts = 0;
std::unordered_map<std::string, int> top_attackers;

// --- NIDS Core Components ---
AhoCorasick pattern_detector({"<script>", "DROP TABLE", "root-access", "phishing_link"});
AnomalyDetector anomaly_detector(10, 30); // 10-second window, 30 requests max
PacketSimulator simulator;

// --- Helper function to read HTML file ---
std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "Error: Could not open file.";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


// --- Packet Processing Thread ---
void packet_processing_loop() {
    while (true) {
        Packet p = simulator.generate_packet();

        // 1. Check for malicious patterns
        auto found_patterns = pattern_detector.search(p.payload);
        if (!found_patterns.empty()) {
            std::lock_guard<std::mutex> lock(mtx);
            patterns_detected++;
            top_attackers[p.src_ip]++;
            json alert_msg;
            alert_msg["type"] = "Pattern Match";
            alert_msg["ip"] = p.src_ip;
            alert_msg["details"] = "Found patterns: " + found_patterns[0];
            alert_msg["timestamp"] = p.timestamp;
            alerts.push_back(alert_msg);
        }

        // 2. Check for DDoS anomalies
        if (anomaly_detector.check_ip(p.src_ip)) {
            std::lock_guard<std::mutex> lock(mtx);
            ddos_alerts++;
            top_attackers[p.src_ip]++;
            json alert_msg;
            alert_msg["type"] = "DDoS Attempt";
            alert_msg["ip"] = p.src_ip;
            alert_msg["details"] = "High request rate detected.";
            alert_msg["timestamp"] = p.timestamp;
            
            // Avoid flooding with the same alert
            if (alerts.empty() || alerts.back()["details"] != alert_msg["details"]) {
                 alerts.push_back(alert_msg);
            }
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            total_packets_processed++;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Control simulation speed
    }
}

int main() {
    srand(time(0));
    httplib::Server svr;

    // --- Serve Frontend ---
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(read_file("index.html"), "text/html");
    });

    // --- API Endpoints ---
    svr.Get("/api/stats", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(mtx);
        json stats;
        stats["total_packets"] = total_packets_processed;
        stats["patterns_detected"] = patterns_detected;
        stats["ddos_alerts"] = ddos_alerts;
        stats["top_attackers"] = top_attackers;
        res.set_content(stats.dump(), "application/json");
    });

    svr.Get("/api/alerts", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(mtx);
        json response;
        response["alerts"] = alerts;
        res.set_content(response.dump(), "application/json");
    });
    
    // --- DDoS Simulation Control ---
    svr.Get("/api/ddos/start", [](const httplib::Request&, httplib::Response& res) {
        simulator.start_ddos();
        res.set_content("DDoS simulation started.", "text/plain");
    });
    
    svr.Get("/api/ddos/stop", [](const httplib::Request&, httplib::Response& res) {
        simulator.stop_ddos();
        res.set_content("DDoS simulation stopped.", "text/plain");
    });


    // --- Start Packet Processing in a Background Thread ---
    std::thread nids_thread(packet_processing_loop);
    nids_thread.detach();

    std::cout << "NIDS Server started at http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
