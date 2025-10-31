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

TEST_CASE("Integration test setup") {
    if (!has_test_credentials()) {
        WARN("Skipping integration tests - CIRCULAR_PRIVATE_KEY and CIRCULAR_ADDRESS not set");
        return;
    }

    SUBCASE("Environment variables are valid") {
        std::string private_key = std::getenv("CIRCULAR_PRIVATE_KEY");
        std::string address = std::getenv("CIRCULAR_ADDRESS");

        CHECK(!private_key.empty());
        CHECK(!address.empty());
        // Accept keys with or without "0x" prefix (64 or 66 chars)
        CHECK((private_key.length() == 64 || private_key.length() == 66));
    }
}

TEST_CASE("Basic account operations") {
    if (!has_test_credentials()) {
        return; // Skip if no credentials
    }

    std::string address = std::getenv("CIRCULAR_ADDRESS");

    SUBCASE("Account can be opened and closed") {
        CepAccount account;
        CHECK(account.open(address));
        CHECK(account.address == address);

        account.close();
        CHECK(account.address == "");
    }
}

TEST_CASE("Network operations") {
    if (!has_test_credentials()) {
        return; // Skip if no credentials
    }

    std::string address = std::getenv("CIRCULAR_ADDRESS");

    SUBCASE("Set network testnet") {
        CepAccount account;
        account.open(address);

        bool network_success = account.set_network("testnet").get();
        if (network_success) {
            CHECK(!account.nag_url.empty());
            CHECK(account.network_node == "testnet");
        } else {
            // Network call may fail in some environments
            std::string error_msg = "Network call failed: " + account.get_last_error().value_or("unknown");
            MESSAGE(error_msg);
        }
    }
}