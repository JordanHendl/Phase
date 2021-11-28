/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Global.h
 * Author: jhendl
 *
 * Created on November 26, 2021, 2:51 AM
 */

#pragma once
#include <vector>
#include <string>

namespace ph
{
  class Camera ;
  struct GlobalData
  { 
    std::vector<std::string> skybox_images   ;
    const Camera*            camera          ;
    std::string              stone_base_path ;
    
    GlobalData() ;
  };
  
  auto g_data() -> GlobalData& ;
}
