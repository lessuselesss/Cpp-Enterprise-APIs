# Tests CMakeLists.txt for Circular Enterprise APIs

# Enable testing
include(CTest)

# Find doctest (use FetchContent if not found)
if(NOT TARGET doctest::doctest)
    # doctest was fetched, create alias
    if(TARGET doctest)
        add_library(doctest::doctest ALIAS doctest)
    else()
        find_package(doctest REQUIRED)
    endif()
endif()

# Common test configuration function
function(add_circular_test test_name source_file)
    add_executable(${test_name} ${source_file})
    circular_target_properties(${test_name})
    target_link_libraries(${test_name}
        PRIVATE
            Circular::circular_enterprise_apis
            doctest::doctest
    )

    # Set test properties
    set_target_properties(${test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )

    # Add to test suite
    add_test(NAME ${test_name} COMMAND ${test_name})

    # Set environment for tests
    set_tests_properties(${test_name} PROPERTIES
        ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1"
    )
endfunction()

# Unit tests
add_circular_test(test_utils unit/test_utils.cpp)
add_circular_test(test_ccertificate unit/test_ccertificate.cpp)
add_circular_test(test_cep_account unit/test_cep_account.cpp)

# Integration tests (require environment variables)
add_circular_test(test_integration integration/test_integration.cpp)

# E2E tests (require environment variables and network access)
add_circular_test(test_e2e e2e/test_e2e.cpp)

# Create a custom target to run only unit tests
add_custom_target(test_unit
    COMMAND ${CMAKE_CTEST_COMMAND} -R "test_(utils|ccertificate|cep_account)" --verbose
    DEPENDS test_utils test_ccertificate test_cep_account
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running unit tests"
)

# Create a custom target to run integration tests
add_custom_target(test_integration_only
    COMMAND ${CMAKE_CTEST_COMMAND} -R "test_integration" --verbose
    DEPENDS test_integration
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running integration tests"
)

# Create a custom target to run E2E tests
add_custom_target(test_e2e_only
    COMMAND ${CMAKE_CTEST_COMMAND} -R "test_e2e" --verbose
    DEPENDS test_e2e
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running E2E tests"
)