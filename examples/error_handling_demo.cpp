#include <circular/circular_enterprise_apis.hpp>
#include <iostream>

int main() {
    std::cout << "=== Circular Protocol C++ API - Error Handling Demo ===" << std::endl;

    // Demo 1: Missing environment variables
    std::cout << "\n1. Testing missing environment variables..." << std::endl;
    {
        circular::CepAccount account;

        // Try to open with invalid address
        if (!account.open("invalid_address")) {
            std::cout << "✓ Correctly rejected invalid address: "
                      << account.get_last_error().value_or("no error details") << std::endl;
        }
    }

    // Demo 2: Network connection errors
    std::cout << "\n2. Testing network connection..." << std::endl;
    {
        circular::CepAccount account;
        if (account.open("bb9dbe8b94ae940016e89837574e84e2651f7f10da7809fff0728cc419514370")) {

            // Try invalid network
            std::string nag_url = account.set_network("invalid_network").get();
            if (nag_url.empty()) {
                std::cout << "✓ Correctly handled invalid network: "
                          << account.get_last_error().value_or("no error details") << std::endl;
            }
        }
    }

    // Demo 3: Authentication errors
    std::cout << "\n3. Testing authentication errors..." << std::endl;
    {
        circular::CepAccount account;
        if (account.open("bb9dbe8b94ae940016e89837574e84e2651f7f10da7809fff0728cc419514370")) {

            // Try to submit with invalid private key
            account.submit_certificate("test data", "invalid_private_key").get();
            if (auto error = account.get_last_error()) {
                std::cout << "✓ Correctly rejected invalid private key: " << *error << std::endl;
            }
        }
    }

    // Demo 4: Successful operation with proper credentials
    std::cout << "\n4. Testing with proper credentials (if available)..." << std::endl;
    {
        circular::EnvLoader::load_env_file();
        auto private_key = circular::EnvLoader::get_env("CIRCULAR_PRIVATE_KEY");
        auto address = circular::EnvLoader::get_env("CIRCULAR_ADDRESS");

        if (private_key && address) {
            circular::CepAccount account;
            if (account.open(*address)) {
                std::cout << "✓ Account opened successfully" << std::endl;

                std::string nag_url = account.set_network("testnet").get();
                if (!nag_url.empty()) {
                    std::cout << "✓ Connected to testnet: " << nag_url << std::endl;

                    if (account.update_account().get()) {
                        std::cout << "✓ Account updated, nonce: " << account.nonce << std::endl;
                    } else {
                        std::cout << "⚠ Account update failed: "
                                  << account.get_last_error().value_or("unknown") << std::endl;
                    }
                } else {
                    std::cout << "✗ Network connection failed: "
                              << account.get_last_error().value_or("unknown") << std::endl;
                }

                account.close();
                std::cout << "✓ Account closed properly" << std::endl;
            }
        } else {
            std::cout << "⚠ No credentials in .env file - skipping live test" << std::endl;
        }
    }

    // Demo 5: Certificate data validation and hex encoding
    std::cout << "\n5. Testing certificate data validation and hex encoding..." << std::endl;
    {
        circular::CCertificate cert;

        // Test various data types
        // Note: Data is hex-encoded internally but get_data() returns the original string
        cert.set_data("Simple string data");
        std::cout << "✓ String data (decoded): " << cert.get_data() << std::endl;

        cert.set_data("Unicode test: 🔒🌐🚀");
        std::cout << "✓ Unicode data (decoded): " << cert.get_data() << std::endl;

        cert.set_data("");
        std::cout << "✓ Empty data: '" << cert.get_data() << "'" << std::endl;

        // The JSON output contains hex-encoded data matching Rust implementation
        cert.set_data("Hello");
        std::string json_output = cert.get_json_certificate();
        std::cout << "✓ JSON output (data is hex-encoded): " << json_output << std::endl;
        std::cout << "✓ Certificate size: " << cert.get_certificate_size() << " bytes" << std::endl;
    }

    std::cout << "\n=== Error Handling Demo Complete ===" << std::endl;
    return 0;
}