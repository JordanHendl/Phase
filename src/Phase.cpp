/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Phase.cpp
 * Author: jhendl
 * 
 * Created on November 10, 2021, 8:42 PM
 */

#include "Phase.h"
#include "Global/Global/Global.h"
#include "Structures/Structures.h"
#include <Catalyst/vk/Conversion.h>
#include <Catalyst/vk/Vulkan.h>

using API = cata::ivk::Vulkan ;

namespace ph
{
   auto Phase::initalize( const char* config, const Camera& camera ) -> void
   {
     API::addValidationLayer( "VK_LAYER_KHRONOS_validation"         ) ;
     API::addValidationLayer( "VK_LAYER_LUNARG_standard_validation" ) ;
     API::initialize() ;
     
     
     g_data().camera = &camera ;
   }
}