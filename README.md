# Circular Enterprise APIs - C++ Implementation

Official Circular Protocol Enterprise APIs for Data Certification - C++ Implementation

## Features

- Account management and blockchain interaction (`CepAccount`)
- Certificate creation and submission (`CCertificate`)
- Transaction tracking and verification
- Secure digital signatures using ECDSA (secp256k1)
- Asynchronous network operations with modern C++20
- **100% API compatibility** with Rust reference implementation

## Requirements

- **C++20 compiler** (GCC 11+, Clang 13+, MSVC 2022+)
- **CMake 3.20+**
- **OpenSSL** (for SHA256 hashing)

## Dependencies

Dependencies are automatically managed via CMake FetchContent:

- `nlohmann/json`: JSON serialization and deserialization
- `cpp-httplib`: Asynchronous HTTP client
- `libsecp256k1`: ECDSA cryptographic operations
- `OpenSSL`: SHA-2 hashing algorithm
- `doctest`: Testing framework

## Quick Start

### 1. Clone and Build

```bash
git clone <repository-url>
cd Cpp-Enterprise-APIs
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### 2. Set Environment Variables

```bash
cp .env.example .env
# Edit .env with your credentials:
# CIRCULAR_PRIVATE_KEY="your_64_character_private_key_here"
# CIRCULAR_ADDRESS="your_account_address_here"
```

### 3. Run Example

```bash
export CIRCULAR_PRIVATE_KEY="your_private_key"
export CIRCULAR_ADDRESS="your_address"
./build/examples/simple_certificate_submission
```

## Usage Example

```cpp
#include <circular/circular_enterprise_apis.hpp>

int main() {
    // Initialize account
    circular::CepAccount account;
    account.open("0x...");

    // Configure network
    account.set_network("testnet").get();
    account.update_account().get();

    // Submit certificate
    account.submit_certificate("Hello, Circular!", "private_key").get();

    // Poll for result
    auto outcome = account.get_transaction_outcome(
        account.latest_tx_id, 60, 5).get();

    return 0;
}
```

## API Documentation

### `CepAccount` Class

Main class for blockchain interactions:

- `open(address)`: Initialize account with blockchain address
- `close()`: Clear all account data
- `set_network(network)`: Configure for specific network ("testnet", "mainnet")
- `set_blockchain(chain)`: Set blockchain identifier
- `update_account()`: Fetch latest nonce from network
- `submit_certificate(data, private_key)`: Submit data certificate
- `get_transaction(block_id, tx_id)`: Retrieve transaction details
- `get_transaction_outcome(tx_id, timeout, interval)`: Poll for transaction status
- `get_last_error()`: Get last error message

### `CCertificate` Class

Certificate management:

- `new()`: Create new certificate
- `set_data(data)`: Set certificate data (auto-converts to hex)
- `get_data()`: Get certificate data (auto-converts from hex)
- `get_json_certificate()`: Serialize to JSON
- `get_certificate_size()`: Get JSON size in bytes
- `set_previous_tx_id(tx_id)`: Set previous transaction ID
- `set_previous_block(block)`: Set previous block ID

## Testing

```bash
# Run all tests
cd build && ctest --output-on-failure

# Run specific test suites
cmake --build build --target test_unit            # Unit tests only
cmake --build build --target test_integration_only # Integration tests
cmake --build build --target test_e2e_only        # End-to-end tests
```

**Note**: Integration and E2E tests require environment variables and network access.

## Building Options

```bash
# Development build with tests and examples
cmake -B build -S . \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCIRCULAR_BUILD_TESTS=ON \
  -DCIRCULAR_BUILD_EXAMPLES=ON \
  -DCIRCULAR_ENABLE_ASAN=ON

# Production build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --target circular_enterprise_apis
```

## Installation

```bash
cmake --build build --target install
```

Or use in your CMake project:

```cmake
find_package(CircularEnterpriseAPIs REQUIRED)
target_link_libraries(your_target Circular::circular_enterprise_apis)
```

## Compatibility

This C++ implementation provides **100% API compatibility** with the Rust reference implementation:

- ✅ Identical method names and signatures
- ✅ Same JSON wire protocol
- ✅ Same error codes and messages
- ✅ Compatible certificate formats
- ✅ Matching async patterns (translated to C++20 futures)

Code using the Rust implementation can be easily ported with minimal syntax changes.

## License

MIT License - see LICENSE file for details

## Credits

**CIRCULAR GLOBAL LEDGERS, INC.** - USA

- Original Design: Gianluca De Novi, PhD
- Rust Reference: Ashley Barr
- C++ Implementation: Generated with Claude Code