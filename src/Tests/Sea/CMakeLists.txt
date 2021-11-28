find_package( Impulse REQUIRED )

add_executable( ph_sea_test Test.cpp )
target_link_libraries( ph_sea_test PUBLIC phase imp::impwindow ) 