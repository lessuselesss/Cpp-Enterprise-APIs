#pragma once

/// @file cep_account.hpp
/// @brief Account management for Circular Protocol Enterprise APIs

#include <string>
#include <optional>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <circular/utils.hpp>

namespace circular {

/// @brief Represents a Circular Enterprise Protocol (CEP) account
///
/// This class holds all the necessary information and state for interacting
/// with the Circular network on behalf of a specific account, including its
/// address, public key, network configuration, and transaction-related data.
class CepAccount {
public:
    /// @brief Creates a new CepAccount instance with default initial values
    ///
    /// Most fields are initialized to empty strings or nullopt, while code_version,
    /// nag_url, blockchain, nonce, interval_sec, and network_url are
    /// set to their respective default or initial values.
    CepAccount();

    /// @brief Destructor
    ~CepAccount() = default;

    /// @brief Copy constructor (deleted - accounts should not be copied)
    CepAccount(const CepAccount&) = delete;

    /// @brief Move constructor
    CepAccount(CepAccount&&) = default;

    /// @brief Copy assignment operator (deleted - accounts should not be copied)
    CepAccount& operator=(const CepAccount&) = delete;

    /// @brief Move assignment operator
    CepAccount& operator=(CepAccount&&) = default;

    /// @brief Opens the account by setting its address
    ///
    /// This method validates the provided address and updates the account's
    /// internal state. If the address is empty, an error is set.
    ///
    /// @param address A string representing the hexadecimal address of the account
    /// @return true if the account was successfully opened, false otherwise
    bool open(const std::string& account_address);

    /// @brief Closes the account by clearing its sensitive and network-related information
    ///
    /// This resets the account to a default, uninitialized state, effectively
    /// logging out or disconnecting the account.
    void close();

    /// @brief Sets the network for the account by fetching the appropriate NAG URL
    ///
    /// This asynchronous method uses the provided network identifier to query
    /// the network discovery service and update the account's nag_url and
    /// network_node fields. If the NAG URL cannot be retrieved, an error
    /// is set and the method returns false.
    ///
    /// Note: The Rust implementation returns the NAG URL string. This C++ version
    /// returns a bool for consistent error handling. The NAG URL is stored in nag_url.
    ///
    /// @param network A string representing the network identifier (e.g., "testnet")
    /// @return A Task<bool> that resolves to true if the network was set successfully,
    ///         false if an error occurred (check get_last_error() for details)
    Task<bool> set_network(const std::string& network);

    /// @brief Sets the blockchain identifier for the account
    ///
    /// @param blockchain_address A string representing the blockchain identifier
    void set_blockchain(const std::string& blockchain_address);

    /// @brief Sets the network node identifier for the account
    ///
    /// @param node A string representing the network node identifier
    void set_network_node(const std::string& node);

    /// @brief Sets the polling interval for transaction outcome checks
    ///
    /// @param seconds The interval in seconds between polling attempts
    void set_interval(std::int32_t seconds);

    /// @brief Updates the account's nonce by querying the network
    ///
    /// This asynchronous method sends a request to the network to retrieve
    /// the latest nonce for the account's address. It handles various network
    /// responses and updates the nonce field accordingly. If the account
    /// is not open or a network error occurs, last_error is set.
    ///
    /// @return A Task<bool> that resolves to true if the account update was successful and the nonce was retrieved,
    ///         false otherwise
    Task<bool> update_account();

    /// @brief Submits a certificate to the Circular network
    ///
    /// This asynchronous method constructs a transaction payload, signs it
    /// using the provided private key, and sends it to the network via the
    /// configured NAG URL. It handles various network responses and updates
    /// the account's latest_tx_id and nonce upon successful submission.
    /// Errors encountered during the process are stored in last_error.
    ///
    /// Note: The Rust implementation returns void. This C++ version returns
    /// a bool for immediate success/failure feedback.
    ///
    /// @param pdata A string containing the payload data for the certificate
    /// @param private_key_hex A string containing the private key in hexadecimal format
    /// @return A Task<bool> that resolves to true if the submission was successful,
    ///         false if an error occurred (check get_last_error() for details)
    Task<bool> submit_certificate(const std::string& pdata, const std::string& private_key_hex);

    /// @brief Retrieves a transaction from the network by its block ID and transaction ID
    ///
    /// This asynchronous method queries the network for a specific transaction.
    /// It validates the block_id and handles potential parsing errors.
    /// The actual fetching is delegated to get_transaction_by_id.
    /// Errors encountered are stored in last_error.
    ///
    /// @param block_id A string representing the block ID where the transaction is located
    /// @param transaction_id A string representing the ID of the transaction to retrieve
    /// @return A Task<std::optional<nlohmann::json>> containing the transaction data as a JSON object if successful,
    ///         or std::nullopt if an error occurred or the transaction was not found
    Task<std::optional<nlohmann::json>> get_transaction(const std::string& block_id, const std::string& transaction_id);

    /// @brief Polls the network to get the outcome of a transaction within a specified timeout
    ///
    /// This asynchronous method repeatedly queries the network for the status of a
    /// transaction until it is no longer "Pending" or a timeout is reached. It uses
    /// get_transaction_by_id internally for polling. Errors encountered during
    /// polling or if a timeout occurs are stored in last_error.
    ///
    /// @param tx_id A string representing the ID of the transaction to poll
    /// @param timeout_sec The maximum time in seconds to wait for the transaction outcome
    /// @param interval_sec The interval in seconds between polling attempts
    /// @return A Task<std::optional<nlohmann::json>> containing the transaction outcome data as a JSON object
    ///         if the transaction is executed, or std::nullopt if a timeout occurs or an error
    ///         prevents retrieval
    Task<std::optional<nlohmann::json>> get_transaction_outcome(const std::string& tx_id, int timeout_sec, int poll_interval_sec);

    /// @brief Retrieves the last error message encountered by the account
    ///
    /// @return An std::optional<std::string> containing the error message if an error occurred,
    ///         or std::nullopt if there was no recent error
    std::optional<std::string> get_last_error() const;

    // Public fields (matching Rust implementation)

    /// @brief The hexadecimal address of the account
    std::string address;

    /// @brief The public key associated with the account, in hexadecimal format
    std::string public_key;

    /// @brief The version of the client code interacting with the network
    std::string code_version;

    /// @brief The URL of the Network Access Gateway (NAG) currently in use
    std::string nag_url;

    /// @brief The identifier of the network node being used (e.g., "testnet")
    std::string network_node;

    /// @brief The blockchain identifier the account is operating on
    std::string blockchain;

    /// @brief The ID of the most recently submitted transaction from this account
    std::string latest_tx_id;

    /// @brief A nonce value used for transaction ordering and replay protection
    std::int64_t nonce;

    /// @brief The interval in seconds for polling operations, such as transaction outcomes
    std::int32_t interval_sec;

    /// @brief The base URL for network discovery
    std::string network_url;

private:
    /// @brief Optional additional information about the account, typically in JSON format
    std::optional<nlohmann::json> info_;

    /// @brief Stores the last encountered error message, if any, during account operations
    std::optional<std::string> last_error_;

    /// @brief Signs a message using the account's private key
    ///
    /// This method takes a message and a hexadecimal private key, then uses
    /// secp256k1 to sign the message. The resulting signature is returned
    /// in hexadecimal format.
    ///
    /// @param message A string representing the message to be signed
    /// @param private_key_hex A string containing the private key in hexadecimal format
    /// @return A Result<std::string, std::string> which is:
    ///         - Ok(String) containing the hexadecimal signature if successful
    ///         - Err(String) containing an error message if the account is not open,
    ///           the private key is invalid, or the signing process fails
    Result<std::string, std::string> sign_data(const std::string& message, const std::string& private_key_hex) const;

    /// @brief Retrieves a transaction by its ID within a specified block range
    ///
    /// This method constructs and sends a request to the network to fetch transaction
    /// details within the specified block range. It handles network responses and JSON parsing.
    ///
    /// @param transaction_id A string representing the ID of the transaction
    /// @param start_block The starting block number for the search range
    /// @param end_block The ending block number for the search range
    /// @return A Task<Result<nlohmann::json, std::string>> which is:
    ///         - Ok(json) containing the transaction data as a JSON object if successful
    ///         - Err(string) containing an error message if the network is not set,
    ///           the network request fails, or JSON decoding fails
    Task<Result<nlohmann::json, std::string>> get_transaction_by_id(const std::string& transaction_id, std::int64_t start_block, std::int64_t end_block);
};

} // namespace circular