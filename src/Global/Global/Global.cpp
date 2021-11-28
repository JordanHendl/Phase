/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Global.cpp
 * Author: jhendl
 * 
 * Created on November 26, 2021, 2:51 AM
 */

#include "Global.h"

namespace ph
{
  GlobalData::GlobalData()
  {
    this->camera = nullptr ;
    
    this->stone_base_path = "/wksp/assets/stone/test/" ;
    
    this->skybox_images = 
    {
      "/wksp/assets/pictures/skybox/1/right.jpg",   
      "/wksp/assets/pictures/skybox/1/left.jpg",  
      "/wksp/assets/pictures/skybox/1/top.jpg",   
      "/wksp/assets/pictures/skybox/1/bottom.jpg",
      "/wksp/assets/pictures/skybox/1/front.jpg", 
      "/wksp/assets/pictures/skybox/1/back.jpg",
    };
  }
  
  auto g_data() -> GlobalData&
  {
    static GlobalData data ;
    
    return data ;
  }
}
