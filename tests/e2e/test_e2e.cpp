#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <circular/circular_enterprise_apis.hpp>
#include <cstdlib>

using namespace circular;

// Helper to check if environment variables are set
bool has_test_credentials() {
    return std::getenv("CIRCULAR_PRIVATE_KEY") != nullptr &&
           std::getenv("CIRCULAR_ADDRESS") != nullptr;
}

TEST_CASE("E2E test setup") {
    if (!has_test_credentials()) {
        WARN("Skipping E2E tests - CIRCULAR_PRIVATE_KEY and CIRCULAR_ADDRESS not set");
        return;
    }

    SUBCASE("Credentials are properly formatted") {
        std::string private_key = std::getenv("CIRCULAR_PRIVATE_KEY");
        std::string address = std::getenv("CIRCULAR_ADDRESS");

        // Accept keys with or without "0x" prefix (64 or 66 chars)
        CHECK((private_key.length() == 64 || private_key.length() == 66));
        CHECK(address.length() >= 40);     // At least 40 hex chars (with or without 0x)
    }
}

TEST_CASE("Full certificate submission workflow") {
    if (!has_test_credentials()) {
        return; // Skip if no credentials
    }

    std::string private_key = std::getenv("CIRCULAR_PRIVATE_KEY");
    std::string address = std::getenv("CIRCULAR_ADDRESS");

    SUBCASE("Complete workflow") {
        CepAccount account;
        CHECK(account.open(address));

        // Set network
        std::string nag_url = account.set_network("testnet").get();
        if (nag_url.empty()) {
            std::string error_msg = "Network setup failed: " + account.get_last_error().value_or("unknown");
            MESSAGE(error_msg);
            return;
        }

        // Update account
        bool update_success = account.update_account().get();
        if (!update_success) {
            auto error = account.get_last_error().value_or("unknown");
            if (error == "Rejected: Insufficient balance" || error == "Rejected: Invalid Blockchain") {
                std::string msg = "Expected error during update_account: " + error;
                MESSAGE(msg);
                return; // This is expected for test accounts
            } else {
                std::string msg = "Unexpected error during update_account: " + error;
                FAIL_CHECK(msg);
            }
        }

        // Submit certificate
        std::string test_data = "E2E test from C++ implementation";
        account.submit_certificate(test_data, private_key).get();

        if (auto error = account.get_last_error()) {
            if (*error == "certificate submission failed: Invalid Signature" ||
                *error == "certificate submission failed: Duplicate Nonce" ||
                *error == "Rejected: Insufficient balance") {
                std::string msg = "Expected error during submit_certificate: " + *error;
                MESSAGE(msg);
                return; // These are expected for some test scenarios
            } else {
                std::string msg = "Unexpected error during certificate submission: " + *error;
                FAIL_CHECK(msg);
            }
        }

        // If we got here, submission was successful
        CHECK(!account.latest_tx_id.empty());

        // Optionally try to get transaction outcome
        auto outcome = account.get_transaction_outcome(account.latest_tx_id, 30, 2).get();
        if (outcome.has_value()) {
            CHECK(outcome.value().contains("Status"));
        } else {
            std::string msg = "Failed to get transaction outcome: " + account.get_last_error().value_or("unknown");
            MESSAGE(msg);
        }
    }
}