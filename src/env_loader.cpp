#include <circular/env_loader.hpp>
#include <fstream>
#include <sstream>
#include <cstdlib>

namespace circular {

std::unordered_map<std::string, std::string> EnvLoader::env_vars_;

bool EnvLoader::load_env_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Find the = separator
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, equals_pos);
        std::string value = line.substr(equals_pos + 1);

        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Remove quotes if present
        if (value.size() >= 2 &&
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }

        env_vars_[key] = value;
    }

    return true;
}

std::optional<std::string> EnvLoader::get_env(const std::string& key) {
    // First check loaded .env file
    auto it = env_vars_.find(key);
    if (it != env_vars_.end()) {
        return it->second;
    }

    // Then check system environment
    const char* env_value = std::getenv(key.c_str());
    if (env_value != nullptr) {
        return std::string(env_value);
    }

    return std::nullopt;
}

std::string EnvLoader::get_env_or(const std::string& key, const std::string& default_value) {
    auto value = get_env(key);
    return value ? *value : default_value;
}

} // namespace circular