#include <circular/ccertificate.hpp>
#include <circular/circular_enterprise_apis.hpp>
#include <circular/utils.hpp>

namespace circular {

/// @brief Default constructor initializing certificate with empty data
CCertificate::CCertificate()
    : data_("")
    , previous_tx_id_("")
    , previous_block_("")
    , version_(LIB_VERSION)
{
}

/// @brief Sets the data content of the certificate
/// @param data A string containing the data to store in the certificate
void CCertificate::set_data(const std::string& data) {
    data_ = str_to_hex(data);
}

/// @brief Retrieves the certificate's data content
/// @return The data string stored in the certificate
std::string CCertificate::get_data() const {
    return hex_to_str(data_);
}

/// @brief Generates a JSON string representation of the certificate
/// @return A JSON-formatted string, or empty string on serialization error
std::string CCertificate::get_json_certificate() const {
    try {
        nlohmann::json json_cert = to_json();
        return json_cert.dump();
    } catch (const std::exception&) {
        return ""; // Return empty string on error
    }
}

/// @brief Calculates the size of the certificate's JSON representation
/// @return The length of the JSON string in bytes
size_t CCertificate::get_certificate_size() const {
    return get_json_certificate().length();
}

/// @brief Sets the ID of the previous transaction
/// @param tx_id The transaction ID to store
void CCertificate::set_previous_tx_id(const std::string& tx_id) {
    previous_tx_id_ = tx_id;
}

/// @brief Sets the previous block identifier
/// @param block The block identifier to store
void CCertificate::set_previous_block(const std::string& block) {
    previous_block_ = block;
}

/// @brief Retrieves the previous transaction ID
/// @return The stored previous transaction ID
std::string CCertificate::get_previous_tx_id() const {
    return previous_tx_id_;
}

/// @brief Retrieves the previous block identifier
/// @return The stored previous block identifier
std::string CCertificate::get_previous_block() const {
    return previous_block_;
}

/// @brief Converts the certificate to a JSON object
/// @return A JSON object containing the certificate data and version
nlohmann::json CCertificate::to_json() const {
    nlohmann::json json_cert;
    json_cert["data"] = data_;
    json_cert["previousTxID"] = previous_tx_id_;
    json_cert["previousBlock"] = previous_block_;
    json_cert["version"] = version_;
    return json_cert;
}

} // namespace circular