/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Structures.h
 * Author: jhendl
 *
 * Created on November 10, 2021, 8:49 PM
 */

#pragma once
#include <EXImpulse/Math/Math.h>

namespace ph
{
  class Camera
  {
    public:
      Camera() ;
      Camera( const Camera& camera ) ;
      ~Camera() ;
      auto setView( imp::ex::Mat4& matrix ) -> void ;
      auto setProjection( imp::ex::Mat4& matrix ) -> void ;
      auto setPosition( imp::ex::Vec3& pos ) -> void ;
      auto view() const -> const imp::ex::Mat4& ;
      auto proj() const -> const imp::ex::Mat4& ;
      auto position() const -> const imp::ex::Vec3& ;
    private:
      imp::ex::Mat4 m_view ;
      imp::ex::Mat4 m_proj ;
      imp::ex::Vec3 m_pos  ;
  };
}

