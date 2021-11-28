#pragma once
#include <Catalyst/vk/Vulkan.h>
#include <CatalystEX/Math/Math.h>

namespace ph
{
  constexpr auto pi = 3.14159265358979323846 ;
  
  struct SeaConfig
  {
    // Simulation parameters
    unsigned width       ;
    unsigned height      ;
    float    gravity     ; // gravitational constant
    float    wave_scale  ; // wave scale factor
    float    patch_size  ; // patch size
    float    wind_speed  ;
    float    wind_dir    ;
    float    dir_depend  ;
    float    h_scale     ;
    float    time        ;
    
    SeaConfig()
    {
      this->width       = 256       ;
      this->height      = 256       ;
      this->gravity     = 9.81f     ;
      this->wave_scale  = 1e-7f     ;
      this->patch_size  = 100       ;
      this->wind_speed  = 100.0f    ;
      this->wind_dir    = pi / 3.0f ;
      this->dir_depend  = 0.07f     ;  
      this->time        = 0.0f      ;
      this->h_scale     = 10.0f     ;
    }
  };
  
  class Camera ;
  using API = cata::ivk::Vulkan ;
  class Sea
  {
    public:
      Sea( cata::Commands<API>& cmds, const SeaConfig& config ) ;
      Sea( cata::Commands<API>& cmds ) ;
      ~Sea() ;
      auto updateConfig( const ph::SeaConfig& config ) -> void ;
      auto pass() -> const cata::RenderPass<API, cata::DefaultAllocator<API>>& ;
      auto commands() -> cata::Commands<API>& ;
      auto update() -> void ;
      auto draw() -> void ;
    private:
      struct SeaData* ocean_data ;
      auto data() -> SeaData& ;
      Sea() ;
  };
}

