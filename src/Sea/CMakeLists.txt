find_package( EXImpulse REQUIRED )
find_package( Impulse   REQUIRED )

set( Sea_Sources 
     Sea.cpp
   )

set( Sea_Libraries
     timer
     pglobal
     pstructures 
     imp::impvulkan 
     eximp::image_loader
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