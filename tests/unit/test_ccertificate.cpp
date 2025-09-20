#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <circular/ccertificate.hpp>
#include <circular/circular_enterprise_apis.hpp>
#include <nlohmann/json.hpp>

using namespace circular;

TEST_CASE("Testing CCertificate constructor") {
    SUBCASE("Default constructor") {
        CCertificate cert;

        // Check initial values
        CHECK(cert.get_data() == "");
        CHECK(cert.get_previous_tx_id() == "");
        CHECK(cert.get_previous_block() == "");

        // Check that JSON contains version
        std::string json_str = cert.get_json_certificate();
        auto json = nlohmann::json::parse(json_str);
        CHECK(json["version"] == LIB_VERSION);
    }
}

TEST_CASE("Testing CCertificate data operations") {
    CCertificate cert;

    SUBCASE("Set and get data") {
        std::string test_data = "Hello, World!";
        cert.set_data(test_data);
        CHECK(cert.get_data() == test_data);
    }

    SUBCASE("Empty data") {
        cert.set_data("");
        CHECK(cert.get_data() == "");
    }

    SUBCASE("Special characters in data") {
        std::string test_data = "Special chars: !@#$%^&*()";
        cert.set_data(test_data);
        CHECK(cert.get_data() == test_data);
    }

    SUBCASE("Unicode data") {
        std::string test_data = "Test with newlines\nand tabs\t";
        cert.set_data(test_data);
        CHECK(cert.get_data() == test_data);
    }
}

TEST_CASE("Testing CCertificate previous transaction operations") {
    CCertificate cert;

    SUBCASE("Set and get previous transaction ID") {
        std::string tx_id = "0x1234567890abcdef";
        cert.set_previous_tx_id(tx_id);
        CHECK(cert.get_previous_tx_id() == tx_id);
    }

    SUBCASE("Set and get previous block") {
        std::string block = "0xabcdef1234567890";
        cert.set_previous_block(block);
        CHECK(cert.get_previous_block() == block);
    }

    SUBCASE("Empty values") {
        cert.set_previous_tx_id("");
        cert.set_previous_block("");
        CHECK(cert.get_previous_tx_id() == "");
        CHECK(cert.get_previous_block() == "");
    }
}

TEST_CASE("Testing CCertificate JSON serialization") {
    CCertificate cert;

    SUBCASE("Basic JSON structure") {
        cert.set_data("test data");
        cert.set_previous_tx_id("prev_tx");
        cert.set_previous_block("prev_block");

        std::string json_str = cert.get_json_certificate();
        CHECK(!json_str.empty());

        auto json = nlohmann::json::parse(json_str);

        // Check that all required fields are present (matching Rust implementation)
        CHECK(json.contains("data"));
        CHECK(json.contains("version"));

        // Check field values match Rust implementation
        CHECK(json["version"] == LIB_VERSION);
    }

    SUBCASE("Data is stored as-is in JSON (matching Rust)") {
        std::string original_data = "Hello";
        cert.set_data(original_data);

        std::string json_str = cert.get_json_certificate();
        auto json = nlohmann::json::parse(json_str);

        // Data should be stored as-is, not hex encoded (matching Rust implementation)
        CHECK(json["data"] == "Hello");
    }

    SUBCASE("Empty certificate JSON") {
        std::string json_str = cert.get_json_certificate();
        auto json = nlohmann::json::parse(json_str);

        CHECK(json["data"] == "");
        CHECK(json["version"] == LIB_VERSION);
    }
}

TEST_CASE("Testing CCertificate size calculation") {
    CCertificate cert;

    SUBCASE("Empty certificate size") {
        size_t size = cert.get_certificate_size();
        std::string json_str = cert.get_json_certificate();
        CHECK(size == json_str.length());
    }

    SUBCASE("Certificate with data size") {
        cert.set_data("Some test data");
        cert.set_previous_tx_id("tx123");
        cert.set_previous_block("block456");

        size_t size = cert.get_certificate_size();
        std::string json_str = cert.get_json_certificate();
        CHECK(size == json_str.length());
        CHECK(size > 0);
    }

    SUBCASE("Size consistency") {
        cert.set_data("Test");
        size_t size1 = cert.get_certificate_size();

        cert.set_data("Test");  // Set same data again
        size_t size2 = cert.get_certificate_size();

        CHECK(size1 == size2);
    }
}

TEST_CASE("Testing CCertificate JSON field names match Rust") {
    CCertificate cert;
    cert.set_data("test");

    std::string json_str = cert.get_json_certificate();
    auto json = nlohmann::json::parse(json_str);

    SUBCASE("Field names are correct") {
        // These field names must match the Rust implementation exactly
        CHECK(json.contains("data"));
        CHECK(json.contains("version"));
    }

    SUBCASE("No unexpected fields") {
        // Should only have these 2 fields (matching Rust implementation)
        CHECK(json.size() == 2);
    }
}