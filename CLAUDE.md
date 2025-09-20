# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Circular Protocol Enterprise APIs - C++ Implementation**

This is the official C++ implementation of the Circular Protocol Enterprise APIs for Data Certification. The API surface, developer experience, and ergonomics exactly match the reference Rust implementation to ensure complete compatibility and consistent usage patterns across language implementations.

## Architecture

### Core Components

- **`circular::CepAccount`**: Main account management class for blockchain interactions
- **`circular::CCertificate`**: Certificate creation and data management
- **`circular::utils`**: Utility functions for hex conversion, timestamps, and network operations
- **Network Layer**: Async HTTP client built on cpp-httplib for NAG communication
- **Crypto Layer**: ECDSA signing using libsecp256k1, SHA256 hashing via OpenSSL

### Key Design Decisions

1. **API Compatibility**: Method names, signatures, error messages, and JSON formats exactly match the Rust reference implementation
2. **Async Operations**: Uses `std::async` with `std::future<T>` (aliased as `Task<T>`) for async operations
3. **Error Handling**: `std::expected<T, E>` for Result types, `std::optional<T>` for nullable returns
4. **Memory Safety**: RAII patterns, smart pointers, no raw pointers in public API
5. **Thread Safety**: Mutex-protected global state matching Rust's `Mutex<T>` patterns

## Build System

### Commands

```bash
# Configure and build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Build with all options
cmake -B build -S . -DCIRCULAR_BUILD_TESTS=ON -DCIRCULAR_BUILD_EXAMPLES=ON
cmake --build build

# Run tests
cd build && ctest --output-on-failure

# Run specific test categories
cmake --build build --target test_unit
cmake --build build --target test_integration_only
cmake --build build --target test_e2e_only

# Run examples
./build/examples/simple_certificate_submission
./build/examples/certify_poem
```

### Dependencies

- **nlohmann/json**: JSON serialization (header-only)
- **cpp-httplib**: HTTP client (header-only)
- **libsecp256k1**: ECDSA cryptography
- **OpenSSL**: SHA256 hashing
- **doctest**: Testing framework (header-only)

All dependencies are automatically fetched via CMake FetchContent if not found system-wide.

## Development Workflow

### Environment Setup

1. **Set environment variables** (copy `.env.example` to `.env`):
   ```bash
   CIRCULAR_PRIVATE_KEY="your_64_character_private_key_here"
   CIRCULAR_ADDRESS="your_account_address_here"
   ```

2. **Compiler Requirements**:
   - C++20 support required (for coroutines, `std::expected`)
   - GCC 11+, Clang 13+, or MSVC 2022+

### Testing Strategy

- **Unit Tests**: Test individual classes and functions in isolation
- **Integration Tests**: Test against live testnet (requires env vars)
- **E2E Tests**: Full workflow testing with real certificate submission

### Code Style

- Modern C++20 patterns
- RAII for all resource management
- `const` correctness throughout
- Descriptive variable and function names
- Comprehensive error handling

## API Usage Patterns

### Basic Certificate Submission

```cpp
#include <circular/circular_enterprise_apis.hpp>

circular::CepAccount account;
account.open("0x...");
account.set_network("testnet").get();
account.update_account().get();
account.submit_certificate("data", "private_key").get();
```

### Error Handling

```cpp
if (auto error = account.get_last_error()) {
    std::cerr << "Error: " << *error << std::endl;
}
```

### Async Operations

All network operations return `Task<T>` (std::future) that must be awaited:

```cpp
auto result = account.set_network("testnet").get();  // Blocks until complete
```

## Common Issues

1. **Build Failures**: Ensure C++20 compiler and CMake 3.20+
2. **Missing Dependencies**: Dependencies auto-fetch, but may need system packages for OpenSSL
3. **Test Failures**: Integration/E2E tests require valid environment variables
4. **Network Errors**: Check firewall settings for HTTPS outbound connections

## File Structure

```
├── include/circular/          # Public headers
├── src/                      # Implementation files
├── examples/                 # Usage examples
├── tests/unit/              # Unit tests
├── tests/integration/       # Integration tests
├── tests/e2e/              # End-to-end tests
├── cmake/                  # CMake modules
└── docs/                   # Documentation
```

## Debugging

- **Debug builds**: Use `-DCMAKE_BUILD_TYPE=Debug`
- **Sanitizers**: Enable with `-DCIRCULAR_ENABLE_ASAN=ON`
- **Logging**: Check `rust_sign_debug.log` for signature debugging
- **Network**: Use Wireshark or similar for HTTPS traffic analysis

## Compatibility Notes

This implementation is designed to be a drop-in replacement for the Rust implementation:

- Same JSON wire protocol
- Identical API method signatures
- Same error codes and messages
- Compatible certificate formats
- Matching network communication patterns

Any code using the Rust implementation should be easily portable to this C++ version with minimal syntax changes.