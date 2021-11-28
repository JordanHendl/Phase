find_package( CatalystEX REQUIRED )
find_package( Catalyst   REQUIRED )

set( Sea_Sources 
     Sea.cpp
   )

set( Sea_Libraries
     timer
     pglobal
     pstructures 
     cata::vulkan 
     excata::image_loader
   )

set( Sea_Dependencies
     sea_process_frequency_compiled
     sea_update_heightmap_compiled 
     sea_calc_slope_compiled       
     sea_draw_compiled                
   )

add_library               ( sea ${Sea_Sources}          )
target_include_directories( sea PRIVATE impl            )
target_link_libraries     ( sea PUBLIC ${Sea_Libraries} ) 
add_dependencies          ( sea ${Sea_Dependencies}     )