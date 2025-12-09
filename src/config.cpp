#include "config.h"
#include <stdexcept>

// Include TinyXML-2 header
// Ensure libtinyxml2-dev is installed on your PetaLinux rootfs
#include <tinyxml2.h>

using namespace tinyxml2;

AppConfig load_config(const char *filename)
{
    XMLDocument doc;
    XMLError result = doc.LoadFile(filename);
    if (result != XML_SUCCESS)
    {
        throw std::runtime_error("Failed to load XML file. Check if file exists and is valid.");
    }

    XMLElement *root = doc.FirstChildElement("config");
    if (root == nullptr)
        throw std::runtime_error("Invalid XML: Missing <config> root element");

    AppConfig config;

    // --- Parse UDP Settings ---
    XMLElement *udp_node = root->FirstChildElement("udp");
    if (udp_node)
    {
        if (udp_node->FirstChildElement("local_port")->QueryIntText(&config.udp_local_port) != XML_SUCCESS)
            throw std::runtime_error("Missing or invalid <local_port>");
        if (udp_node->FirstChildElement("remote_port")->QueryIntText(&config.udp_remote_port) != XML_SUCCESS)
            throw std::runtime_error("Missing or invalid <remote_port>");
        const char *ip_text = udp_node->FirstChildElement("remote_ip")->GetText();
        if (ip_text)
            config.udp_remote_ip = ip_text;
        else
            throw std::runtime_error("Missing <remote_ip>");
    }
    else
    {
        throw std::runtime_error("Missing <udp> section");
    }

    // --- Parse UDS Settings ---
    XMLElement *uds_node = root->FirstChildElement("uds");
    if (uds_node)
    {
        const char *my_path_text = uds_node->FirstChildElement("my_path")->GetText();
        const char *target_path_text = uds_node->FirstChildElement("target_path")->GetText();
        if (my_path_text)
            config.uds_my_path = my_path_text;
        else
            throw std::runtime_error("Missing <my_path>");
        if (target_path_text)
            config.uds_target_path = target_path_text;
        else
            throw std::runtime_error("Missing <target_path>");
    }
    else
    {
        throw std::runtime_error("Missing <uds> section");
    }

    return config;
}
