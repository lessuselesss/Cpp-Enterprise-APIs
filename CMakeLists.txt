cmake_minimum_required(VERSION 3.20)

project(CircularEnterpriseAPIs
    VERSION 1.0.13
    DESCRIPTION "Official Circular Protocol Enterprise APIs for Data Certification - C++ Implementation"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include common compiler flags and settings
include(cmake/CompilerFlags.cmake)

# Options
option(CIRCULAR_BUILD_TESTS "Build tests" ON)
option(CIRCULAR_BUILD_EXAMPLES "Build examples" ON)
option(CIRCULAR_USE_CONAN "Use Conan for dependency management" OFF)
option(CIRCULAR_USE_VCPKG "Use vcpkg for dependency management" OFF)

# Find or fetch dependencies
include(cmake/FindDependencies.cmake)

# Add the main library
add_subdirectory(src)

# Add examples if requested
if(CIRCULAR_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Add tests if requested
if(CIRCULAR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install configuration (commented out for now to allow building)
# include(GNUInstallDirs)
#
# # Export targets
# install(TARGETS circular_enterprise_apis
#     EXPORT CircularEnterpriseAPIsTargets
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )