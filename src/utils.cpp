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

std::string pad_number(int num) {
    if (num >= 0 && num < 10) {
        return "0" + std::to_string(num);
    }
    return std::to_string(num);
}

std::string get_formatted_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::gmtime(&time_t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y:%m:%d-%H:%M:%S");
    return oss.str();
}

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

std::string encode_hex(const std::string& s) {
    std::ostringstream oss;
    for (unsigned char c : s) {
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return oss.str();
}

std::string hex_to_string(const std::string& hex_str) {
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
    /// This matches the Rust implementation's NETWORK_URL with Mutex protection
    class NetworkUrlManager {
    public:
        static NetworkUrlManager& instance() {
            static NetworkUrlManager instance;
            return instance;
        }

        std::string get_url() {
            std::lock_guard<std::mutex> lock(mutex_);
            return network_url_;
        }

        void set_url(const std::string& url) {
            std::lock_guard<std::mutex> lock(mutex_);
            network_url_ = url;
        }

    private:
        NetworkUrlManager() : network_url_(DEFAULT_NETWORK_URL) {}

        std::mutex mutex_;
        std::string network_url_;
    };
}

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