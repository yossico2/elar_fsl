#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

// Include TinyXML-2 header
// Ensure libtinyxml2-dev is installed on your PetaLinux rootfs
#include <tinyxml2.h>

using namespace tinyxml2;

// --- Config Structure ---
struct AppConfig {
    int udp_local_port;
    std::string udp_remote_ip;
    int udp_remote_port;
    std::string uds_my_path;
    std::string uds_target_path;
};

// Global variables needed for cleanup
int uds_fd = -1;
std::string global_uds_path; 

void signal_handler(int signum) {
    std::cout << "\nClosing application..." << std::endl;
    if (uds_fd != -1) {
        close(uds_fd);
        if (!global_uds_path.empty()) {
            unlink(global_uds_path.c_str());
        }
    }
    exit(signum);
}

// Function to load settings from XML file using TinyXML-2
AppConfig load_config(const char* filename) {
    XMLDocument doc;
    XMLError result = doc.LoadFile(filename);
    
    if (result != XML_SUCCESS) {
        throw std::runtime_error("Failed to load XML file. Check if file exists and is valid.");
    }

    XMLElement* root = doc.FirstChildElement("config");
    if (root == nullptr) throw std::runtime_error("Invalid XML: Missing <config> root element");

    AppConfig config;

    // --- Parse UDP Settings ---
    XMLElement* udp_node = root->FirstChildElement("udp_settings");
    if (udp_node) {
        // Safe query for integers
        if (udp_node->FirstChildElement("local_port")->QueryIntText(&config.udp_local_port) != XML_SUCCESS)
            throw std::runtime_error("Missing or invalid <local_port>");
        
        if (udp_node->FirstChildElement("remote_port")->QueryIntText(&config.udp_remote_port) != XML_SUCCESS)
            throw std::runtime_error("Missing or invalid <remote_port>");

        // Get text for string
        const char* ip_text = udp_node->FirstChildElement("remote_ip")->GetText();
        if (ip_text) config.udp_remote_ip = ip_text;
        else throw std::runtime_error("Missing <remote_ip>");
    } else {
        throw std::runtime_error("Missing <udp_settings> section");
    }

    // --- Parse UDS Settings ---
    XMLElement* uds_node = root->FirstChildElement("uds_settings");
    if (uds_node) {
        const char* my_path_text = uds_node->FirstChildElement("my_path")->GetText();
        const char* target_path_text = uds_node->FirstChildElement("target_path")->GetText();

        if (my_path_text) config.uds_my_path = my_path_text;
        else throw std::runtime_error("Missing <my_path>");

        if (target_path_text) config.uds_target_path = target_path_text;
        else throw std::runtime_error("Missing <target_path>");
    } else {
        throw std::runtime_error("Missing <uds_settings> section");
    }

    return config;
}

int main() {
    signal(SIGINT, signal_handler);

    AppConfig config;
    try {
        config = load_config("./config.xml");
        std::cout << "Configuration loaded from XML successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return 1;
    }

    global_uds_path = config.uds_my_path;

    // ====================================================
    // Step 1: Initialize UDP Socket
    // ====================================================
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        perror("Error creating UDP socket");
        return 1;
    }

    sockaddr_in local_udp_addr{};
    local_udp_addr.sin_family = AF_INET;
    local_udp_addr.sin_port = htons(config.udp_local_port);
    local_udp_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udp_fd, (struct sockaddr*)&local_udp_addr, sizeof(local_udp_addr)) < 0) {
        perror("Error binding UDP socket");
        return 1;
    }

    // ====================================================
    // Step 2: Initialize Remote Client
    // ====================================================
    sockaddr_in remote_udp_client{};
    remote_udp_client.sin_family = AF_INET;
    remote_udp_client.sin_port = htons(config.udp_remote_port);
    if (inet_pton(AF_INET, config.udp_remote_ip.c_str(), &remote_udp_client.sin_addr) <= 0) {
        std::cerr << "Invalid Remote IP Address: " << config.udp_remote_ip << std::endl;
        return 1;
    }
    socklen_t remote_udp_len = sizeof(remote_udp_client);

    // ====================================================
    // Step 3: Initialize UDS Socket
    // ====================================================
    uds_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (uds_fd < 0) {
        perror("Error creating UDS socket");
        close(udp_fd);
        return 1;
    }

    sockaddr_un uds_server_addr{};
    uds_server_addr.sun_family = AF_UNIX;
    strncpy(uds_server_addr.sun_path, config.uds_my_path.c_str(), sizeof(uds_server_addr.sun_path) - 1);
    
    unlink(config.uds_my_path.c_str());

    if (bind(uds_fd, (struct sockaddr*)&uds_server_addr, sizeof(uds_server_addr)) < 0) {
        perror("Error binding UDS socket");
        close(udp_fd);
        close(uds_fd);
        return 1;
    }

    sockaddr_un target_uds_addr{};
    target_uds_addr.sun_family = AF_UNIX;
    strncpy(target_uds_addr.sun_path, config.uds_target_path.c_str(), sizeof(target_uds_addr.sun_path) - 1);

    std::cout << "Bridge Service Running (XML Config).\n"
              << "UDP: " << config.udp_local_port << " <-> " << config.udp_remote_ip << ":" << config.udp_remote_port << "\n"
              << "UDS: " << config.uds_my_path << " -> " << config.uds_target_path << std::endl;

    // ====================================================
    // Step 4: Main Loop
    // ====================================================
    struct pollfd fds[2];
    fds[0].fd = udp_fd;
    fds[0].events = POLLIN;
    fds[1].fd = uds_fd;
    fds[1].events = POLLIN;

    char buffer[4096];

    while (true) {
        int ret = poll(fds, 2, -1);
        if (ret < 0) {
            perror("Poll failed");
            break;
        }

        // UDP -> UDS
        if (fds[0].revents & POLLIN) {
            sockaddr_in sender_addr;
            socklen_t sender_len = sizeof(sender_addr);
            int n = recvfrom(udp_fd, buffer, sizeof(buffer), 0, 
                             (struct sockaddr*)&sender_addr, &sender_len);
            if (n > 0) {
                sendto(uds_fd, buffer, n, 0, 
                       (struct sockaddr*)&target_uds_addr, sizeof(target_uds_addr));
            }
        }

        // UDS -> UDP
        if (fds[1].revents & POLLIN) {
            int n = recvfrom(uds_fd, buffer, sizeof(buffer), 0, NULL, NULL);
            if (n > 0) {
                sendto(udp_fd, buffer, n, 0, 
                       (struct sockaddr*)&remote_udp_client, remote_udp_len);
            }
        }
    }

    close(udp_fd);
    close(uds_fd);
    if (!config.uds_my_path.empty()) {
        unlink(config.uds_my_path.c_str());
    }
    
    return 0;
}
