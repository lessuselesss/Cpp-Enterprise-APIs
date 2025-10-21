#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <circular/utils.hpp>

using namespace circular;

TEST_CASE("Testing pad_number function") {
    SUBCASE("Single digit numbers") {
        CHECK(pad_number(0) == "00");
        CHECK(pad_number(1) == "01");
        CHECK(pad_number(5) == "05");
        CHECK(pad_number(9) == "09");
    }

    SUBCASE("Double digit and larger numbers") {
        CHECK(pad_number(10) == "10");
        CHECK(pad_number(15) == "15");
        CHECK(pad_number(99) == "99");
        CHECK(pad_number(100) == "100");
    }

    SUBCASE("Negative numbers") {
        CHECK(pad_number(-1) == "-1");
        CHECK(pad_number(-10) == "-10");
    }
}

TEST_CASE("Testing get_formatted_timestamp function") {
    SUBCASE("Timestamp format") {
        std::string timestamp = get_formatted_timestamp();

        // Should be in format YYYY:MM:DD-HH:MM:SS (19 characters)
        CHECK(timestamp.length() == 19);

        // Check format structure
        CHECK(timestamp[4] == ':');   // After year
        CHECK(timestamp[7] == ':');   // After month
        CHECK(timestamp[10] == '-');  // After day
        CHECK(timestamp[13] == ':');  // After hour
        CHECK(timestamp[16] == ':');  // After minute
    }
}

TEST_CASE("Testing hex_fix function") {
    SUBCASE("Empty string") {
        CHECK(hex_fix("") == "");
    }

    SUBCASE("Remove 0x prefix") {
        CHECK(hex_fix("0x1234") == "1234");
        CHECK(hex_fix("0X1234") == "1234");
        CHECK(hex_fix("0xABCD") == "abcd");
    }

    SUBCASE("Convert to lowercase") {
        CHECK(hex_fix("ABCD") == "abcd");
        CHECK(hex_fix("1234ABCD") == "1234abcd");
    }

    SUBCASE("Pad odd length strings") {
        CHECK(hex_fix("123") == "0123");
        CHECK(hex_fix("a") == "0a");
        CHECK(hex_fix("0x123") == "0123");
    }

    SUBCASE("Even length strings remain unchanged") {
        CHECK(hex_fix("1234") == "1234");
        CHECK(hex_fix("abcd") == "abcd");
        CHECK(hex_fix("0x1234") == "1234");
    }
}

TEST_CASE("Testing str_to_hex function") {
    SUBCASE("Basic string conversion") {
        CHECK(str_to_hex("") == "");
        CHECK(str_to_hex("A") == "41");
        CHECK(str_to_hex("Hello") == "48656C6C6F");
    }

    SUBCASE("Special characters") {
        CHECK(str_to_hex(" ") == "20");  // Space
        CHECK(str_to_hex("\n") == "0A"); // Newline
        CHECK(str_to_hex("!") == "21");  // Exclamation
    }

    SUBCASE("Unicode characters") {
        // Test with basic ASCII range
        CHECK(str_to_hex("123") == "313233");
        CHECK(str_to_hex("abc") == "616263");
    }
}

TEST_CASE("Testing hex_to_str function") {
    SUBCASE("Empty string") {
        CHECK(hex_to_str("") == "");
    }

    SUBCASE("Remove 0x prefix") {
        CHECK(hex_to_str("0x48656C6C6F") == "Hello");
        CHECK(hex_to_str("0X48656C6C6F") == "Hello");
    }

    SUBCASE("Basic hex to string conversion") {
        CHECK(hex_to_str("41") == "A");
        CHECK(hex_to_str("48656C6C6F") == "Hello");
        CHECK(hex_to_str("313233") == "123");
    }

    SUBCASE("Invalid hex strings") {
        CHECK(hex_to_str("G1") == "");      // Invalid hex character
        CHECK(hex_to_str("123") == "");     // Odd length
        CHECK(hex_to_str("ZZ") == "");      // Invalid characters
    }

    SUBCASE("Case insensitive") {
        CHECK(hex_to_str("48656c6c6f") == "Hello"); // lowercase
        CHECK(hex_to_str("48656C6C6F") == "Hello"); // uppercase
    }
}

TEST_CASE("Testing str_to_hex and hex_to_str roundtrip") {
    SUBCASE("Roundtrip conversion") {
        std::string original = "Hello, World!";
        std::string hex = str_to_hex(original);
        std::string decoded = hex_to_str(hex);
        CHECK(decoded == original);
    }

    SUBCASE("Empty string roundtrip") {
        std::string original = "";
        std::string hex = str_to_hex(original);
        std::string decoded = hex_to_str(hex);
        CHECK(decoded == original);
    }

    SUBCASE("Special characters roundtrip") {
        std::string original = "Line 1\nLine 2\tTabbed";
        std::string hex = str_to_hex(original);
        std::string decoded = hex_to_str(hex);
        CHECK(decoded == original);
    }
}