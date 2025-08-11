# DDOS-Attack-detection
High-Performance Real-Time Network Intrusion Detection System (NIDS)
This project is a high-performance Network Intrusion Detection System (NIDS) written in modern C++. It is designed to monitor and analyze a stream of network packets in real-time, detecting both known malicious patterns and anomalous traffic behavior, such as DDoS attacks.

The system features a multi-threaded C++ backend for high-speed processing and a dynamic, real-time web dashboard for visualizing alerts and traffic metrics.

✨ Key Features
Dual-Threat Detection Model:

Signature-Based Detection: Utilizes the Aho-Corasick algorithm to efficiently match multiple malicious string patterns (virus signatures, SQL injection, etc.) in a single pass.

Anomaly-Based Detection: Employs a Sliding Window algorithm to detect rate-based anomalies like DDoS attacks by tracking request frequency from IP addresses.

High-Performance C++ Backend:

Multi-threaded Architecture: The packet simulation, processing logic, and web server run on separate threads to ensure a non-blocking, responsive system.

Thread-Safe: Uses std::mutex to manage concurrent access to shared resources like alerts and statistics, preventing race conditions.

Interactive Web Dashboard:

A sleek, modern frontend served as a single HTML file containing all necessary CSS and JavaScript to visualize data in real-time.

Features a live-updating traffic graph (using Chart.js), a real-time alerts log, and panels for key statistics and top attacking IPs.

RESTful API:

The C++ backend serves data to the frontend via a set of clean, simple API endpoints.

🛠️ Tech Stack
Category

Technology

Backend

C++ (17), Multi-threading (<thread>)

Frontend

HTML5, CSS3, JavaScript (ES6)

Libraries

httplib.h (Web Server), nlohmann/json (JSON), Chart.js (Graphing)

Compiler

g++ (MinGW on Windows)

Tools

Git, GitHub, Visual Studio Code

⚙️ Setup and Installation
Follow these steps to compile and run the project on your local machine.

Prerequisites
A C++ compiler that supports C++17 (e.g., g++, Clang, MSVC).

Git for cloning the repository.

1. Clone the Repository
git clone <your-repository-url>
cd <repository-folder>

2. File Structure
Ensure your project directory is organized as follows. Note that all frontend code is contained within index.html.

/
|-- nlohmann/
|   |-- json.hpp
|-- AhoCorasick.hpp
|-- AnomalyDetector.hpp
|-- PacketSimulator.hpp
|-- httplib.h
|-- main.cpp
|-- index.html

3. Compile the Application
Open your terminal in the project's root directory and run the appropriate command for your operating system.

On Windows (using MinGW):
The -lws2_32 flag is crucial for linking the Windows Sockets library.

g++ main.cpp -o nids_server -std=c++17 -pthread -lws2_32

On Linux or macOS:

g++ main.cpp -o nids_server -std=c++17 -pthread

4. Run the Server
Once compiled successfully, an executable named nids_server will be created. Run it:

./nids_server

You should see the output: NIDS Server started at http://localhost:8080

5. View the Dashboard
Open your web browser and navigate to http://localhost:8080. You will see the live dashboard in action. Use the "Start DDoS Sim" button to test the anomaly detection feature.

📄 API Endpoints
The C++ server exposes the following RESTful API endpoints:

GET /: Serves the main index.html page.

GET /api/stats: Returns a JSON object with live statistics (total packets, alerts, top attackers, traffic history).

GET /api/alerts: Returns a JSON array of all security alerts generated.

GET /api/ddos/start: Triggers the DDoS simulation.

GET /api/ddos/stop: Stops the DDoS simulation.
