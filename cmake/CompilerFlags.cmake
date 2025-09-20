# Common compiler flags and settings for Circular Enterprise APIs

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Common compiler flags
if(MSVC)
    # MSVC-specific flags
    add_compile_options(
        /W4                    # High warning level
        /WX                    # Treat warnings as errors
        /permissive-           # Strict conformance mode
        /Zc:__cplusplus        # Enable correct __cplusplus macro
        /EHsc                  # Exception handling model
    )

    # Release optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /DNDEBUG)
    endif()

    # Debug flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /Zi /DDEBUG)
    endif()

else()
    # GCC/Clang flags - only for our code, not dependencies
    # Note: These are applied via circular_target_properties function

    # Release optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
        # Note: LTO disabled due to secp256k1 C/C++ compatibility issues
        # add_compile_options(-flto)
        # add_link_options(-flto)
    endif()

    # Debug flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O0 -g3 -DDEBUG -fno-omit-frame-pointer)

        # AddressSanitizer for debug builds (optional)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            option(CIRCULAR_ENABLE_ASAN "Enable AddressSanitizer" OFF)
            if(CIRCULAR_ENABLE_ASAN)
                add_compile_options(-fsanitize=address)
                add_link_options(-fsanitize=address)
            endif()
        endif()
    endif()

    # Clang-specific flags
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(
            -Wno-c++98-compat      # We're targeting C++20+
            -Wno-c++98-compat-pedantic
        )
    endif()
endif()

# Platform-specific definitions
if(WIN32)
    add_compile_definitions(
        WIN32_LEAN_AND_MEAN    # Exclude rarely-used stuff from Windows headers
        NOMINMAX               # Don't define min/max macros
        _WIN32_WINNT=0x0A00    # Target Windows 10+
    )
endif()

# Export compile commands for development tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Position independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Function to apply common target properties
function(circular_target_properties target_name)
    target_compile_features(${target_name} PRIVATE cxx_std_20)

    # Set target properties
    set_target_properties(${target_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    # Apply strict compiler flags only to our targets
    if(NOT MSVC)
        target_compile_options(${target_name} PRIVATE
            -Wall                  # Enable most warnings
            -Wextra                # Enable extra warnings
            -Wpedantic             # Pedantic warnings
            -Wconversion           # Conversion warnings
            -Wsign-conversion      # Sign conversion warnings
            -Wnon-virtual-dtor     # Non-virtual destructor warnings
            -Woverloaded-virtual   # Overloaded virtual function warnings
            -Wcast-align           # Cast alignment warnings
            -Wunused               # Unused variable warnings
            -Wshadow               # Variable shadowing warnings
            -Wold-style-cast       # Old-style cast warnings
        )
    endif()

    # Link-time optimization for release builds
    # Note: LTO disabled due to secp256k1 C/C++ compatibility issues
    # if(CMAKE_BUILD_TYPE STREQUAL "Release")
    #     set_target_properties(${target_name} PROPERTIES
    #         INTERPROCEDURAL_OPTIMIZATION TRUE
    #     )
    # endif()
endfunction()