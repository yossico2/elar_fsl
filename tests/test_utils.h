#pragma once
#include <string>
#include <cstdlib>
#include <libgen.h>
#include <fstream>

// Returns the absolute path to config.xml
inline std::string get_test_config_path()
{
    // First, check if config.xml exists in the current directory
    const char *local_config = "config.xml";
    if (std::ifstream(local_config))
        return std::string(local_config);

    // Otherwise, try to find the output directory relative to __FILE__
    std::string file_path = __FILE__;
    size_t pos = file_path.rfind("/build/");
    if (pos == std::string::npos)
        throw std::runtime_error("Cannot determine build directory from __FILE__");
    std::string build_dir = file_path.substr(0, pos + 7); // include '/build/'
    // Assume config.xml is in the output directory (one level up from build/)
    std::string output_dir = build_dir.substr(0, build_dir.length() - 6); // remove 'build/'
    return output_dir + "src/config.xml";
}
