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

## API Reference

### CepAccount Class
Primary interface for blockchain interaction:

- **CepAccount()** - Creates a new account instance
- **open(account_address)** - Initializes account with specified blockchain address
- **close()** - Clears sensitive operational data
- **set_network(network)** - Configures network by fetching NAG URL (async, requires `.get()`)
- **set_blockchain(blockchain_address)** - Explicitly sets blockchain identifier
- **set_network_node(node)** - Sets network node identifier
- **set_polling_interval(interval_sec)** - Sets polling interval for transaction checks
- **update_account()** - Retrieves latest account nonce from NAG (async, requires `.get()`)
- **submit_certificate(pdata, private_key_hex)** - Creates, signs, and submits data certificate (async, requires `.get()`)
- **get_transaction(block_id, transaction_id)** - Retrieves transaction details (async, requires `.get()`)
- **get_transaction_outcome(tx_id, timeout_sec, poll_interval_sec)** - Polls transaction status (async, requires `.get()`)
- **get_last_error()** - Returns most recent error message

### CCertificate Class
Manages certificate data and metadata:

- **CCertificate()** - Instantiates certificate object
- **set_data(data)** / **get_data()** - Manages primary content
- **get_json_certificate()** - Serializes to JSON format
- **get_certificate_size()** - Calculates JSON byte size
- **set_previous_tx_id(tx_id)** / **get_previous_tx_id()** - Manages preceding transaction reference
- **set_previous_block(block)** / **get_previous_block()** - Manages preceding block identifier

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