cmake_minimum_required(VERSION 3.16)

project(QtLiquidGlass VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

if(APPLE)
    enable_language(OBJCXX)
endif()

add_library(QtLiquidGlass
    src/QtLiquidGlass.cpp
)

if(APPLE)
    target_sources(QtLiquidGlass PRIVATE src/QtLiquidGlass.mm)
    
    find_library(APPKIT_LIBRARY AppKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    
    target_link_libraries(QtLiquidGlass PRIVATE ${APPKIT_LIBRARY} ${FOUNDATION_LIBRARY})
endif()

target_include_directories(QtLiquidGlass PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(QtLiquidGlass PUBLIC Qt6::Core Qt6::Widgets)

# --- Installation ---

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS QtLiquidGlass
    EXPORT QtLiquidGlassTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT QtLiquidGlassTargets
    FILE QtLiquidGlassTargets.cmake
    NAMESPACE QtLiquidGlass::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtLiquidGlass
)

# Generate Config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/QtLiquidGlassConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QtLiquidGlassConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtLiquidGlass
)

# Generate Version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QtLiquidGlassConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QtLiquidGlassConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QtLiquidGlassConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtLiquidGlass
)

# Only build examples if this is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(examples/GlassDemo)
endif()
