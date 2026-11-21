cmake_minimum_required(VERSION 3.20..4.0)

project(symplast
    VERSION 0.1.0
    DESCRIPTION "A Cpp library for building applications with webui."
    LANGUAGES CXX)

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

# ---------------------------------------------------------------------------
# Build type switch
# ---------------------------------------------------------------------------
option(SYMPLAST_BUILD_SHARED "Build symplast as a shared library" OFF)

if(SYMPLAST_BUILD_SHARED)
    set(SYMPLAST_LIB_TYPE SHARED)
else()
    set(SYMPLAST_LIB_TYPE STATIC)
endif()

# ---------------------------------------------------------------------------
# Library target
# ---------------------------------------------------------------------------
if(WIN32)
set(SYMPLAST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/win)
endif()
add_library(symplast ${SYMPLAST_LIB_TYPE}
    ${SYMPLAST_SOURCE_DIR}/application.cc
    src/win/utils.cc
)

# Namespaced alias for consumption (downstream always links symplast::symplast)
add_library(symplast::symplast ALIAS symplast)

# ---------------------------------------------------------------------------
# Include directories: separate build tree and install tree so that no
# absolute paths leak into the exported package configuration
# ---------------------------------------------------------------------------
target_include_directories(symplast
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(symplast PUBLIC cxx_std_20)

if(WIN32)
    message(STATUS "Platform: Windows, Backend: WebView2")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/backends/webview2.cmake)
    symplast_link_webview2(symplast 1.0.2277.86)
endif()

# ---------------------------------------------------------------------------
# Symbol export/import macros
# ---------------------------------------------------------------------------
if(SYMPLAST_BUILD_SHARED)
    # Consumers need to know the library is shared (-> dllimport on Windows).
    target_compile_definitions(symplast PUBLIC SYMPLAST_BUILD_SHARED)
    # Only the library itself sees the dllexport side.
    target_compile_definitions(symplast PRIVATE SYMPLAST_EXPORTS)

    set_target_properties(symplast PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        VERSION  ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

# Useful when a static symplast is later linked into a shared library.
set_target_properties(symplast PROPERTIES POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# Install and export
# ---------------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS symplast
    EXPORT symplastTargets
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT symplastTargets
    FILE      symplastTargets.cmake
    NAMESPACE symplast::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/symplast
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/symplastConfigVersion.cmake"
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/symplastConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/symplastConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/symplast
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/symplastConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/symplastConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/symplast
)

# ---------------------------------------------------------------------------
# Examples
# ---------------------------------------------------------------------------
option(SYMPLAST_BUILD_EXAMPLES "Build examples" ${MAIN_PROJECT})

if(SYMPLAST_BUILD_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples/basic)
endif()