find_package( EXImpulse REQUIRED )
find_package( Impulse   REQUIRED )

set( Sky_Sources 
     Sky.cpp
   )

set( Sky_Libraries
     pglobal
     pstructures 
     imp::impvulkan 
     eximp::image_loader
   )

set( Sky_Dependencies
     sky_draw_compiled
   )

add_library               ( sky ${Sky_Sources}          )
target_include_directories( sky PRIVATE impl            )
target_link_libraries     ( sky PUBLIC ${Sky_Libraries} ) 
add_dependencies          ( sky ${Sky_Dependencies}     )