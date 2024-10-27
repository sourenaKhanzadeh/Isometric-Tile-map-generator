function(fetch_and_create_library target_name source_dir)
    # Collect all .cpp and .hpp files in the specified directory
    file(GLOB_RECURSE CPP_FILES CONFIGURE_DEPENDS "${source_dir}/*.cpp")
    file(GLOB_RECURSE HPP_FILES CONFIGURE_DEPENDS "${source_dir}/*.hpp")

    # Check if any source files were found
    if(NOT CPP_FILES AND NOT HPP_FILES)
        message(FATAL_ERROR "No source files found in directory: ${source_dir}")
    endif()

    # Create the library
    add_library(${target_name} ${CPP_FILES} ${HPP_FILES})

    # Set include directories for the library
    target_include_directories(${target_name} PUBLIC "${source_dir}")

    # Set the output directory for the library
    set_target_properties(${target_name} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    # Link dependencies if provided
    if(ARGN)
        target_link_libraries(${target_name} PUBLIC ${ARGN})
        message(STATUS "Dependencies for ${target_name}: ${ARGN}")
    endif()

    # Get the number of source files
    list(LENGTH CPP_FILES NUM_CPP_FILES)
    list(LENGTH HPP_FILES NUM_HPP_FILES)

    # Print information about the created library
    message(STATUS "Created library: ${target_name}")
    message(STATUS "Source directory: ${source_dir}")
    message(STATUS "Number of .cpp files: ${NUM_CPP_FILES}")
    message(STATUS "Number of .hpp files: ${NUM_HPP_FILES}")
endfunction()
