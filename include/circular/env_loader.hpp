#pragma once

/// @file env_loader.hpp
/// @brief Simple .env file loader utility for development convenience

#include <string>
#include <unordered_map>
#include <optional>

namespace circular {

/// @brief Simple .env file loader for development convenience
class EnvLoader {
public:
    /// @brief Load environment variables from a .env file
    /// @param filename Path to the .env file (default: ".env")
    /// @return True if the file was successfully loaded, false if file doesn't exist or has errors
    static bool load_env_file(const std::string& filename = ".env");

    /// @brief Get an environment variable, checking both .env file and system environment
    /// @param key Environment variable name
    /// @return Value if found, std::nullopt if not found
    static std::optional<std::string> get_env(const std::string& key);

    /// @brief Get an environment variable with a default value
    /// @param key Environment variable name
    /// @param default_value Default value if not found
    /// @return Value if found, default_value if not found
    static std::string get_env_or(const std::string& key, const std::string& default_value);

private:
    static std::unordered_map<std::string, std::string> env_vars_;
};

} // namespace circular