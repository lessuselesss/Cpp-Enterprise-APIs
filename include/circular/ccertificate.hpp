#pragma once

/// @file ccertificate.hpp
/// @brief Certificate management for Circular Protocol Enterprise APIs

#include <string>
#include <nlohmann/json.hpp>

namespace circular {

/// @brief Represents a Circular Enterprise API (CEP) certificate
///
/// This class encapsulates the data, previous transaction ID, previous block,
/// and version information for a certificate within the Circular Protocol.
/// It is designed to be serialized to JSON for submission to the network.
class CCertificate {
public:
    /// @brief Creates a new CCertificate instance with default empty values
    ///
    /// The version field is initialized with the LIB_VERSION constant.
    CCertificate();

    /// @brief Destructor
    ~CCertificate() = default;

    /// @brief Copy constructor
    CCertificate(const CCertificate&) = default;

    /// @brief Move constructor
    CCertificate(CCertificate&&) = default;

    /// @brief Copy assignment operator
    CCertificate& operator=(const CCertificate&) = default;

    /// @brief Move assignment operator
    CCertificate& operator=(CCertificate&&) = default;

    /// @brief Sets the data content of the certificate
    ///
    /// The provided data is automatically hex-encoded before storage.
    /// This matches the Rust implementation's behavior.
    ///
    /// @param data A string containing the data to be set
    void set_data(const std::string& data);

    /// @brief Retrieves the data content of the certificate
    ///
    /// The stored hex-encoded data is automatically decoded and returned
    /// as the original string. This matches the Rust implementation's behavior.
    ///
    /// @return A string containing the decoded data
    std::string get_data() const;

    /// @brief Returns the JSON string representation of the certificate
    ///
    /// This method serializes the CCertificate object into a JSON string.
    ///
    /// @return A string containing the JSON representation of the certificate.
    ///         Returns an empty string if serialization fails.
    std::string get_json_certificate() const;

    /// @brief Calculates the size of the JSON string representation of the certificate
    ///
    /// @return A size_t representing the length of the JSON string
    size_t get_certificate_size() const;

    /// @brief Sets the previous transaction ID for the certificate
    ///
    /// @param tx_id A string containing the previous transaction ID
    void set_previous_tx_id(const std::string& tx_id);

    /// @brief Sets the previous block identifier for the certificate
    ///
    /// @param block A string containing the previous block identifier
    void set_previous_block(const std::string& block);

    /// @brief Retrieves the previous transaction ID of the certificate
    ///
    /// @return A string containing the previous transaction ID
    std::string get_previous_tx_id() const;

    /// @brief Retrieves the previous block identifier of the certificate
    ///
    /// @return A string containing the previous block identifier
    std::string get_previous_block() const;

private:
    /// @brief The main data content of the certificate, typically hex-encoded
    std::string data_;

    /// @brief The ID of the previous transaction in the blockchain, if applicable
    std::string previous_tx_id_;

    /// @brief The identifier of the previous block in the blockchain, if applicable
    std::string previous_block_;

    /// @brief The version of the certificate format or the library used to create it
    std::string version_;

    /// @brief Converts the certificate to a JSON object for serialization
    ///
    /// @return A nlohmann::json object representing the certificate
    nlohmann::json to_json() const;
};

} // namespace circular