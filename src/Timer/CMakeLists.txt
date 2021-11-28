SET( Timer_SOURCES 
     Timer.cpp
   )

SET( Timer_HEADERS
   )

SET( Timer_LIBRARIES
   )

SET( Timer_INCLUDE_DIRS
   )

ADD_LIBRARY               ( timer SHARED  ${Timer_SOURCES} ${Timer_HEADERS} )
TARGET_LINK_LIBRARIES     ( timer PUBLIC  ${Timer_LIBRARIES}                )
TARGET_INCLUDE_DIRECTORIES( timer PRIVATE ${Timer_INCLUDE_DIRS}             )