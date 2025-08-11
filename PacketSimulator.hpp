#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>

struct Packet {
    long long timestamp;
    std::string src_ip;
    std::string payload;
};

class PacketSimulator {
private:
    std::vector<std::string> normal_ips;
    std::vector<std::string> malicious_payloads;
    std::string ddos_attacker_ip = "198.51.100.10";
    bool ddos_active = false;

public:
    PacketSimulator() {
        normal_ips = {"203.0.113.5", "192.0.2.18", "203.0.113.25", "198.51.100.33"};
        malicious_payloads = {"<script>malware()</script>", "DROP TABLE users;", "root-access-key", "phishing_link"};
    }

    void start_ddos() { ddos_active = true; }
    void stop_ddos() { ddos_active = false; }

    Packet generate_packet() {
        long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        
        std::string ip = normal_ips[rand() % normal_ips.size()];
        std::string payload = "some normal application data...";

        // Simulate a DDoS attack
        if (ddos_active && (rand() % 100 < 95)) { // 95% chance to be attacker
            ip = ddos_attacker_ip;
        }

        // Occasionally inject a malicious payload
        if (rand() % 100 < 5) { // 5% chance
            payload = malicious_payloads[rand() % malicious_payloads.size()];
        }

        return {timestamp, ip, payload};
    }
};