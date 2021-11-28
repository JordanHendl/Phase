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
#include <CatalystEX/Math/Math.h>

namespace ph
{
  class Camera
  {
    public:
      Camera() ;
      Camera( const Camera& camera ) ;
      ~Camera() ;
      auto setView( cata::ex::Mat4& matrix ) -> void ;
      auto setProjection( cata::ex::Mat4& matrix ) -> void ;
      auto setPosition( cata::ex::Vec3& pos ) -> void ;
      auto view() const -> const cata::ex::Mat4& ;
      auto proj() const -> const cata::ex::Mat4& ;
      auto position() const -> const cata::ex::Vec3& ;
    private:
      cata::ex::Mat4 m_view ;
      cata::ex::Mat4 m_proj ;
      cata::ex::Vec3 m_pos  ;
  };
}

