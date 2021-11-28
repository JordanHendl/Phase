find_package( Catalyst REQUIRED )

add_executable( ph_stone_test Test.cpp )
target_link_libraries( ph_stone_test PUBLIC phase cata::window ) 