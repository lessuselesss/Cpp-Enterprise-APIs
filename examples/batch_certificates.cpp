#include <circular/circular_enterprise_apis.hpp>
#include <iostream>
#include <vector>
#include <string>

int main() {
    // Load environment variables from .env file
    circular::EnvLoader::load_env_file();

    auto private_key = circular::EnvLoader::get_env("CIRCULAR_PRIVATE_KEY");
    auto address = circular::EnvLoader::get_env("CIRCULAR_ADDRESS");

    if (!private_key || !address) {
        std::cerr << "Error: CIRCULAR_PRIVATE_KEY and CIRCULAR_ADDRESS must be set" << std::endl;
        return 1;
    }

    // Initialize account
    circular::CepAccount account;
    if (!account.open(*address)) {
        std::cerr << "Failed to open account: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }

    // Connect to testnet
    if (!account.set_network("testnet").get()) {
        std::cerr << "Failed to set network: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }
    std::cout << "Connected to: " << account.nag_url << std::endl;

    // Update account nonce
    if (!account.update_account().get()) {
        std::cerr << "Failed to update account: " << account.get_last_error().value_or("unknown error") << std::endl;
        return 1;
    }
    std::cout << "Initial nonce: " << account.nonce << std::endl;

    // Batch of data to certify
    std::vector<std::string> data_batch = {
        "Document #1: Contract Agreement",
        "Document #2: Financial Report Q3",
        "Document #3: Technical Specification v2.1",
        "Document #4: Audit Results Summary",
        "Document #5: Compliance Certificate"
    };

    std::vector<std::string> transaction_ids;

    // Submit certificates in batch
    for (size_t i = 0; i < data_batch.size(); ++i) {
        std::cout << "\nSubmitting certificate " << (i + 1) << "/" << data_batch.size() << std::endl;

        bool success = account.submit_certificate(data_batch[i], *private_key).get();

        if (!success) {
            std::cerr << "Failed to submit certificate " << (i + 1) << ": "
                      << account.get_last_error().value_or("unknown error") << std::endl;
            continue;
        }

        transaction_ids.push_back(account.latest_tx_id);
        std::cout << "Certificate " << (i + 1) << " submitted. TX ID: " << account.latest_tx_id << std::endl;

        // Brief delay between submissions
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\n=== Batch Submission Summary ===" << std::endl;
    std::cout << "Total certificates submitted: " << transaction_ids.size() << "/" << data_batch.size() << std::endl;

    for (size_t i = 0; i < transaction_ids.size(); ++i) {
        std::cout << "TX " << (i + 1) << ": " << transaction_ids[i] << std::endl;
    }

    account.close();
    return 0;
}