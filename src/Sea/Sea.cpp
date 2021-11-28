/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Sea.cpp
 * Author: jhendl
 * 
 * Created on November 10, 2021, 8:45 PM
 */

#include "Sea.h"
#include "Global/Global/Global.h"
#include <sea_draw.h>
#include <sea_process_frequency.h>
#include <sea_update_heightmap.h>
#include <sea_calc_slope.h>
#include <Timer/Timer.h>
#include "Structures/Structures.h"
#include <Catalyst/vk/Vulkan.h>
#include <Catalyst/structures/Structures.h>
#include <CatalystEX/Algorithm/FFT.h>
#include <CatalystEX/Math/Math.h>
#include <iostream>
#include <random>
#include <cmath>
using API = cata::ivk::Vulkan ;

using namespace cata ;

namespace ph
{
  struct ViewProj
  {
    cata::ex::Mat4 vp  ;
    cata::ex::Vec3 pos ;
  };

  struct SeaVertex
  {
    cata::ex::Vec4 pos    ;
    cata::ex::Vec4 normal ;
    cata::ex::Vec2 uv     ;
  };
  
  struct SeaData
  {
    cata::ex::FFT*                      fft         ;
    cata::RenderPass<API>               pass        ;
    cata::Commands<API>                 fft_cmds    ;
    cata::Commands<API>*                cmd         ;
    cata::Pipeline<API>                 draw_pipe   ;
    cata::Pipeline<API>                 proc_pipe   ;
    cata::Pipeline<API>                 sign_correct;
    cata::Pipeline<API>                 slope_calc  ;
    cata::VertexArray<API, SeaVertex>   vertices    ;
    cata::IndexArray<API, unsigned>     indices     ;
    cata::UniformArray<API, ViewProj>   viewproj    ;
    cata::UniformArray<API, SeaConfig>  d_cfg       ;
    cata::Array<API, cata::ex::Vec3>     viewpos     ;
    cata::Array<API, cata::ex::Complex>  fft_buffer  ;
    cata::Array<API, cata::ex::Complex>  time_buffer ;
    cata::Array<API, ex::Vec2        >  slopes      ;
    cata::Array<API, float>             h_offsets   ;
    cata::Array<API, float>             v_offsets   ;
    cata::Texture<API>                  heightmap   ;
    cata::Texture<API>                  normalmap   ;
    Camera*                            cameras     ;
    std::vector<SeaVertex>             h_vertices  ;
    std::vector<unsigned>              h_indices   ;
    Timer                              timer       ;
    SeaConfig                          cfg         ;
    
    bool initialized = false ;
    
    SeaData( cata::Commands<API>& cmds, const SeaConfig& config ) ;
    
    auto initializeBuffers() -> void ;
    
    auto buildVertices() -> void ;
    
    auto triangulate() -> void ;
    
    auto generateH0() -> std::vector<cata::ex::Complex> ;
    
    auto randomGaussian() -> std::vector<cata::ex::Complex> ;
    
    auto updateFFT() -> void ;
    
    // Generates Gaussian random number with mean 0 and standard deviation 1.
    auto gauss() -> float ;
    
    // Phillips spectrum
    // (Kx, Ky) - normalized wave vector
    // Vdir - wind angle in radians
    // V - wind speed
    // A - constant
    auto phillips( float Kx, float Ky, float Vdir, float V, float A, float dir_depend ) -> float ;
  };
  
  SeaData::SeaData( cata::Commands<API>& cmds, const SeaConfig& config )
  {
    auto pass_info     = RenderPassInfo() ;
    auto subpass       = Subpass       () ;
    auto attachment    = Attachment    () ;
    auto viewport      = Viewport      () ;
    auto pipeline_info = PipelineInfo  () ;

    this->cmd = &cmds ;

    viewport.setWidth ( 1280 ) ;
    viewport.setHeight( 1024 ) ;

    pipeline_info.setFileBytes( bytes::sea_draw, sizeof( bytes::sea_draw ) ) ;
    pipeline_info.addViewport( viewport ) ;
    pipeline_info.setDepthTest( true ) ;

    // Configure attachments
    attachment.setFormat( Format::RGBA8           ) ;
    attachment.setLayout( Layout::ColorAttachment ) ;
    attachment.setClearColor( 0.0f, 0.1f, 0.1f, 1.0f ) ;

    // Configure subpasses
    subpass.addAttachment( attachment ) ;
    subpass.setDepthStencilEnable( true ) ;
    pass_info.addSubpass   ( subpass    ) ;

    this->pass = RenderPass<API>( 0, pass_info ) ;
    this->draw_pipe = Pipeline<API>  ( pass, pipeline_info ) ;
    
    pipeline_info.setFileBytes( bytes::sea_process_frequency, sizeof( bytes::sea_process_frequency ) ) ;
    this->proc_pipe = Pipeline<API>( 0, pipeline_info ) ;

    pipeline_info.setFileBytes( bytes::sea_update_heightmap, sizeof( bytes::sea_update_heightmap ) ) ;
    this->sign_correct = Pipeline<API>( 0, pipeline_info ) ;
    
    pipeline_info.setFileBytes( bytes::sea_calc_slope, sizeof( bytes::sea_calc_slope ) ) ;
    this->slope_calc = Pipeline<API>( 0, pipeline_info ) ;


    this->buildVertices()     ;
    this->triangulate()       ;
    this->initializeBuffers() ;

    ex::FFTConfig fft_config ;
    fft_config.width     = this->cfg.width  ;
    fft_config.height    = this->cfg.height ;
    fft_config.normalize = false       ;

    this->fft = new cata::ex::FFT( cmds, fft_config ) ;

    this->updateFFT() ;

    this->slope_calc  .bind( "in_buffer" , this->v_offsets   ) ;
    this->slope_calc  .bind( "out_buffer", this->slopes      ) ;
    this->slope_calc  .bind( "config"    , this->d_cfg       ) ;
    this->draw_pipe   .bind( "viewproj"  , this->viewproj    ) ;
    this->draw_pipe   .bind( "heightmap" , this->heightmap   ) ;
    this->draw_pipe   .bind( "normalmap" , this->normalmap   ) ;
    this->draw_pipe   .bind( "config"    , this->d_cfg       ) ;
    this->proc_pipe   .bind( "in_signal" , this->fft_buffer  ) ;
    this->proc_pipe   .bind( "out_signal", this->time_buffer ) ;
    this->proc_pipe   .bind( "config"    , this->d_cfg       ) ;
    this->sign_correct.bind( "in_signal" , this->time_buffer ) ;
    this->sign_correct.bind( "out_signal", this->v_offsets   ) ;
    this->sign_correct.bind( "config"    , this->d_cfg       ) ;

    this->initialized = true ;
    this->timer.start() ;
  }
   
  auto SeaData::gauss() -> float
  {
    float u1 = rand() / static_cast<float>( RAND_MAX ) ;
    float u2 = rand() / static_cast<float>( RAND_MAX ) ;
  
    if (u1 < 1e-6f)
    {
        u1 = 1e-6f;
    }
  
    return sqrtf(-2 * logf(u1)) * cosf(2*M_PI * u2);
  }

  auto SeaData::phillips( float Kx, float Ky, float Vdir, float V, float A, float dir_depend ) -> float
  {
    float k_squared = Kx * Kx + Ky * Ky;
  
    if (k_squared == 0.0f)
    {
        return 0.0f;
    }
  
    // largest possible wave from constant wind of velocity v
    float L = V * V / this->cfg.gravity ;
  
    float k_x = Kx / sqrtf(k_squared);
    float k_y = Ky / sqrtf(k_squared);
    float w_dot_k = k_x * cosf(Vdir) + k_y * sinf(Vdir);
  
    float phillips = A * expf( -1.0f / ( k_squared * L * L ) ) / ( k_squared * k_squared ) * w_dot_k * w_dot_k ;
  
    // filter out waves moving opposite to wind
    if (w_dot_k < 0.0f)
    {
        phillips *= dir_depend;
    }
  
    // damp out waves with very small length w << l
    //float w = L / 10000;
    //phillips *= expf(-k_squared * w * w);
  
    return phillips;
  }

  auto SeaData::generateH0() -> std::vector<ex::Complex>
  {
    auto specW = ( this->cfg.width  + 4 ) ;
    auto specH = ( this->cfg.height + 1 ) ;
    
    std::vector<ex::Complex> ret ;
    ret.resize( specW * specH ) ;
    
    for (unsigned int y = 0; y < this->cfg.height; y++)
    {
      for (unsigned int x = 0; x < this->cfg.width ; x++)
      {
        float kx = ( -static_cast<int>( this->cfg.width  ) / 2.0f + x ) * (2.0f * M_PI / this->cfg.patch_size ) ;
        float ky = ( -static_cast<int>( this->cfg.height ) / 2.0f + y ) * (2.0f * M_PI / this->cfg.patch_size ) ;

        float P = std::sqrt( this->phillips( kx, ky, this->cfg.wind_dir, this->cfg.wind_speed, this->cfg.wave_scale, this->cfg.dir_depend ) ) ;

        if (kx == 0.0f && ky == 0.0f)
        {
            P = 0.0f;
        }

        float Er = this->gauss() ;
        float Ei = this->gauss() ;

        float h0_re = Er * P * M_SQRT1_2 ;
        float h0_im = Ei * P * M_SQRT1_2 ;
        if( h0_re != h0_re ) throw std::runtime_error( "NAN detected!" ) ;
        if( h0_im != h0_im ) throw std::runtime_error( "NAN detected!" ) ;

        int i = ( y * specW ) + x ;
        ret[ i ].r = h0_re ;
        ret[ i ].i = h0_im ;
      }
    }
    
    return ret ;
  }

  auto SeaData::updateFFT() -> void
  {
    auto specW = ( this->cfg.width  + 4 ) ;
    auto specH = ( this->cfg.height + 1 ) ;
    const auto size = specW * specH ;
    auto wb         = Array   <API , cata::ex::Complex>( 0, size, true ) ;
    auto randoms    = this->generateH0() ;
    
    this->fft_cmds.begin() ;
    
    this->fft_cmds.copy( randoms.data(), wb   ) ;
    this->fft_cmds.copy( wb, this->fft_buffer ) ;
    
    this->fft_cmds.submit     () ;
    this->fft_cmds.synchronize() ;
  }

  auto SeaData::initializeBuffers() -> void
  {
    const auto size          = this->cfg.width * this->cfg.height ;
    const auto spectrum_size = ( this->cfg.width + 4 ) * ( this->cfg.height + 1 ) ;
    
    auto cmds = Commands<API>( 0, SubmitType::Graphics ) ;
    
    auto tex_info = TextureInfo() ;
    tex_info.setDimensions( this->cfg.width, this->cfg.height ) ;
    tex_info.setFormat( Format::R32F ) ;
    
    auto buff = Commands<API>( 0, SubmitType::Graphics ) ;
    
    // Maps
    auto hmap = Texture<API>( 0, tex_info ) ;
    
    tex_info.setFormat( Format::RG32F ) ;
    auto wmap = Texture<API>( 0, tex_info ) ;
    
    // Writeback buffers
    auto wb   = VertexArray<API, SeaVertex>( 0, this->h_vertices.size(), true ) ;
    auto wb2  = IndexArray<API , unsigned   >( 0, this->h_indices.size() , true ) ;
    
    //Uniform arrays
    auto vp    = UniformArray<API, ViewProj >( 0, 1, true ) ;
    auto param = UniformArray<API, SeaConfig>( 0, 1, true ) ;
    
    auto vertices = VertexArray<API, SeaVertex     >( 0, this->h_vertices.size() ) ;
    auto indices  = IndexArray<API , unsigned        >( 0, this->h_indices .size() ) ;
    
    //FFT buffers.
    auto fft   = Array<API, cata::ex::Complex>( 0, spectrum_size ) ;
    auto fft_t = Array<API, cata::ex::Complex>( 0, size ) ;
    auto h_off = Array<API, float           >( 0, size ) ;
    auto v_off = Array<API, float           >( 0, size ) ;
    
    buff.copy( this->h_vertices.data(), wb       ) ;
    buff.copy( this->h_indices .data(), wb2      ) ;
    buff.copy( wb                     , vertices ) ;
    buff.copy( wb2                    , indices  ) ;
    
    buff.submit() ;
    buff.synchronize() ;
    
    this->slopes = Array<API, ex::Vec2>( 0, size ) ;

    this->time_buffer.swap( fft_t    ) ;
    this->fft_cmds   .swap( cmds     ) ;
    this->vertices   .swap( vertices ) ;
    this->indices    .swap( indices  ) ;
    this->viewproj   .swap( vp       ) ;
    this->heightmap  .swap( hmap     ) ;
    this->normalmap  .swap( wmap     ) ;
    this->fft_buffer .swap( fft      ) ;
    this->h_offsets  .swap( h_off    ) ;
    this->v_offsets  .swap( v_off    ) ;
    this->d_cfg      .swap( param    ) ;
  }
  
  auto SeaData::buildVertices() -> void
  {
    const float dist = 1.0f ;
    
    this->h_vertices.resize( this->cfg.width * this->cfg.height ) ;
    
    for( int iy = 0; iy < this->cfg.height; iy++ )
    {
      for( int ix = 0; ix < this->cfg.width; ix++ )
      {
        auto index = ix + ( iy * this->cfg.width ) ;
        
        auto& vertex = this->h_vertices[ index ] ;
        vertex.pos.x() = static_cast<float>( ix ) * dist ;
        vertex.pos.y() = 0.0f                            ;
        vertex.pos.z() = static_cast<float>( iy ) * dist ;
        vertex.pos.w() = 1.0f                            ;
        vertex.uv.x() = static_cast<float>( ix ) / static_cast<float>( this->cfg.width  ) ;
        vertex.uv.y() = static_cast<float>( iy ) / static_cast<float>( this->cfg.height ) ;
        vertex.normal = cata::ex::Vec4( 0, -1, 0, 0 ) ;
      }
    }
  }
  
  auto SeaData::triangulate() -> void
  {
    for( int ix = 0; ix < this->cfg.width - 1; ix++ )
    {
      for( int iy = 0; iy < this->cfg.height - 1; iy++ )
      {
        auto index = ix + ( iy * this->cfg.width ) ;
        {
          this->h_indices.push_back( index + 1               ) ;
          this->h_indices.push_back( index + this->cfg.width ) ;
          this->h_indices.push_back( index                   ) ;
          
          this->h_indices.push_back( index + 1                   ) ;
          this->h_indices.push_back( index + this->cfg.width     ) ;
          this->h_indices.push_back( index + this->cfg.width + 1 ) ;
        }
      }
    }
  }

  Sea::Sea( cata::Commands<API>& cmds, const SeaConfig& config )
  {
    this->ocean_data = new SeaData( cmds, config ) ;
  }
  
  Sea::Sea( cata::Commands<API>& cmds )
  {
    auto config = SeaConfig() ;
    this->ocean_data = new SeaData( cmds, config ) ;
  }

  Sea::~Sea()
  {
    delete this->ocean_data ;
  }
  
  auto Sea::data() -> SeaData&
  {
    return *this->ocean_data ;
  }

  auto Sea::updateConfig( const ph::SeaConfig& config ) -> void
  {
   //TODO
  }

  auto Sea::pass() -> const cata::RenderPass<cata::ivk::Vulkan, cata::DefaultAllocator<cata::ivk::Vulkan>>&
  {
    return data().pass ;
  }
  
  auto Sea::commands() -> cata::Commands<cata::ivk::Vulkan>&
  {
    return *data().cmd ;
  }

  auto Sea::update() -> void
  {
    auto& cmds = *data().cmd ;
    static ViewProj vp ;
    
    vp.vp  = g_data().camera->proj() * g_data().camera->view() ;
    vp.pos = g_data().camera->position()                       ;
    
    data().cfg.time = static_cast<float>( data().timer.time() / 1000.f ) ;
    
    cmds.copy( &vp        , data().viewproj ) ;
    cmds.copy( &data().cfg, data().d_cfg    ) ;
  }
  
  auto Sea::draw() -> void
  {
    auto& cmds = *data().cmd ;
    
//    this->update() ;
    auto dims = ex::Vec2( data().cfg.width / 32, data().cfg.height / 32 ) ;
    
    cmds.dispatch( data().proc_pipe, dims.x(), dims.y() ) ;
    
    data().fft->inverse( data().time_buffer ) ;
    
    cmds.dispatch( data().sign_correct, dims.x(), dims.y() ) ;
    cmds.dispatch( data().slope_calc  , dims.x(), dims.y() ) ;
    
    cmds.copy( data().v_offsets, data().heightmap ) ;
    cmds.copy( data().slopes   , data().normalmap ) ;
    
    cmds.attach( this->pass() ) ;
    cmds.drawInstanced( data().draw_pipe, 50, data().indices, data().vertices ) ;
    cmds.detach() ;
  }
}

