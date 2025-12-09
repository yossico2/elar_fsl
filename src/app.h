// app.h - Main FSL Application class
//
// The App class manages UDP and multiple UDS sockets (server/client) for routing
// messages between the ground segment (GSL) and space segment applications.
//
// Responsibilities:
//   - Load configuration from XML
//   - Create and manage UDP and UDS sockets
//   - Route messages based on opcode and UDS mapping
//   - Validate configuration and handle errors
//   - Support graceful shutdown via signal handling
//
// Main methods:
//   - App(const std::string &config_path): Constructor, loads config and sets up sockets
//   - run(): Main event loop for polling and routing
//   - cleanup(): Closes sockets and unlinks UDS files
//   - signalHandler(int): Handles SIGINT/SIGTERM for graceful shutdown

#pragma once
#include <string>
#include "config.h"
#include "udp.h"
#include "uds.h"
#include <memory>
#include <signal.h>

class App
{
public:
    App(const std::string &config_path);
    void run();
    void cleanup();
    static void signalHandler(int signum);

private:
    AppConfig config_;
    UdpServerSocket udp_;
    // Multiple UDS servers (downlink)
    std::vector<std::unique_ptr<UdsSocket>> uds_servers_;
    // Multiple UDS clients (uplink), mapped by name
    std::map<std::string, std::unique_ptr<UdsSocket>> uds_clients_;
    static volatile sig_atomic_t shutdown_flag_;
};
