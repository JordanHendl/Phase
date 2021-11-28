add_subdirectory( Global     ) 
add_subdirectory( Shaders    ) 
add_subdirectory( Structures ) 
add_subdirectory( Stone      ) 
add_subdirectory( Sky        ) 
add_subdirectory( Timer      ) 
add_subdirectory( Sea        ) 
add_subdirectory( Tests      ) 

find_package( EXImpulse REQUIRED )
find_package( Impulse   REQUIRED )

set( Phase_Sources 
     Phase.cpp
   )

set( Phase_Libraries
     stone 
     sky
     sea
   )


add_library( phase ${Phase_Sources} )
target_include_directories( phase PUBLIC impl               )
target_link_libraries     ( phase PUBLIC ${Phase_Libraries} ) 