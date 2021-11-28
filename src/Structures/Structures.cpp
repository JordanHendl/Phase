/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Structures.cpp
 * Author: jhendl
 * 
 * Created on November 10, 2021, 8:49 PM
 */

#include "Structures.h"
#include <vulkan/vulkan.hpp>
#include <vector>

namespace ph
{
  Camera::Camera() = default ;

  Camera::~Camera() = default ;

  Camera::Camera( const Camera& camera )
  {
    this->m_view = camera.m_view ;
    this->m_proj = camera.m_proj ;
  }

  auto Camera::setView( imp::ex::Mat4& matrix ) -> void
  {
    this->m_view = matrix ;
  }

  auto Camera::setProjection( imp::ex::Mat4& matrix ) -> void
  {
    this->m_proj = matrix ;
  }
  
  auto Camera::setPosition( imp::ex::Vec3& pos ) -> void
  {
    this->m_pos = pos ;
  }
  
  auto Camera::view() const -> const imp::ex::Mat4&
  {
    return this->m_view ;
  }

  auto Camera::proj() const -> const imp::ex::Mat4&
  {
    return this->m_proj ;
  }
  
  auto Camera::position() const -> const imp::ex::Vec3&
  {
    return this->m_pos ;
  }
}
