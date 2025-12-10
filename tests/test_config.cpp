#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/config.h"
#include <fstream>

TEST_CASE("Config parsing: minimal valid config", "[config]")
{
    const char *xml = R"(<config>
    <udp>
        <local_port>1234</local_port>
        <remote_ip>127.0.0.1</remote_ip>
        <remote_port>5678</remote_port>
    </udp>
    <uds>
        <server>/tmp/test1.sock</server>
        <client name="test.ul">/tmp/test2.sock</client>
    </uds>
    <ul_uds_mapping>
        <mapping opcode="1" uds="test.ul" />
    </ul_uds_mapping>
</config>)";
    std::ofstream f("test_config.xml");
    f << xml;
    f.close();

    AppConfig cfg = load_config("test_config.xml");
    REQUIRE(cfg.udp_local_port == 1234);
    REQUIRE(cfg.udp_remote_ip == "127.0.0.1");
    REQUIRE(cfg.udp_remote_port == 5678);
    REQUIRE(cfg.uds_servers.size() == 1);
    REQUIRE(cfg.uds_servers[0].path == "/tmp/test1.sock");
    REQUIRE(cfg.uds_clients.size() == 1);
    REQUIRE(cfg.uds_clients.at("test.ul") == "/tmp/test2.sock");
    REQUIRE(cfg.ul_uds_mapping.size() == 1);
    REQUIRE(cfg.ul_uds_mapping.at(1) == "test.ul");
    remove("test_config.xml");
}
