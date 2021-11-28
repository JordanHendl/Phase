#pragma once
#include <Impulse/vk/Vulkan.h>
#include <EXImpulse/Math/Math.h>

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
  
  using API = imp::ivk::Vulkan ;
  class Stone
  {
    public:
      Stone( imp::Commands<API>& cmds, const StoneConfig& config ) ;
      Stone( imp::Commands<API>& cmds ) ;
      ~Stone() ;
      auto updateConfig( const ph::StoneConfig& config ) -> void ;
      auto pass() -> const imp::RenderPass<API, imp::DefaultAllocator<API>>& ;
      auto update() -> void ;
      auto draw() -> void ;
    private:
      struct StoneData* data ;
      Stone() ;
  };
}

