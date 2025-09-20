#include <circular/circular_enterprise_apis.hpp>
#include <iostream>
#include <string>
#include <cstdlib>

int main() {
    // Load environment variables from .env file (if it exists)
    circular::EnvLoader::load_env_file();

    auto private_key = circular::EnvLoader::get_env("CIRCULAR_PRIVATE_KEY");
    auto address = circular::EnvLoader::get_env("CIRCULAR_ADDRESS");

    if (!private_key) {
        std::cerr << "Error: CIRCULAR_PRIVATE_KEY not set in environment or .env file" << std::endl;
        return 1;
    }

    if (!address) {
        std::cerr << "Error: CIRCULAR_ADDRESS not set in environment or .env file" << std::endl;
        return 1;
    }

    // Initialize CepAccount
    circular::CepAccount account;
    if (!account.open(*address)) {
        std::cerr << "Failed to open account: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }

    // Set network (e.g., "testnet")
    bool network_success = account.set_network("testnet").get();
    if (!network_success) {
        std::cerr << "Failed to set network: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }
    std::cout << "Connected to NAG: " << account.nag_url << std::endl;

    // Update account nonce
    bool update_success = account.update_account().get();
    if (!update_success) {
        std::cerr << "Failed to update account: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }
    std::cout << "Account nonce updated. Current Nonce: " << account.nonce << std::endl;

    // Create and submit a certificate
    std::string certificate_data = "Hello, Circular Protocol from C++!";
    bool submit_success = account.submit_certificate(certificate_data, *private_key).get();

    if (!submit_success) {
        std::cerr << "Failed to submit certificate: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }
    std::cout << "Certificate submitted. Latest Transaction ID: " << account.latest_tx_id << std::endl;

    // Poll for transaction outcome
    std::cout << "Polling for transaction outcome..." << std::endl;
    std::string latest_tx_id = account.latest_tx_id;
    auto outcome = account.get_transaction_outcome(latest_tx_id, 60, 5).get(); // 60s timeout, 5s interval

    if (!outcome.has_value()) {
        std::cerr << "Failed to get transaction outcome: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }
    std::cout << "Transaction Outcome: " << outcome.value().dump(2) << std::endl;

    // Close the account
    account.close();
    std::cout << "Account closed." << std::endl;

    return 0;
}