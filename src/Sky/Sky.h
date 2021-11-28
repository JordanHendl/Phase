#pragma once
#include <Impulse/vk/Vulkan.h>
#include <EXImpulse/Math/Math.h>

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
  
  using API = imp::ivk::Vulkan ;
  class Sky
  {
    public:
      Sky( imp::Commands<API>& cmds, const SkyConfig& config ) ;
      Sky( imp::Commands<API>& cmds ) ;
      ~Sky() ;
      auto updateConfig( const ph::SkyConfig& config ) -> void ;
      auto pass() -> const imp::RenderPass<API, imp::DefaultAllocator<API>>& ;
      auto update() -> void ;
      auto draw() -> void ;
    private:
      struct SkyData* data ;
      Sky() ;
  };
}

