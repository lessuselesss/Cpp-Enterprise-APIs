#include <circular/cep_account.hpp>
#include <circular/circular_enterprise_apis.hpp>
#include <circular/utils.hpp>

#include <secp256k1.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstring>
#include <vector>

namespace circular {

namespace {
    // Internal helper functions

    /// @brief Convert hex string to bytes
    /// @param hex The hexadecimal string to convert (with or without "0x" prefix)
    /// @return A vector of bytes representing the hex string
    std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
        std::vector<uint8_t> bytes;
        std::string clean_hex = hex_fix(hex);

        for (size_t i = 0; i < clean_hex.length(); i += 2) {
            std::string byte_string = clean_hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoi(byte_string, nullptr, 16));
            bytes.push_back(byte);
        }
        return bytes;
    }

    /// @brief Convert bytes to hex string
    /// @param bytes The vector of bytes to convert
    /// @return A lowercase hexadecimal string representation
    std::string bytes_to_hex(const std::vector<uint8_t>& bytes) {
        std::ostringstream oss;
        for (uint8_t byte : bytes) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return oss.str();
    }

    /// @brief Compute SHA256 hash
    /// @param data The input string to hash
    /// @return A 32-byte vector containing the SHA256 hash
    std::vector<uint8_t> sha256(const std::string& data) {
        std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
        SHA256_CTX sha256_ctx;
        SHA256_Init(&sha256_ctx);
        SHA256_Update(&sha256_ctx, data.c_str(), data.length());
        SHA256_Final(hash.data(), &sha256_ctx);
        return hash;
    }

    /// @brief Network client for HTTP operations
    class NetworkClient {
    public:
        /// @brief Performs an async POST request with JSON data
        /// @param url The HTTPS URL to request
        /// @param data The JSON data to send in the request body
        /// @return Task that resolves to Result containing parsed JSON response or error message
        static Task<Result<nlohmann::json, std::string>> post_json(const std::string& url, const nlohmann::json& data) {
            return std::async(std::launch::async, [url, data]() -> Result<nlohmann::json, std::string> {
                try {
                    // Parse URL to extract host and path
                    std::string host, path;
                    if (!parse_url(url, host, path)) {
                        return Result<nlohmann::json, std::string>::Err("invalid URL format");
                    }

                    httplib::Client client(("https://" + host).c_str());
                    client.set_connection_timeout(30, 0);
                    client.set_read_timeout(30, 0);

                    std::string json_str = data.dump();
                    auto response = client.Post(path.c_str(), json_str, "application/json");

                    if (!response) {
                        return Result<nlohmann::json, std::string>::Err("network request failed");
                    }

                    if (response->status != 200) {
                        return Result<nlohmann::json, std::string>::Err("network request failed with status: " + std::to_string(response->status));
                    }

                    auto json_response = nlohmann::json::parse(response->body);
                    return Result<nlohmann::json, std::string>::Ok(json_response);

                } catch (const nlohmann::json::exception& e) {
                    return Result<nlohmann::json, std::string>::Err("failed to decode response JSON: " + std::string(e.what()));
                } catch (const std::exception& e) {
                    return Result<nlohmann::json, std::string>::Err("http post failed: " + std::string(e.what()));
                }
            });
        }

    private:
        /// @brief Parses an HTTPS URL into host and path components
        /// @param url The full HTTPS URL to parse
        /// @param host Output parameter for the hostname
        /// @param path Output parameter for the path (defaults to "/" if not present)
        /// @return true if parsing succeeded, false if URL format is invalid
        static bool parse_url(const std::string& url, std::string& host, std::string& path) {
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
}

CepAccount::CepAccount()
    : address("")
    , public_key("")
    , code_version(LIB_VERSION)
    , nag_url(DEFAULT_NAG)
    , network_node("")
    , blockchain(DEFAULT_CHAIN)
    , latest_tx_id("")
    , nonce(0)
    , interval_sec(2)
    , network_url(DEFAULT_NETWORK_URL)
    , info_(std::nullopt)
    , last_error_(std::nullopt)
{
}

bool CepAccount::open(const std::string& account_address) {
    if (account_address.empty()) {
        last_error_ = "invalid address format";
        return false;
    }
    this->address = account_address;
    return true;
}

void CepAccount::close() {
    address = "";
    public_key = "";
    info_ = std::nullopt;
    nag_url = "";
    network_node = "";
    blockchain = "";
    latest_tx_id = "";
    nonce = 0;
    interval_sec = 0;
}

Task<bool> CepAccount::set_network(const std::string& network) {
    return std::async(std::launch::async, [this, network]() -> bool {
        auto result = get_nag(network).get();
        if (result.has_value()) {
            nag_url = result.value();
            network_node = network;
            return true;
        } else {
            last_error_ = result.error();
            return false;
        }
    });
}

void CepAccount::set_blockchain(const std::string& blockchain_address) {
    blockchain = blockchain_address;
}

void CepAccount::set_network_node(const std::string& node) {
    network_node = node;
}

void CepAccount::set_interval(std::int32_t seconds) {
    interval_sec = seconds;
}

Task<bool> CepAccount::update_account() {
    return std::async(std::launch::async, [this]() -> bool {
        if (address.empty()) {
            last_error_ = "Account not open";
            return false;
        }

        nlohmann::json request_data = {
            {"Address", hex_fix(address)},
            {"Version", code_version},
            {"Blockchain", hex_fix(blockchain)}
        };

        std::string url = nag_url + "Circular_GetWalletNonce_" + network_node;
        auto result = NetworkClient::post_json(url, request_data).get();

        if (!result.has_value()) {
            last_error_ = result.error();
            return false;
        }

        auto data = result.value();
        if (data.contains("Result") && data["Result"].is_number_integer()) {
            int result_code = data["Result"];
            if (result_code == 200) {
                if (data.contains("Response") && data["Response"].contains("Nonce") &&
                    data["Response"]["Nonce"].is_number_integer()) {
                    nonce = data["Response"]["Nonce"].get<std::int64_t>() + 1;
                    return true;
                } else {
                    last_error_ = "failed to decode nonce response";
                    return false;
                }
            } else if (result_code == 114) {
                last_error_ = "Rejected: Invalid Blockchain";
                return false;
            } else if (result_code == 115) {
                last_error_ = "Rejected: Insufficient balance";
                return false;
            } else {
                if (data.contains("Response") && data["Response"].is_string()) {
                    last_error_ = "failed to update account: " + data["Response"].get<std::string>();
                } else {
                    last_error_ = "failed to update account: unknown error response";
                }
                return false;
            }
        } else {
            last_error_ = "failed to get result from response";
            return false;
        }
    });
}

Result<std::string, std::string> CepAccount::sign_data(const std::string& message, const std::string& private_key_hex) const {
    if (address.empty()) {
        return Result<std::string, std::string>::Err("account is not open");
    }

    try {
        // Decode private key
        auto private_key_bytes = hex_to_bytes(private_key_hex);
        if (private_key_bytes.size() != 32) {
            return Result<std::string, std::string>::Err("private key must be 32 bytes long");
        }

        // Initialize secp256k1 context
        secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
        if (!ctx) {
            return Result<std::string, std::string>::Err("failed to create secp256k1 context");
        }

        // Verify private key
        if (!secp256k1_ec_seckey_verify(ctx, private_key_bytes.data())) {
            secp256k1_context_destroy(ctx);
            return Result<std::string, std::string>::Err("invalid private key");
        }

        // Hash the message
        auto hash = sha256(message);

        // Sign the hash
        secp256k1_ecdsa_signature sig;
        if (!secp256k1_ecdsa_sign(ctx, &sig, hash.data(), private_key_bytes.data(), nullptr, nullptr)) {
            secp256k1_context_destroy(ctx);
            return Result<std::string, std::string>::Err("failed to sign message");
        }

        // Serialize signature to DER format
        uint8_t der_sig[72]; // Maximum DER signature size
        size_t der_sig_len = sizeof(der_sig);
        if (!secp256k1_ecdsa_signature_serialize_der(ctx, der_sig, &der_sig_len, &sig)) {
            secp256k1_context_destroy(ctx);
            return Result<std::string, std::string>::Err("failed to serialize signature");
        }

        secp256k1_context_destroy(ctx);

        // Convert to hex
        std::vector<uint8_t> sig_bytes(der_sig, der_sig + der_sig_len);
        std::string sig_hex = bytes_to_hex(sig_bytes);

        // Debug logging for signature verification
        std::ofstream debug_file("rust_sign_debug.log", std::ios::app);
        if (debug_file.is_open()) {
            debug_file << "C++ SignData Debug:\n";
            debug_file << "  Message: " << message << "\n";
            debug_file << "  Private Key (hex): " << private_key_hex << "\n";
            debug_file << "  Message Hash (hex): " << bytes_to_hex(hash) << "\n";
            debug_file << "  Signature (hex): " << sig_hex << "\n\n";
        }

        return Result<std::string, std::string>::Ok(sig_hex);

    } catch (const std::exception& e) {
        return Result<std::string, std::string>::Err("signing error: " + std::string(e.what()));
    }
}

Task<bool> CepAccount::submit_certificate(const std::string& pdata, const std::string& private_key_hex) {
    return std::async(std::launch::async, [this, pdata, private_key_hex]() -> bool {
        if (address.empty()) {
            last_error_ = "Account is not open";
            return false;
        }

        // Create payload object
        nlohmann::json payload_object = {
            {"Action", "CP_CERTIFICATE"},
            {"Data", str_to_hex(pdata)}
        };
        std::string payload = str_to_hex(payload_object.dump());
        std::string timestamp = get_formatted_timestamp();

        // Create string to hash
        std::string str_to_hash = hex_fix(blockchain) + hex_fix(address) + hex_fix(address) +
                                  payload + std::to_string(nonce) + timestamp;

        auto hash = sha256(str_to_hash);
        std::string id = bytes_to_hex(hash);

        // Sign the ID
        auto signature_result = sign_data(id, private_key_hex);
        if (!signature_result.has_value()) {
            last_error_ = "failed to sign data: " + signature_result.error();
            return false;
        }

        // Create request data
        nlohmann::json request_data = {
            {"ID", id},
            {"From", hex_fix(address)},
            {"To", hex_fix(address)},
            {"Timestamp", timestamp},
            {"Payload", payload},
            {"Nonce", std::to_string(nonce)},
            {"Signature", signature_result.value()},
            {"Blockchain", hex_fix(blockchain)},
            {"Type", "C_TYPE_CERTIFICATE"},
            {"Version", code_version}
        };

        // Submit to network
        std::string url = nag_url + "Circular_AddTransaction_" + network_node;
        auto result = NetworkClient::post_json(url, request_data).get();

        if (!result.has_value()) {
            last_error_ = result.error();
            return false;
        }

        auto data = result.value();
        if (data.contains("Result") && data["Result"].is_number_integer()) {
            int result_code = data["Result"];
            if (result_code == 200) {
                latest_tx_id = id;
                nonce += 1;
                return true;
            } else {
                if (data.contains("Response") && data["Response"].is_string()) {
                    last_error_ = "certificate submission failed: " + data["Response"].get<std::string>();
                } else {
                    last_error_ = "certificate submission failed with non-200 result code";
                }
                return false;
            }
        }
        return false;
    });
}

Task<std::optional<nlohmann::json>> CepAccount::get_transaction(const std::string& block_id, const std::string& transaction_id) {
    return std::async(std::launch::async, [this, block_id, transaction_id]() -> std::optional<nlohmann::json> {
        if (block_id.empty()) {
            last_error_ = "blockID cannot be empty";
            return std::nullopt;
        }

        std::int64_t start_block;
        try {
            start_block = std::stoll(block_id);
        } catch (const std::exception&) {
            last_error_ = "invalid blockID";
            return std::nullopt;
        }

        auto result = get_transaction_by_id(transaction_id, start_block, start_block).get();
        if (result.has_value()) {
            return result.value();
        } else {
            last_error_ = "failed to get transaction by ID: " + result.error();
            return std::nullopt;
        }
    });
}

Task<Result<nlohmann::json, std::string>> CepAccount::get_transaction_by_id(const std::string& transaction_id, std::int64_t start_block, std::int64_t end_block) {
    return std::async(std::launch::async, [this, transaction_id, start_block, end_block]() -> Result<nlohmann::json, std::string> {
        if (nag_url.empty()) {
            return Result<nlohmann::json, std::string>::Err("network is not set");
        }

        nlohmann::json request_data = {
            {"Blockchain", hex_fix(blockchain)},
            {"ID", hex_fix(transaction_id)},
            {"Start", std::to_string(start_block)},
            {"End", std::to_string(end_block)},
            {"Version", code_version}
        };

        std::string url = nag_url + "Circular_GetTransactionbyID_" + network_node;
        auto network_result = NetworkClient::post_json(url, request_data).get();
        if (network_result.has_value()) {
            return Result<nlohmann::json, std::string>::Ok(network_result.value());
        } else {
            return Result<nlohmann::json, std::string>::Err(network_result.error());
        }
    });
}

Task<std::optional<nlohmann::json>> CepAccount::get_transaction_outcome(const std::string& tx_id, int timeout_sec, int poll_interval_sec) {
    return std::async(std::launch::async, [this, tx_id, timeout_sec, poll_interval_sec]() -> std::optional<nlohmann::json> {
        if (nag_url.empty()) {
            last_error_ = "network is not set";
            return std::nullopt;
        }

        auto start_time = std::chrono::steady_clock::now();
        auto timeout = std::chrono::seconds(timeout_sec);
        auto interval = std::chrono::seconds(poll_interval_sec);

        while (true) {
            auto elapsed = std::chrono::steady_clock::now() - start_time;
            if (elapsed > timeout) {
                last_error_ = "timeout exceeded while waiting for transaction outcome";
                return std::nullopt;
            }

            auto result = get_transaction_by_id(tx_id, 0, 10).get();
            if (result.has_value()) {
                auto data = result.value();
                if (data.contains("Result") && data["Result"].is_number_integer()) {
                    int result_code = data["Result"];
                    if (result_code == 200) {
                        if (data.contains("Response")) {
                            auto response = data["Response"];
                            if (response.contains("Status") && response["Status"].is_string()) {
                                std::string status = response["Status"];
                                if (status != "Pending") {
                                    return response;
                                }
                            }
                        }
                    }
                }
            }

            std::this_thread::sleep_for(interval);
        }
    });
}

std::optional<std::string> CepAccount::get_last_error() const {
    return last_error_;
}

} // namespace circular