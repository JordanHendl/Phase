find_package( CatalystEX REQUIRED )
find_package( Catalyst   REQUIRED )

set( Stone_Sources 
     Stone.cpp
   )

set( Stone_Libraries
     pglobal
     pstructures 
     cata::vulkan 
     excata::image_loader
   )

set( Stone_Dependencies
     stone_draw_compiled
   )

add_library               ( stone ${Stone_Sources}          )
target_include_directories( stone PRIVATE impl              )
target_link_libraries     ( stone PUBLIC ${Stone_Libraries} ) 
add_dependencies          ( stone ${Stone_Dependencies}     )