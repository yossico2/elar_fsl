#pragma once
#include <cstdlib>
#include <string>
#include <cstring>

inline bool is_k8s_mode()
{
    const char *env = std::getenv("K8S_MODE");
    return env && std::string(env) == "true";
}

// get_instance_from_args_env: Get instance number from command line or environment
// Priority: -i/--instance arg > positional integer arg > STATEFULSET_INDEX env var
inline int get_instance_from_args_env(int argc, char *argv[])
{
    int instance = -1;

    if (instance < 0)
    {
        // Try to parse HOSTNAME env var (defined by kubernetes for statefulsets)
        // e.g: sensor-sim-0, sensor-sim-1, etc.
        const char *hostname = std::getenv("HOSTNAME");
        if (hostname)
        {
            std::string host_str(hostname);
            size_t dash_pos = host_str.rfind('-');
            if (dash_pos != std::string::npos && dash_pos + 1 < host_str.size())
            {
                std::string index_str = host_str.substr(dash_pos + 1);
                bool is_number = true;
                for (char c : index_str)
                {
                    if (!std::isdigit(c))
                    {
                        is_number = false;
                        break;
                    }
                }

                if (is_number)
                {
                    instance = std::atoi(index_str.c_str());
                }
            }
        }
    }

    if (instance < 0)
    {
        // Check command line args for instance
        for (int i = 1; i < argc; ++i)
        {
            if ((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--instance") == 0) && i + 1 < argc)
            {
                instance = std::atoi(argv[i + 1]);
                break;
            }

            // Accept the first positional integer argument (not an option)
            if (instance < 0 && argv[i][0] != '-' && std::atoi(argv[i]) >= 0)
            {
                instance = std::atoi(argv[i]);
                break;
            }
        }
    }

    // If not set, check STATEFULSET_INDEX env var
    if (instance < 0)
    {
        const char *env = std::getenv("STATEFULSET_INDEX");
        if (env)
            instance = std::atoi(env);
    }

    return instance;
}
