# Source directory CMakeLists.txt for Circular Enterprise APIs

# Define the library sources
set(CIRCULAR_SOURCES
    cep_account.cpp
    ccertificate.cpp
    utils.cpp
    network.cpp
    env_loader.cpp
)

# Define the library headers
set(CIRCULAR_HEADERS
    ../include/circular/circular_enterprise_apis.hpp
    ../include/circular/cep_account.hpp
    ../include/circular/ccertificate.hpp
    ../include/circular/utils.hpp
    ../include/circular/env_loader.hpp
)

# Create the main library
add_library(circular_enterprise_apis ${CIRCULAR_SOURCES} ${CIRCULAR_HEADERS})

# Apply common target properties
circular_target_properties(circular_enterprise_apis)

# Set target properties
set_target_properties(circular_enterprise_apis PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "circular_enterprise_apis"
    EXPORT_NAME "circular_enterprise_apis"
)

# Link dependencies directly instead of through interface library
target_link_libraries(circular_enterprise_apis
    PUBLIC
        nlohmann_json::nlohmann_json
        httplib::httplib
        secp256k1::secp256k1
        OpenSSL::SSL
        OpenSSL::Crypto
        ${PLATFORM_LIBS}
)

# Include directories
target_include_directories(circular_enterprise_apis
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler definitions
target_compile_definitions(circular_enterprise_apis
    PUBLIC
        CIRCULAR_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        CIRCULAR_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        CIRCULAR_VERSION_PATCH=${PROJECT_VERSION_PATCH}
        CIRCULAR_VERSION_STRING="${PROJECT_VERSION}"
)

# Platform-specific configurations
if(WIN32)
    target_compile_definitions(circular_enterprise_apis PRIVATE
        _CRT_SECURE_NO_WARNINGS  # Disable MSVC security warnings for standard functions
    )
endif()

# Create alias for easier usage in subdirectories
add_library(Circular::circular_enterprise_apis ALIAS circular_enterprise_apis)