#pragma once
#include <string>
#include <cstdlib>
#include <libgen.h>

// Returns the absolute path to test_config.xml in the tests directory
inline std::string get_test_config_path()
{
    // Use __FILE__ to get the current file's path, then find the tests directory
    std::string file_path = __FILE__;
    size_t pos = file_path.rfind("/tests/");
    if (pos == std::string::npos)
        throw std::runtime_error("Cannot determine tests directory from __FILE__");
    std::string tests_dir = file_path.substr(0, pos + 7); // include '/tests/'
    return tests_dir + "test_config.xml";
}
