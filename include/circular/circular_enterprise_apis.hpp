#pragma once

/// @file circular_enterprise_apis.hpp
/// @brief Main header for Circular Protocol Enterprise APIs C++ implementation
///
/// This header provides the complete public API for the Circular Protocol Enterprise APIs,
/// including account management, certificate creation and submission, and transaction tracking.

#include <circular/cep_account.hpp>
#include <circular/ccertificate.hpp>
#include <circular/utils.hpp>
#include <circular/env_loader.hpp>

/// @namespace circular
/// @brief Main namespace for Circular Protocol Enterprise APIs
namespace circular {

/// @brief Current version of the Circular Enterprise APIs C++ library
///
/// This constant tracks the library's version and can be included in
/// API requests to indicate the client's version.
inline constexpr const char* LIB_VERSION = "1.0.13";

/// @brief Default blockchain identifier used for transactions and account operations
///
/// This hexadecimal string represents a specific blockchain network that the API
/// interacts with by default.
inline constexpr const char* DEFAULT_CHAIN = "0x8a20baa40c45dc5055aeb26197c203e576ef389d9acb171bd62da11dc5ad72b2";

/// @brief Default Network Access Gateway (NAG) URL
///
/// This URL serves as the base for discovering network-specific endpoints.
inline constexpr const char* DEFAULT_NAG = "https://nag.circularlabs.io/NAG.php?cep=";

/// @brief Default network discovery URL
///
/// This URL is used to fetch the appropriate Network Access Gateway (NAG) for a
/// given network.
inline constexpr const char* DEFAULT_NETWORK_URL = "https://circularlabs.io/network/getNAG?network=";

} // namespace circular