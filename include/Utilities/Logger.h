#pragma once

#include <iostream>
#include <string>
#include <format>

enum e_log_type {
    log_default,
    log_error,
    log_debug,
    log_status,
    log_sst_error,
    log_sst_status,
};

//TODO CHANGE THIS LOGGER CLASS TO A THREAD THAT YOU PUSH MESSAGES TO TO ALLOW MAIN THREAD TO RUN CORRECT SPEED

class Logger {
public:
    static void update_logger(bool status_enabled, bool debug_enabled) {
        status = status_enabled;
        debug = debug_enabled;
    }

    static void log(e_log_type type, const std::string& msg) {
        std::string prefix;
        switch (type) {
        case log_default:     
            prefix = "[CORE] ";
            break;

        case log_error:       
            prefix = "[CORE::ERROR] "; 
            break;

        case log_debug:
            if (!debug) {
                return;
            }
            prefix = "[CORE::DEBUG] ";
            break;

        case log_status:
            if (!status) {
                return;
            }
            prefix = "[CORE] ";
            break;

        case log_sst_error:   
            prefix = "[C/SST:ERROR] "; 
            break;

        case log_sst_status: 
            prefix = "[C/SST] "; 
            break;

        default:              
            prefix = "[CORE] "; 
            break;
        }

        std::cout << std::format("{}{}\n", prefix, msg);
    }

    static void log_sst_progress(bool is_cb, int test_number, int test_total) {
        float complete_per = (static_cast<float>(test_number) / test_total) * 100.0f;
        std::cout << std::format(
            "[C/SST] Progress: {}%  Test Number: {}{}\n",
            static_cast<int>(complete_per),
            is_cb ? "CB " : "",
            test_number
        );
    }

private:
    static bool status;
    static bool debug;
};

inline bool Logger::status = false;
inline bool Logger::debug = false;