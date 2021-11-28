#pragma once
#include <Catalyst/vk/Vulkan.h>
#include <CatalystEX/Math/Math.h>

namespace ph
{
  struct SkyConfig
  {
    // Simulation parameters
    unsigned width  ;
    unsigned height ;
    
    SkyConfig()
    {
      this->width  = 1280 ;
      this->height = 1024 ;
    }
  };
  
  using API = cata::ivk::Vulkan ;
  class Sky
  {
    public:
      Sky( cata::Commands<API>& cmds, const SkyConfig& config ) ;
      Sky( cata::Commands<API>& cmds ) ;
      ~Sky() ;
      auto updateConfig( const ph::SkyConfig& config ) -> void ;
      auto pass() -> const cata::RenderPass<API, cata::DefaultAllocator<API>>& ;
      auto update() -> void ;
      auto draw() -> void ;
    private:
      struct SkyData* data ;
      Sky() ;
  };
}

