add_executable(TestGlassShape
    TestGlassShape.mm
)

find_library(APPKIT_LIBRARY AppKit REQUIRED)

target_link_libraries(TestGlassShape PRIVATE
    Qt6::Core
    Qt6::Widgets
    QtLiquidGlass
    ${APPKIT_LIBRARY}
)

target_include_directories(TestGlassShape PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

add_test(NAME QtLiquidGlass.CustomShape COMMAND TestGlassShape)

add_executable(TestGlassLifecycle
    TestGlassLifecycle.mm
)

target_include_directories(TestGlassLifecycle PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(TestGlassLifecycle PRIVATE
    Qt6::Core
    Qt6::Widgets
    QtLiquidGlass
    ${APPKIT_LIBRARY}
)

add_test(NAME QtLiquidGlass.Lifecycle COMMAND TestGlassLifecycle)

add_executable(TestGlassConfiguration
    TestGlassConfiguration.mm
)

target_include_directories(TestGlassConfiguration PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(TestGlassConfiguration PRIVATE
    Qt6::Core
    Qt6::Widgets
    QtLiquidGlass
    ${APPKIT_LIBRARY}
)

add_test(NAME QtLiquidGlass.Configuration COMMAND TestGlassConfiguration)
