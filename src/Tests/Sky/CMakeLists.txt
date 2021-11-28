find_package( Impulse REQUIRED )

add_executable( ph_sky_test Test.cpp )
target_link_libraries( ph_sky_test PUBLIC phase imp::impwindow ) 