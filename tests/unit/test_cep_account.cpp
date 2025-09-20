#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <circular/cep_account.hpp>
#include <circular/circular_enterprise_apis.hpp>

using namespace circular;

TEST_CASE("Testing CepAccount constructor") {
    SUBCASE("Default constructor values") {
        CepAccount account;

        CHECK(account.address == "");
        CHECK(account.public_key == "");
        CHECK(account.code_version == LIB_VERSION);
        CHECK(account.nag_url == DEFAULT_NAG);
        CHECK(account.network_node == "");
        CHECK(account.blockchain == DEFAULT_CHAIN);
        CHECK(account.latest_tx_id == "");
        CHECK(account.nonce == 0);
        CHECK(account.interval_sec == 2);
        CHECK(account.network_url == DEFAULT_NETWORK_URL);
        CHECK(!account.get_last_error().has_value());
    }
}

TEST_CASE("Testing CepAccount open/close operations") {
    CepAccount account;

    SUBCASE("Open account with valid address") {
        std::string test_address = "0x1234567890abcdef1234567890abcdef12345678";
        CHECK(account.open(test_address));
        CHECK(account.address == test_address);
        CHECK(!account.get_last_error().has_value());
    }

    SUBCASE("Open account with empty address") {
        CHECK_FALSE(account.open(""));
        CHECK(account.address == "");
        CHECK(account.get_last_error().has_value());
        CHECK(account.get_last_error().value() == "invalid address format");
    }

    SUBCASE("Close account") {
        // First open an account
        account.open("0x1234567890abcdef1234567890abcdef12345678");
        account.nonce = 5;
        account.latest_tx_id = "some_tx_id";

        // Close the account
        account.close();

        // Check all fields are reset
        CHECK(account.address == "");
        CHECK(account.public_key == "");
        CHECK(account.nag_url == "");
        CHECK(account.network_node == "");
        CHECK(account.blockchain == "");
        CHECK(account.latest_tx_id == "");
        CHECK(account.nonce == 0);
        CHECK(account.interval_sec == 0);
    }
}

TEST_CASE("Testing CepAccount blockchain operations") {
    CepAccount account;

    SUBCASE("Set blockchain") {
        std::string test_blockchain = "0xabcdef1234567890abcdef1234567890abcdef12";
        account.set_blockchain(test_blockchain);
        CHECK(account.blockchain == test_blockchain);
    }

    SUBCASE("Set empty blockchain") {
        account.set_blockchain("");
        CHECK(account.blockchain == "");
    }
}

TEST_CASE("Testing CepAccount error handling") {
    CepAccount account;

    SUBCASE("Initial state has no error") {
        CHECK(!account.get_last_error().has_value());
    }

    SUBCASE("Error after failed open") {
        account.open("");
        CHECK(account.get_last_error().has_value());
        CHECK(account.get_last_error().value() == "invalid address format");
    }

    SUBCASE("Multiple errors overwrite previous ones") {
        account.open("");  // First error
        std::string first_error = account.get_last_error().value();

        // This would cause another error if we had a way to trigger it
        // For now, just verify the error state is maintained
        CHECK(account.get_last_error().has_value());
    }
}

TEST_CASE("Testing CepAccount state consistency") {
    CepAccount account;

    SUBCASE("Account state after open") {
        std::string address = "0x1234567890abcdef1234567890abcdef12345678";
        account.open(address);

        // Verify state is consistent
        CHECK(account.address == address);
        CHECK(account.code_version == LIB_VERSION);
        CHECK(account.blockchain == DEFAULT_CHAIN);
        CHECK(account.nag_url == DEFAULT_NAG);
    }

    SUBCASE("Account state after close") {
        // Open first
        account.open("0x1234567890abcdef1234567890abcdef12345678");
        account.set_blockchain("custom_blockchain");

        // Close and verify critical fields are cleared
        account.close();

        CHECK(account.address == "");
        CHECK(account.blockchain == "");
        CHECK(account.nag_url == "");
        CHECK(account.latest_tx_id == "");
        CHECK(account.nonce == 0);
    }
}

TEST_CASE("Testing CepAccount field assignments") {
    CepAccount account;

    SUBCASE("Public key assignment") {
        std::string pub_key = "0x04abcdef...";
        account.public_key = pub_key;
        CHECK(account.public_key == pub_key);
    }

    SUBCASE("Latest transaction ID assignment") {
        std::string tx_id = "0xabc123def456";
        account.latest_tx_id = tx_id;
        CHECK(account.latest_tx_id == tx_id);
    }

    SUBCASE("Nonce assignment") {
        account.nonce = 42;
        CHECK(account.nonce == 42);
    }

    SUBCASE("Interval assignment") {
        account.interval_sec = 10;
        CHECK(account.interval_sec == 10);
    }
}

TEST_CASE("Testing CepAccount constants") {
    CepAccount account;

    SUBCASE("Default values match constants") {
        CHECK(account.code_version == LIB_VERSION);
        CHECK(account.nag_url == DEFAULT_NAG);
        CHECK(account.blockchain == DEFAULT_CHAIN);
        CHECK(account.network_url == DEFAULT_NETWORK_URL);
    }
}