find_package( Impulse REQUIRED )

add_executable( ph_stone_test Test.cpp )
target_link_libraries( ph_stone_test PUBLIC phase imp::impwindow ) 