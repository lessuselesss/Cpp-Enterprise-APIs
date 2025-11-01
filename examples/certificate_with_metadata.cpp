#include <circular/circular_enterprise_apis.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ctime>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

std::string get_current_iso_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_point = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::gmtime(&time_point);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

int main() {
    // Load environment variables
    circular::EnvLoader::load_env_file();

    auto private_key = circular::EnvLoader::get_env("CIRCULAR_PRIVATE_KEY");
    auto address = circular::EnvLoader::get_env("CIRCULAR_ADDRESS");

    if (!private_key || !address) {
        std::cerr << "Error: Environment variables not set" << std::endl;
        return 1;
    }

    // Initialize account
    circular::CepAccount account;
    if (!account.open(*address)) {
        std::cerr << "Failed to open account" << std::endl;
        return 1;
    }

    // Connect to testnet
    std::string nag_url = account.set_network("testnet").get();
    if (nag_url.empty()) {
        std::cerr << "Failed to connect to testnet" << std::endl;
        return 1;
    }

    if (!account.update_account().get()) {
        std::cerr << "Failed to update account" << std::endl;
        return 1;
    }

    // Create certificate with rich metadata
    json certificate_data = {
        {"type", "document_certification"},
        {"document", {
            {"title", "Software License Agreement"},
            {"version", "2.1.0"},
            {"hash", "sha256:a1b2c3d4e5f6..."},
            {"size_bytes", 15420}
        }},
        {"metadata", {
            {"author", "Development Team"},
            {"department", "Legal & Compliance"},
            {"classification", "Public"},
            {"retention_years", 7}
        }},
        {"certification", {
            {"certifier_id", *address},
            {"timestamp", get_current_iso_timestamp()},
            {"compliance_standard", "ISO/IEC 27001"},
            {"audit_trail", "AT-2024-09-001"}
        }},
        {"verification", {
            {"checksum_algorithm", "SHA-256"},
            {"digital_signature", true},
            {"blockchain_network", "circular_testnet"}
        }}
    };

    std::string json_string = certificate_data.dump(2);
    std::cout << "Certificate data to be submitted:\n" << json_string << std::endl;

    // Submit the certificate
    std::cout << "\nSubmitting certificate with metadata..." << std::endl;
    account.submit_certificate(json_string, *private_key).get();

    if (auto error = account.get_last_error()) {
        std::cerr << "Failed to submit certificate: " << *error << std::endl;
        return 1;
    }

    std::cout << "Certificate submitted successfully!" << std::endl;
    std::cout << "Transaction ID: " << account.latest_tx_id << std::endl;

    // Get transaction outcome
    std::cout << "\nRetrieving transaction outcome..." << std::endl;
    auto outcome = account.get_transaction_outcome(account.latest_tx_id, 30, 2).get();

    if (outcome.has_value()) {
        std::cout << "Transaction outcome:\n" << outcome.value().dump(2) << std::endl;

        // Extract and display key information
        if (outcome.value().contains("Status")) {
            std::cout << "\n=== Certification Summary ===" << std::endl;
            std::cout << "Status: " << outcome.value()["Status"] << std::endl;
            std::cout << "Block ID: " << outcome.value().value("BlockID", "N/A") << std::endl;
            std::cout << "Processing Fee: " << outcome.value().value("ProcessingFee", 0.0) << std::endl;
            std::cout << "Timestamp: " << outcome.value().value("Timestamp", "N/A") << std::endl;
        }
    } else {
        std::cout << "Could not retrieve transaction outcome (timeout or error)" << std::endl;
    }

    account.close();
    return 0;
}