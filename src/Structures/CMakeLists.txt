find_package( EXImpulse REQUIRED )
find_package( Impulse REQUIRED   )

add_library( pstructures STATIC Structures.cpp )
target_link_libraries( pstructures imp::impvulkan eximp::fft )


