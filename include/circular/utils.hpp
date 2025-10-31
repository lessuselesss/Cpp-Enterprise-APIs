#pragma once

/// @file utils.hpp
/// @brief Utility functions for Circular Protocol Enterprise APIs

#include <string>
#include <future>
#include <memory>
#include <variant>
#include <type_traits>

namespace circular {

/// @brief Task type for asynchronous operations
///
/// This provides a convenient alias for std::future used for async operations
/// throughout the Circular Protocol API.
template<typename T>
using Task = std::future<T>;

/// @brief Result type for operations that can fail
///
/// Provides a type-safe way to return either a success value (T) or an error (E).
/// Use the has_value() method to check for success before accessing the result.
template<typename T, typename E = std::string>
class Result {
public:
    /// @brief Creates a successful Result containing a value (copy)
    /// @param value The success value to store
    /// @return A Result in the Ok state containing the value
    static Result Ok(const T& value) {
        Result r;
        r.value_ = value;
        r.is_ok_ = true;
        return r;
    }

    /// @brief Creates a successful Result containing a value (move)
    /// @param value The success value to move into the Result
    /// @return A Result in the Ok state containing the value
    static Result Ok(T&& value) {
        Result r;
        r.value_ = std::move(value);
        r.is_ok_ = true;
        return r;
    }

    /// @brief Creates a failed Result containing an error (copy)
    /// @param error The error to store
    /// @return A Result in the Err state containing the error
    static Result Err(const E& error) {
        Result r;
        r.error_ = error;
        r.is_ok_ = false;
        return r;
    }

    /// @brief Creates a failed Result containing an error (move)
    /// @param error The error to move into the Result
    /// @return A Result in the Err state containing the error
    static Result Err(E&& error) {
        Result r;
        r.error_ = std::move(error);
        r.is_ok_ = false;
        return r;
    }

    /// @brief Checks if the Result contains a success value
    /// @return true if the Result is Ok, false if it is Err
    bool has_value() const { return is_ok_; }

    /// @brief Allows Result to be used in boolean contexts
    /// @return true if the Result is Ok, false if it is Err
    explicit operator bool() const { return has_value(); }

    /// @brief Accesses the success value (const)
    /// @return A const reference to the contained value
    /// @throws std::runtime_error if the Result is in the Err state
    const T& value() const {
        if (!is_ok_) throw std::runtime_error("accessing value on error result");
        return value_;
    }

    /// @brief Accesses the success value (mutable)
    /// @return A mutable reference to the contained value
    /// @throws std::runtime_error if the Result is in the Err state
    T& value() {
        if (!is_ok_) throw std::runtime_error("accessing value on error result");
        return value_;
    }

    /// @brief Accesses the error (const)
    /// @return A const reference to the contained error
    /// @throws std::runtime_error if the Result is in the Ok state
    const E& error() const {
        if (is_ok_) throw std::runtime_error("accessing error on ok result");
        return error_;
    }

    /// @brief Accesses the error (mutable)
    /// @return A mutable reference to the contained error
    /// @throws std::runtime_error if the Result is in the Ok state
    E& error() {
        if (is_ok_) throw std::runtime_error("accessing error on ok result");
        return error_;
    }

    /// @brief Dereference operator to access the value (const)
    /// @return A const reference to the contained value
    /// @throws std::runtime_error if the Result is in the Err state
    const T& operator*() const { return value(); }

    /// @brief Dereference operator to access the value (mutable)
    /// @return A mutable reference to the contained value
    /// @throws std::runtime_error if the Result is in the Err state
    T& operator*() { return value(); }

private:
    Result() = default;
    T value_;
    E error_;
    bool is_ok_ = false;
};

/// @brief Pads a number with a leading zero if it is a single digit
///
/// This utility function is typically used for formatting numbers (e.g., hours, minutes)
/// to ensure a consistent two-digit representation.
///
/// @param num An integer to be padded
/// @return A string representation of the number, padded with a leading zero if 0 <= num < 10
std::string pad_number(int num);

/// @brief Generates a formatted timestamp string in "YYYY:MM:DD-HH:MM:SS" format
///
/// This utility function uses the current UTC time to create a consistent
/// timestamp string, suitable for use in transaction data or logging.
///
/// @return A string containing the formatted timestamp
std::string get_formatted_timestamp();

/// @brief Cleans and normalizes a hexadecimal string
///
/// This utility function performs the following operations:
/// 1. Removes "0x" or "0X" prefixes
/// 2. Converts the string to lowercase
/// 3. Pads the string with a leading '0' if its length is odd
///
/// @param hex_str A string representing the hexadecimal string to fix
/// @return A string containing the cleaned and normalized hexadecimal string
std::string hex_fix(const std::string& hex_str);

/// @brief Converts a string to its hexadecimal representation
///
/// Each byte of the input string is converted into its two-digit uppercase
/// hexadecimal equivalent. This function matches the StringToHex function
/// from other language implementations (Go, Java, Node.js, PHP).
///
/// Note: Named str_to_hex (not string_to_hex) to avoid collision with OpenSSL's
/// macro that redefines string_to_hex to OPENSSL_hexstr2buf.
///
/// @param s A string to be converted to hexadecimal
/// @return A string containing the uppercase hexadecimal representation of the input string
std::string str_to_hex(const std::string& s);

/// @brief Alias for backward compatibility
/// @deprecated Use str_to_hex instead
inline std::string encode_hex(const std::string& s) { return str_to_hex(s); }

/// @brief Converts a hexadecimal string back to its original string representation
///
/// This utility function decodes a hexadecimal string into bytes and then
/// attempts to convert those bytes into a UTF-8 string. It handles optional
/// "0x" or "0X" prefixes.
///
/// Note: Named hex_to_str (not hex_to_string) to avoid collision with OpenSSL's
/// macro that redefines hex_to_string to OPENSSL_buf2hexstr.
///
/// @param hex_str A string representing the hexadecimal string to decode
/// @return A string containing the decoded string. Returns an empty string if the
///         input is empty or if decoding/conversion fails
std::string hex_to_str(const std::string& hex_str);

/// @brief Fetches the Network Access Gateway (NAG) URL for a given network identifier
///
/// This asynchronous function queries the network discovery service to retrieve
/// the appropriate NAG URL. It performs validation on the input and the received
/// response to ensure a valid URL is returned.
///
/// @param network A string representing the network identifier (e.g., "testnet", "mainnet")
/// @return A Task<Result<std::string, std::string>> which is:
///         - Ok(String) containing the NAG URL if the request is successful and the response is valid
///         - Err(String) containing an error message if the network identifier is empty,
///           the network request fails, the response status is not OK, or the NAG response
///           indicates an error or contains an invalid URL
Task<Result<std::string, std::string>> get_nag(const std::string& network);

} // namespace circular