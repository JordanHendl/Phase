/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Phase.h
 * Author: jhendl
 *
 * Created on November 10, 2021, 8:42 PM
 */

#pragma once

#include "Stone/Stone.h"
#include "Sky/Sky.h"
#include "Sea/Sea.h"

namespace ph
{
  class Camera ;
  class Phase
  {
    public:
      static auto initalize( const char* config, const Camera& camera ) -> void ;
  };
}
