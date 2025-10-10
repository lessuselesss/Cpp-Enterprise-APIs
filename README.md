# Circular Enterprise APIs - C++ Implementation

Official Circular Protocol Enterprise APIs for Data Certification - C++ Implementation

## Features

- Account management and blockchain interaction
- Certificate creation and submission
- Transaction tracking and verification
- Secure digital signatures using ECDSA (secp256k1)
- RFC 6979 compliant deterministic signatures

## Requirements

- C++20 compiler (GCC 11+, Clang 13+, MSVC 2022+)
- CMake 3.20 or higher

## Dependencies

Dependencies are automatically managed via CMake FetchContent:

- `nlohmann/json` for JSON serialization
- `cpp-httplib` for HTTP client operations
- `libsecp256k1` for secp256k1 elliptic curve operations
- `OpenSSL` for SHA-256 hashing

## Installation

```bash
git clone https://github.com/lessuselesss/Cpp-Enterprise-APIs.git
cd Cpp-Enterprise-APIs
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Or install to your system:

```bash
cmake --build build --target install
```

## Usage Example

See `examples/simple_certificate_submission.cpp` for a basic example of how to use the API to submit a certificate. You can build and run it with:

```bash
cmake --build build
./build/examples/simple_certificate_submission
```

A more detailed example can be found in `examples/` directory.

## API Documentation

### CepAccount Class

Main class for interacting with the Circular blockchain:

- `CepAccount()` - Constructor to create a new `CepAccount` instance.
- `bool open(const std::string& address)` - Initializes the account with a specified blockchain address.
- `void close()` - Clears all sensitive and operational data from the account.
- `std::future<std::string> set_network(const std::string& network)` - Configures the account to operate on a specific blockchain network.
- `void set_blockchain(const std::string& chain)` - Explicitly sets the blockchain identifier for the account.
- `std::future<bool> update_account()` - Fetches the latest nonce for the account from the NAG.
- `std::future<void> submit_certificate(const std::string& pdata, const std::string& privateKeyHex)` - Creates, signs, and submits a data certificate to the blockchain.
- `std::future<nlohmann::json> get_transaction(const std::string& blockID, const std::string& transactionID)` - Retrieves transaction details by block and transaction ID.
- `std::future<nlohmann::json> get_transaction_outcome(const std::string& txID, int timeoutSec, int intervalSec)` - Polls for the final status of a transaction.
- `std::string get_last_error()` - Retrieves the last error message.

### CCertificate Class

Class for managing certificates:

- `CCertificate()` - Constructor to create a new `CCertificate` instance.
- `void set_data(const std::string& data)` - Sets the primary data content of the certificate.
- `std::string get_data()` - Retrieves the primary data content from the certificate.
- `std::string get_json_certificate()` - Serializes the certificate object into a JSON string.
- `size_t get_certificate_size()` - Calculates the size of the JSON-serialized certificate in bytes.
- `void set_previous_tx_id(const std::string& txID)` - Sets the transaction ID of the preceding certificate.
- `void set_previous_block(const std::string& block)` - Sets the block identifier of the preceding certificate.
- `std::string get_previous_tx_id()` - Retrieves the transaction ID of the preceding certificate.
- `std::string get_previous_block()` - Retrieves the block identifier of the preceding certificate.

## Testing

To run the tests, you need to set up a `.env` file in the project root. You can copy the `.env.example` file to get started:

```bash
cp .env.example .env
```

Then, edit the `.env` file with your credentials:

```
CIRCULAR_PRIVATE_KEY="your_64_character_private_key_here"
CIRCULAR_ADDRESS="your_wallet_address_here"
```

The private key should be a 64-character (32-byte) hex string, and the address should be a valid Ethereum-style address (40 characters + 0x prefix).

### Running Tests

```bash
# Build with tests
cmake -B build -S . -DCIRCULAR_BUILD_TESTS=ON
cmake --build build

# Run all tests
cd build && ctest --output-on-failure
```

## License

MIT License - see LICENSE file for details

## Credits

CIRCULAR GLOBAL LEDGERS, INC. - USA

- Original JS Version: Gianluca De Novi, PhD
- Go Implementation: Danny De Novi