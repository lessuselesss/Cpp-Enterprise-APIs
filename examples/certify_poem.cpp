#include <circular/circular_enterprise_apis.hpp>
#include <iostream>
#include <string>
#include <cstdlib>

int main() {
    // Load environment variables
    const char* private_key_env = std::getenv("CIRCULAR_PRIVATE_KEY");
    const char* address_env = std::getenv("CIRCULAR_ADDRESS");

    if (!private_key_env) {
        std::cerr << "Error: CIRCULAR_PRIVATE_KEY not set in environment" << std::endl;
        return 1;
    }

    if (!address_env) {
        std::cerr << "Error: CIRCULAR_ADDRESS not set in environment" << std::endl;
        return 1;
    }

    std::string private_key = private_key_env;
    std::string address = address_env;

    // Define the data to be certified (famous poem excerpt)
    std::string poem_content = R"(
Two roads diverged in a yellow wood,
And sorry I could not travel both
And be one traveler, long I stood
And looked down one as far as I could
To where it bent in the undergrowth;
    )";

    // Create a CCertificate instance and set the poem as its data
    circular::CCertificate certificate;
    certificate.set_data(poem_content);

    std::cout << "Certificate JSON (partial): " << certificate.get_json_certificate() << std::endl;
    std::cout << "Certificate size: " << certificate.get_certificate_size() << " bytes" << std::endl;

    // Create a CepAccount instance
    circular::CepAccount account;

    // Open the CepAccount with your blockchain address
    if (!account.open(address)) {
        std::cerr << "Failed to open account: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }
    std::cout << "Account opened successfully for address: " << account.address << std::endl;

    // Set the network (e.g., "testnet")
    std::string nag_url = account.set_network("testnet").get();
    if (nag_url.empty()) {
        std::cerr << "Failed to set network: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }
    std::cout << "Network set. NAG URL: " << nag_url << std::endl;

    // Update the account to fetch the latest nonce
    bool update_success = account.update_account().get();
    if (!update_success) {
        std::cerr << "Failed to update account (get nonce): " << account.get_last_error().value_or("unknown error") << std::endl;
        std::cerr << "Note: 'Rejected: Insufficient balance' is expected for integration tests against live network." << std::endl;
        return 1;
    }
    std::cout << "Account updated. Current nonce: " << account.nonce << std::endl;

    // Submit the certificate
    std::cout << "Submitting certificate..." << std::endl;
    account.submit_certificate(certificate.get_json_certificate(), private_key).get();

    if (auto err = account.get_last_error()) {
        std::cerr << "Certificate submission failed: " << *err << std::endl;
    } else {
        std::cout << "Certificate submitted successfully!" << std::endl;
        std::cout << "Latest Transaction ID: " << account.latest_tx_id << std::endl;

        // Optional: Poll for transaction outcome
        std::cout << "Polling for transaction outcome (this may take a moment)..." << std::endl;
        auto outcome = account.get_transaction_outcome(account.latest_tx_id, 60, 5).get();
        if (outcome.has_value()) {
            std::cout << "Transaction Outcome: " << outcome.value().dump(2) << std::endl;
        } else {
            std::cerr << "Failed to get transaction outcome: " << account.get_last_error().value_or("unknown error") << std::endl;
        }
    }

    return 0;
}