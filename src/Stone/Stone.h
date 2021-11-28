#pragma once
#include <Catalyst/vk/Vulkan.h>
#include <CatalystEX/Math/Math.h>

namespace ph
{
  struct StoneConfig
  {
    // Simulation parameters
    unsigned width  ;
    unsigned height ;
    
    StoneConfig()
    {
      this->width  = 1280 ;
      this->height = 1280 ;
    }
  };
  
  using API = cata::ivk::Vulkan ;
  class Stone
  {
    public:
      Stone( cata::Commands<API>& cmds, const StoneConfig& config ) ;
      Stone( cata::Commands<API>& cmds ) ;
      ~Stone() ;
      auto updateConfig( const ph::StoneConfig& config ) -> void ;
      auto pass() -> const cata::RenderPass<API, cata::DefaultAllocator<API>>& ;
      auto update() -> void ;
      auto draw() -> void ;
    private:
      struct StoneData* data ;
      Stone() ;
  };
}

