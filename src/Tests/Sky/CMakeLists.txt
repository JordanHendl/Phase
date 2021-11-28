find_package( Catalyst REQUIRED )

add_executable( ph_sky_test Test.cpp )
target_link_libraries( ph_sky_test PUBLIC phase cata::window ) 