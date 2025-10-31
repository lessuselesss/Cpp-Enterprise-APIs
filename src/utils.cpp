#include <circular/utils.hpp>
#include <circular/circular_enterprise_apis.hpp>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <thread>
#include <httplib.h>
#include <nlohmann/json.hpp>

namespace circular {

/// @brief Pads a number with a leading zero if it is a single digit
/// @param num An integer to be padded
/// @return A string representation of the number, padded with leading zero if 0 <= num < 10
std::string pad_number(int num) {
    if (num >= 0 && num < 10) {
        return "0" + std::to_string(num);
    }
    return std::to_string(num);
}

/// @brief Generates a formatted timestamp string in "YYYY:MM:DD-HH:MM:SS" format
/// @return A string containing the current UTC timestamp
std::string get_formatted_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::gmtime(&time_t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y:%m:%d-%H:%M:%S");
    return oss.str();
}

/// @brief Cleans and normalizes a hexadecimal string
/// @param hex_str A string representing the hexadecimal value to normalize
/// @return A normalized lowercase hex string without "0x" prefix and with even length
std::string hex_fix(const std::string& hex_str) {
    if (hex_str.empty()) {
        return "";
    }

    std::string s = hex_str;

    // Remove "0x" or "0X" prefix
    if (s.length() >= 2 && (s.substr(0, 2) == "0x" || s.substr(0, 2) == "0X")) {
        s = s.substr(2);
    }

    // Convert to lowercase
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    // Pad with '0' if length is odd
    if (s.length() % 2 != 0) {
        s = "0" + s;
    }

    return s;
}

/// @brief Converts a string to its hexadecimal representation
/// @param s A string to be converted to hexadecimal
/// @return An uppercase hexadecimal string representing the input bytes
std::string str_to_hex(const std::string& s) {
    std::ostringstream oss;
    for (unsigned char c : s) {
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return oss.str();
}

/// @brief Converts a hexadecimal string back to its original string representation
/// @param hex_str A string representing the hexadecimal data to decode
/// @return The decoded string, or empty string if input is invalid
std::string hex_to_str(const std::string& hex_str) {
    if (hex_str.empty()) {
        return "";
    }

    std::string s = hex_str;

    // Remove "0x" or "0X" prefix if present
    if (s.length() >= 2 && (s.substr(0, 2) == "0x" || s.substr(0, 2) == "0X")) {
        s = s.substr(2);
    }

    // Check if the string has valid hex characters and even length
    if (s.length() % 2 != 0) {
        return ""; // Invalid hex string
    }

    for (char c : s) {
        if (!std::isxdigit(c)) {
            return ""; // Invalid hex character
        }
    }

    std::string result;
    result.reserve(s.length() / 2);

    for (size_t i = 0; i < s.length(); i += 2) {
        std::string byte_string = s.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byte_string, nullptr, 16));
        result.push_back(byte);
    }

    return result;
}

namespace {
    /// @brief Thread-safe access to the network URL
    ///
    /// Provides mutex-protected access to the network discovery URL
    class NetworkUrlManager {
    public:
        /// @brief Returns the singleton instance of NetworkUrlManager
        /// @return Reference to the singleton instance
        static NetworkUrlManager& instance() {
            static NetworkUrlManager instance;
            return instance;
        }

        /// @brief Gets the current network URL
        /// @return The current network discovery URL (thread-safe)
        std::string get_url() {
            std::lock_guard<std::mutex> lock(mutex_);
            return network_url_;
        }

        /// @brief Sets the network URL
        /// @param url The new network discovery URL to use (thread-safe)
        void set_url(const std::string& url) {
            std::lock_guard<std::mutex> lock(mutex_);
            network_url_ = url;
        }

    private:
        /// @brief Private constructor for singleton pattern
        NetworkUrlManager() : network_url_(DEFAULT_NETWORK_URL) {}

        std::mutex mutex_;
        std::string network_url_;
    };
}

/// @brief Fetches the Network Access Gateway (NAG) URL for a given network identifier
/// @param network A string representing the network identifier (e.g., "testnet", "mainnet")
/// @return A Task that resolves to a Result containing the NAG URL on success, or error message on failure
Task<Result<std::string, std::string>> get_nag(const std::string& network) {
    return std::async(std::launch::async, [network]() -> Result<std::string, std::string> {
        if (network.empty()) {
            return Result<std::string, std::string>::Err("network identifier cannot be empty");
        }

        std::string request_url = NetworkUrlManager::instance().get_url() + network;

        // Parse URL to extract host and path
        httplib::Client client("https://circularlabs.io");
        client.set_connection_timeout(30, 0); // 30 seconds timeout
        client.set_read_timeout(30, 0);

        std::string path = "/network/getNAG?network=" + network;
        auto response = client.Get(path.c_str());

        if (!response) {
            return Result<std::string, std::string>::Err("failed to fetch NAG URL: network request failed");
        }

        if (response->status != 200) {
            return Result<std::string, std::string>::Err("network discovery failed with status: " + std::to_string(response->status));
        }

        try {
            auto json_response = nlohmann::json::parse(response->body);

            std::string status = json_response.value("status", "");
            std::string url = json_response.value("url", "");
            std::string message = json_response.value("message", "");

            if (status == "error") {
                return Result<std::string, std::string>::Err("failed to get valid NAG URL from response: " + message);
            }

            if (status != "success" || url.empty()) {
                return Result<std::string, std::string>::Err("failed to get valid NAG URL from response: " + message);
            }

            return Result<std::string, std::string>::Ok(url);
        } catch (const nlohmann::json::exception& e) {
            return Result<std::string, std::string>::Err("failed to unmarshal NAG response: " + std::string(e.what()));
        }
    });
}

} // namespace circular