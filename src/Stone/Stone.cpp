#include "Stone.h"
#include "Global/Global/Global.h"
#include "Structures/Structures.h"
#include <stone_draw.h>
#include <Catalyst/structures/Structures.h>
#include <CatalystEX/IO/Image.h>
#include <fstream>
#include <iostream>

namespace ph
{
  using namespace cata ;
  
  struct SViewProj
  {
    cata::ex::Mat4 vp  ;
    cata::ex::Vec3 pos ;
  };
  
  struct StoneVertex
  {
    ex::Vec4 position ;
    ex::Vec2 uv       ;
  };
  
  struct StoneTextures
  {
    Texture<API> base   ;
    Texture<API> height ;
    Texture<API> normal ;
  };
  
  struct StoneData
  {
    Commands<API>*                 cmds        ;
    RenderPass<API>                pass        ;
    Pipeline  <API>                draw        ;
    Pipeline  <API>                normal_calc ;
    UniformArray<API, StoneConfig> d_cfg       ;
    VertexArray<API, StoneVertex>  d_vertices  ;
    IndexArray<API, unsigned>      d_indices   ;
    UniformArray<API, SViewProj>   d_viewproj  ;
    SViewProj                      h_vp        ;
    StoneConfig                    h_cfg       ;
    StoneTextures                  textures    ;

    std::vector<unsigned   > h_indices  ;
    std::vector<StoneVertex> h_vertices ;

    StoneData( cata::Commands<API>& cmds, const StoneConfig& config ) ;
    
    auto generateVertices() -> void ;
    
    auto triangulate() -> void ;
  };
  
  auto StoneData::generateVertices() -> void
  {
    const float dist = 10.0f ;
    
    this->h_vertices.resize( this->h_cfg.width * this->h_cfg.height ) ;
    
    for( int iy = 0; iy < this->h_cfg.height; iy++ )
    {
      for( int ix = 0; ix < this->h_cfg.width; ix++ )
      {
        auto index = ix + ( iy * this->h_cfg.width ) ;
        
        auto& vertex = this->h_vertices[ index ] ;
        vertex.position.x() = static_cast<float>( ix ) * dist ;
        vertex.position.y() = 0.0f                            ;
        vertex.position.z() = static_cast<float>( iy ) * dist ;
        vertex.position.w() = 1.0f                            ;
        vertex.uv.x() = static_cast<float>( ix ) / static_cast<float>( this->h_cfg.width  ) ;
        vertex.uv.y() = static_cast<float>( iy ) / static_cast<float>( this->h_cfg.height ) ;
      }
    }
  }

  auto StoneData::triangulate() -> void
  {
    for( int ix = 0; ix < this->h_cfg.width - 1; ix++ )
    {
      for( int iy = 0; iy < this->h_cfg.height - 1; iy++ )
      {
        auto index = ix + ( iy * this->h_cfg.width ) ;
        {
          this->h_indices.push_back( index + 1                 ) ;
          this->h_indices.push_back( index + this->h_cfg.width ) ;
          this->h_indices.push_back( index                     ) ;
          
          this->h_indices.push_back( index + 1                     ) ;
          this->h_indices.push_back( index + this->h_cfg.width     ) ;
          this->h_indices.push_back( index + this->h_cfg.width + 1 ) ;
        }
      }
    }
  }
    
  StoneData::StoneData( cata::Commands<API>& cmds, const StoneConfig& config )
  {
    auto start_location = g_data().stone_base_path + std::string( "/x0_y0.png" ) ;
    
    auto loaded = cata::ex::loadR32F( start_location.c_str() ) ;
    
    auto rp_info    = RenderPassInfo() ;
    auto subpass    = Subpass()        ;
    auto attachment = Attachment()     ;
    auto viewport   = Viewport()       ;

    this->cmds  = &cmds  ;
    this->h_cfg = config ;

    viewport.setWidth ( 1280 ) ;
    viewport.setHeight( 1024 ) ;
    
    attachment.setClearColor( 0.1, 0.1, 0.0, 1.0 ) ;
    attachment.setFormat( Format::RGBA8 ) ;
    
    subpass.addAttachment( attachment ) ;
    subpass.setDepthStencilEnable( true ) ;
    
    rp_info.addSubpass( subpass ) ;
    
    this->pass = RenderPass<API>( 0, rp_info ) ;
    auto pipe_info = PipelineInfo() ;
    pipe_info.setFileBytes( bytes::stone_draw, sizeof( bytes::stone_draw ) ) ;
    pipe_info.setDepthTest( true ) ;
    pipe_info.addViewport( viewport ) ;
    
    this->draw  = Pipeline<API>( this->pass, pipe_info ) ;
    
    auto tex_info = TextureInfo() ;
    tex_info.setDimensions( loaded.width, loaded.height ) ;
    tex_info.setFormat( Format::R32F ) ;
    
    this->textures.height = Texture<API>( 0, tex_info ) ;
    
    this->generateVertices() ;
    this->triangulate     () ;
    
    auto wb      = Array<API, float      >( 0, loaded.width * loaded.height * loaded.channels, true ) ;
    auto wb2     = Array<API, StoneVertex>( 0, this->h_vertices.size()                       , true ) ;
    auto wb3     = Array<API, unsigned   >( 0, this->h_indices .size()                       , true ) ;
    
    this->d_vertices = VertexArray <API, StoneVertex>( 0, this->h_vertices.size() ) ;
    this->d_indices  = IndexArray  <API, unsigned   >( 0, this->h_indices.size()  ) ;
    this->d_cfg      = UniformArray<API, StoneConfig>( 0, 1, true                 ) ;
    this->d_viewproj = UniformArray<API, SViewProj  >( 0, 1, true                 ) ;

    auto oneshot = Commands<API>( 0, SubmitType::Transfer ) ;
    
    oneshot.copy( loaded.pixels   .data(), wb  ) ;
    oneshot.copy( this->h_vertices.data(), wb2 ) ;
    oneshot.copy( this->h_indices .data(), wb3 ) ;
    
    oneshot.copy( wb , this->textures.height ) ;
    oneshot.copy( wb2, this->d_vertices      ) ;
    oneshot.copy( wb3, this->d_indices       ) ;

    oneshot.submit     () ;
    oneshot.synchronize() ;
    
    this->draw.bind( "heightmap", this->textures.height ) ;
    this->draw.bind( "viewproj" , this->d_viewproj      ) ;
  }
  
  Stone::Stone( cata::Commands<API>& cmds, const StoneConfig& config )
  {
    this->data = new StoneData( cmds, config ) ;
  }

  Stone::Stone( cata::Commands<API>& cmds )
  { 
    auto config = StoneConfig() ;
    this->data = new StoneData( cmds, config ) ;
  }

  Stone::~Stone()
  {  
    delete this->data ;
  }

  auto Stone::updateConfig( const ph::StoneConfig& config ) -> void
  {  
  }

  auto Stone::pass() -> const cata::RenderPass<API, cata::DefaultAllocator<API>>&
  {  
    return data->pass ;
  }

  auto Stone::update() -> void
  {  
    data->h_vp.vp  = g_data().camera->proj() * g_data().camera->view() ;
    data->h_vp.pos = g_data().camera->position() ;
    
    data->cmds->copy( &data->h_vp , data->d_viewproj ) ;
    data->cmds->copy( &data->h_cfg, data->d_cfg      ) ;
  }

  auto Stone::draw() -> void
  {  
    this->update() ;
    
    data->cmds->attach( data->pass ) ;
    data->cmds->draw( data->draw, data->d_indices, data->d_vertices ) ;
    data->cmds->detach() ;
  }
}
