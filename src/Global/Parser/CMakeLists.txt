# 
# Copyright (C) 2020 Jordan Hendl
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 

# 
# File:   CMakeLists.h
# Author: Jordan Hendl
# 
# Created on January 2nd, 2020, 11:30 PM
# 

set( Phase_Config_Headers
      Parser.h
      Configuration.h
   )

set( Phase_Config_Sources
      Parser.cpp
      Configuration.cpp
   )

if( UNIX )
  list( APPEND Phase_Config_Libraries stdc++fs )
endif()

add_library               ( pconfiguration SHARED  ${Phase_Config_Sources} ${Phase_Config_Headers} )
target_include_directories( pconfiguration PRIVATE ${Phase_Config_INCLUDES}                        ) 
target_link_libraries     ( pconfiguration PUBLIC  ${Phase_Config_Libraries}                       ) 