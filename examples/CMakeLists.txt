# Examples CMakeLists.txt for Circular Enterprise APIs

# Simple certificate submission example
add_executable(simple_certificate_submission simple_certificate_submission.cpp)
circular_target_properties(simple_certificate_submission)
target_link_libraries(simple_certificate_submission
    PRIVATE
        Circular::circular_enterprise_apis
)

# Certify poem example
add_executable(certify_poem certify_poem.cpp)
circular_target_properties(certify_poem)
target_link_libraries(certify_poem
    PRIVATE
        Circular::circular_enterprise_apis
)

# Batch certificates example
add_executable(batch_certificates batch_certificates.cpp)
circular_target_properties(batch_certificates)
target_link_libraries(batch_certificates
    PRIVATE
        Circular::circular_enterprise_apis
)

# Certificate with metadata example
add_executable(certificate_with_metadata certificate_with_metadata.cpp)
circular_target_properties(certificate_with_metadata)
target_link_libraries(certificate_with_metadata
    PRIVATE
        Circular::circular_enterprise_apis
)

# Error handling demo
add_executable(error_handling_demo error_handling_demo.cpp)
circular_target_properties(error_handling_demo)
target_link_libraries(error_handling_demo
    PRIVATE
        Circular::circular_enterprise_apis
)

# Set output directory for examples
set_target_properties(
    simple_certificate_submission
    certify_poem
    batch_certificates
    certificate_with_metadata
    error_handling_demo
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
)