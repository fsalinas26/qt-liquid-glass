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

add_test(NAME QtLiquidGlass.CustomShape COMMAND TestGlassShape)
