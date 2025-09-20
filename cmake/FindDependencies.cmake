# Find or fetch dependencies for Circular Enterprise APIs

include(FetchContent)

# Set FetchContent properties
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# Function to find or fetch a dependency
function(find_or_fetch_dependency name url tag target)
    find_package(${name} QUIET)
    if(NOT ${name}_FOUND)
        message(STATUS "Fetching ${name} from ${url}")
        FetchContent_Declare(
            ${name}
            GIT_REPOSITORY ${url}
            GIT_TAG ${tag}
            GIT_SHALLOW ON
        )
        FetchContent_MakeAvailable(${name})
    else()
        message(STATUS "Found ${name} via find_package")
    endif()
endfunction()

# nlohmann/json - JSON library
find_or_fetch_dependency(
    nlohmann_json
    "https://github.com/nlohmann/json.git"
    "v3.11.3"
    nlohmann_json::nlohmann_json
)

# cpp-httplib - HTTP client library
find_package(httplib QUIET)
if(NOT httplib_FOUND)
    message(STATUS "Fetching cpp-httplib")
    FetchContent_Declare(
        httplib
        GIT_REPOSITORY "https://github.com/yhirose/cpp-httplib.git"
        GIT_TAG "v0.14.3"
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(httplib)
else()
    message(STATUS "Found httplib via find_package")
endif()

# libsecp256k1 - Cryptographic library for ECDSA
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(SECP256K1 QUIET libsecp256k1)
endif()

if(NOT SECP256K1_FOUND)
    message(STATUS "Fetching libsecp256k1")
    FetchContent_Declare(
        secp256k1
        GIT_REPOSITORY "https://github.com/bitcoin-core/secp256k1.git"
        GIT_TAG "v0.4.1"
        GIT_SHALLOW ON
    )

    # Configure secp256k1 options before making available
    set(SECP256K1_BUILD_BENCHMARK OFF CACHE BOOL "")
    set(SECP256K1_BUILD_TESTS OFF CACHE BOOL "")
    set(SECP256K1_BUILD_EXHAUSTIVE_TESTS OFF CACHE BOOL "")
    set(SECP256K1_BUILD_CTIME_TESTS OFF CACHE BOOL "")
    set(SECP256K1_BUILD_EXAMPLES OFF CACHE BOOL "")
    set(SECP256K1_ENABLE_MODULE_RECOVERY ON CACHE BOOL "")
    set(SECP256K1_ENABLE_MODULE_ECDH OFF CACHE BOOL "")
    set(SECP256K1_ENABLE_MODULE_SCHNORRSIG OFF CACHE BOOL "")

    # Disable LTO for secp256k1 to avoid C/C++ LTO linking conflicts
    # We need to handle this at the target level since global flags are applied via add_compile_options
    set(_CIRCULAR_DISABLE_LTO_FOR_SECP256K1 ON)

    FetchContent_MakeAvailable(secp256k1)

    # Ensure secp256k1 target doesn't have LTO enabled and remove LTO flags
    if(TARGET secp256k1)
        set_target_properties(secp256k1 PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION FALSE
        )

        # Add explicit no-LTO flag to override global LTO settings
        target_compile_options(secp256k1 PRIVATE -fno-lto)
    endif()

    # Also disable LTO for secp256k1_precomputed which contains the precomputed tables
    if(TARGET secp256k1_precomputed)
        set_target_properties(secp256k1_precomputed PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION FALSE
        )
        target_compile_options(secp256k1_precomputed PRIVATE -fno-lto)
    endif()

    # Create an alias for consistent naming
    if(TARGET secp256k1)
        add_library(secp256k1::secp256k1 ALIAS secp256k1)
    endif()
else()
    message(STATUS "Found libsecp256k1 via pkg-config")
    # Create imported target for secp256k1
    add_library(secp256k1::secp256k1 INTERFACE IMPORTED)
    target_link_libraries(secp256k1::secp256k1 INTERFACE ${SECP256K1_LIBRARIES})
    target_include_directories(secp256k1::secp256k1 INTERFACE ${SECP256K1_INCLUDE_DIRS})
    target_compile_options(secp256k1::secp256k1 INTERFACE ${SECP256K1_CFLAGS_OTHER})
endif()

# OpenSSL - For SHA256 hashing and other crypto operations
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    message(STATUS "Found OpenSSL version: ${OPENSSL_VERSION}")
else()
    message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL development packages.")
endif()

# doctest - Testing framework (only for tests)
if(CIRCULAR_BUILD_TESTS)
    find_package(doctest QUIET)
    if(NOT doctest_FOUND)
        message(STATUS "Fetching doctest")
        FetchContent_Declare(
            doctest
            GIT_REPOSITORY "https://github.com/doctest/doctest.git"
            GIT_TAG "v2.4.11"
            GIT_SHALLOW ON
        )
        FetchContent_MakeAvailable(doctest)
    else()
        message(STATUS "Found doctest via find_package")
    endif()
endif()

# Platform-specific dependencies
if(WIN32)
    # Windows-specific dependencies
    # Note: cpp-httplib requires ws2_32 and winmm on Windows
    set(PLATFORM_LIBS ws2_32 winmm)
elseif(UNIX AND NOT APPLE)
    # Linux-specific dependencies
    find_package(Threads REQUIRED)
    set(PLATFORM_LIBS Threads::Threads)
elseif(APPLE)
    # macOS-specific dependencies
    find_package(Threads REQUIRED)
    set(PLATFORM_LIBS Threads::Threads)
endif()

# Create interface library for all dependencies
add_library(circular_dependencies INTERFACE)

target_link_libraries(circular_dependencies INTERFACE
    nlohmann_json::nlohmann_json
    httplib::httplib
    secp256k1::secp256k1
    OpenSSL::SSL
    OpenSSL::Crypto
    ${PLATFORM_LIBS}
)

# Set required features
target_compile_features(circular_dependencies INTERFACE cxx_std_20)

# Summary
message(STATUS "Dependency configuration complete:")
message(STATUS "  - nlohmann/json: ${nlohmann_json_VERSION}")
message(STATUS "  - cpp-httplib: Available")
message(STATUS "  - libsecp256k1: Available")
message(STATUS "  - OpenSSL: ${OPENSSL_VERSION}")
if(CIRCULAR_BUILD_TESTS)
    message(STATUS "  - doctest: Available")
endif()