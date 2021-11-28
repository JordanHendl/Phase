find_package( CatalystEX REQUIRED )
find_package( Catalyst REQUIRED   )

add_library( pstructures STATIC Structures.cpp )
target_link_libraries( pstructures cata::vulkan excata::fft )


