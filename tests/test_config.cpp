#include <iostream>
#include <unistd.h>
struct PrintCWD
{
    PrintCWD()
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)))
        {
            std::cout << "[DEBUG] (main) CWD: " << cwd << std::endl;
        }
    }
};
static PrintCWD printCWDInstance;
#include "catch.hpp"
#include "../src/config.h"
#include <fstream>
#include "test_utils.h"

TEST_CASE("Config parsing: minimal valid config", "[config]")
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)))
    {
        std::cout << "[DEBUG] (test_config) CWD: " << cwd << std::endl;
        std::cout.flush();
    }
    std::cout << "[DEBUG] (test_config) About to create test_config.xml" << std::endl;
    std::cout.flush();
    const char *xml = R"(
        <config>
            <udp>
                <local_port>1234</local_port>
                <remote_ip>127.0.0.1</remote_ip>
                <remote_port>5678</remote_port>
            </udp>
            <data_link_uds>
                <server>
                    <path>/tmp/test1.sock</path>
                    <receive_buffer_size>1024</receive_buffer_size>
                    </server>
                <client name="test.ul">/tmp/test2.sock</client>
            </data_link_uds>
            <ul_uds_mapping>
                <mapping opcode="1" uds="test.ul" />
            </ul_uds_mapping>
            <ctrl_status_uds>
                <appx>
                    <request>
                        <path>/tmp/appx_to_fcom</path>
                        <receive_buffer_size>2048</receive_buffer_size>
                    </request>
                    <response>
                        <path>/tmp/fcom_to_appx</path>
                        <receive_buffer_size>4096</receive_buffer_size>
                    </response>
                </appx>
            </ctrl_status_uds>
        </config>
    )";

    std::string config_path = get_test_config_path();
    std::ofstream f(config_path);
    if (f.is_open())
    {
        std::cout << "[DEBUG] (test_config) test_config.xml created/opened for writing" << std::endl;
    }
    else
    {
        std::cout << "[DEBUG] (test_config) FAILED to open test_config.xml for writing" << std::endl;
    }
    std::cout.flush();
    f << xml;
    f.close();
    std::cout << "[DEBUG] (test_config) test_config.xml write/close done" << std::endl;
    std::cout.flush();

    AppConfig cfg = load_config(config_path.c_str(), -1);
    REQUIRE(cfg.udp_local_port == 1234);
    REQUIRE(cfg.udp_remote_ip == "127.0.0.1");
    REQUIRE(cfg.udp_remote_port == 5678);
    REQUIRE(cfg.uds_servers.size() == 1);
    REQUIRE(cfg.uds_servers[0].path == "/tmp/test1.sock");
    REQUIRE(cfg.uds_clients.size() == 1);
    REQUIRE(cfg.uds_clients.at("test.ul") == "/tmp/test2.sock");
    REQUIRE(cfg.ul_uds_mapping.size() == 1);
    REQUIRE(cfg.ul_uds_mapping.at(1) == "test.ul");
    REQUIRE(cfg.ctrl_uds_name.size() == 1);
    REQUIRE(cfg.ctrl_uds_name.count("appx") == 1);
    REQUIRE(cfg.ctrl_uds_name.at("appx").request_path == "/tmp/appx_to_fcom");
    REQUIRE(cfg.ctrl_uds_name.at("appx").request_buffer_size == 2048);
    REQUIRE(cfg.ctrl_uds_name.at("appx").response_path == "/tmp/fcom_to_appx");
    REQUIRE(cfg.ctrl_uds_name.at("appx").response_buffer_size == 4096);
    // Do not remove the shared test_config.xml, as other tests depend on it
}
