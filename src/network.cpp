#include <circular/utils.hpp>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <thread>
#include <chrono>

namespace circular {

namespace network {

/// @brief Internal HTTP client wrapper for async operations
class HttpClient {
public:
    /// @brief Perform an async GET request
    static Task<Result<nlohmann::json, std::string>> get_json(const std::string& url) {
        return std::async(std::launch::async, [url]() -> Result<nlohmann::json, std::string> {
            return perform_get_request(url);
        });
    }

    /// @brief Perform an async POST request with JSON data
    static Task<Result<nlohmann::json, std::string>> post_json(const std::string& url, const nlohmann::json& data) {
        return std::async(std::launch::async, [url, data]() -> Result<nlohmann::json, std::string> {
            return perform_post_request(url, data);
        });
    }

private:
    /// @brief Performs a synchronous GET request and parses JSON response
    /// @param url The HTTPS URL to request
    /// @return Result containing parsed JSON on success, or error message on failure
    static Result<nlohmann::json, std::string> perform_get_request(const std::string& url) {
        try {
            // Parse URL - assume HTTPS
            std::string host, path;
            if (!parse_url(url, host, path)) {
                return Result<nlohmann::json, std::string>::Err("invalid URL format");
            }

            httplib::Client client(("https://" + host).c_str());
            client.set_connection_timeout(30, 0); // 30 seconds
            client.set_read_timeout(30, 0);

            auto response = client.Get(path.c_str());
            if (!response) {
                return Result<nlohmann::json, std::string>::Err("network request failed");
            }

            if (response->status != 200) {
                return Result<nlohmann::json, std::string>::Err("HTTP request failed with status: " + std::to_string(response->status));
            }

            auto json_response = nlohmann::json::parse(response->body);
            return Result<nlohmann::json, std::string>::Ok(json_response);

        } catch (const nlohmann::json::exception& e) {
            return Result<nlohmann::json, std::string>::Err("failed to parse JSON response: " + std::string(e.what()));
        } catch (const std::exception& e) {
            return Result<nlohmann::json, std::string>::Err("network error: " + std::string(e.what()));
        }
    }

    /// @brief Performs a synchronous POST request with JSON data and parses JSON response
    /// @param url The HTTPS URL to request
    /// @param data The JSON data to send in the request body
    /// @return Result containing parsed JSON response on success, or error message on failure
    static Result<nlohmann::json, std::string> perform_post_request(const std::string& url, const nlohmann::json& data) {
        try {
            // Parse URL - assume HTTPS
            std::string host, path;
            if (!parse_url(url, host, path)) {
                return Result<nlohmann::json, std::string>::Err("invalid URL format");
            }

            httplib::Client client(("https://" + host).c_str());
            client.set_connection_timeout(30, 0); // 30 seconds
            client.set_read_timeout(30, 0);

            std::string json_str = data.dump();
            auto response = client.Post(path.c_str(), json_str, "application/json");

            if (!response) {
                return Result<nlohmann::json, std::string>::Err("network request failed");
            }

            if (response->status != 200) {
                return Result<nlohmann::json, std::string>::Err("HTTP request failed with status: " + std::to_string(response->status));
            }

            auto json_response = nlohmann::json::parse(response->body);
            return Result<nlohmann::json, std::string>::Ok(json_response);

        } catch (const nlohmann::json::exception& e) {
            return Result<nlohmann::json, std::string>::Err("failed to parse JSON response: " + std::string(e.what()));
        } catch (const std::exception& e) {
            return Result<nlohmann::json, std::string>::Err("network error: " + std::string(e.what()));
        }
    }

    /// @brief Parses an HTTPS URL into host and path components
    /// @param url The full HTTPS URL to parse
    /// @param host Output parameter for the hostname
    /// @param path Output parameter for the path (defaults to "/" if not present)
    /// @return true if parsing succeeded, false if URL format is invalid
    static bool parse_url(const std::string& url, std::string& host, std::string& path) {
        // Simple URL parsing for HTTPS URLs
        const std::string https_prefix = "https://";

        if (url.substr(0, https_prefix.length()) != https_prefix) {
            return false;
        }

        std::string url_without_protocol = url.substr(https_prefix.length());
        size_t slash_pos = url_without_protocol.find('/');

        if (slash_pos == std::string::npos) {
            host = url_without_protocol;
            path = "/";
        } else {
            host = url_without_protocol.substr(0, slash_pos);
            path = url_without_protocol.substr(slash_pos);
        }

        return !host.empty();
    }
};

/// @brief Sleep for a specified duration asynchronously
Task<void> async_sleep(std::chrono::milliseconds duration) {
    return std::async(std::launch::async, [duration]() {
        std::this_thread::sleep_for(duration);
    });
}

} // namespace network

} // namespace circular