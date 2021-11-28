/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Sky.cpp
 * Author: jhendl
 * 
 * Created on November 26, 2021, 3:22 AM
 */

#include "Sky.h"
#include "Global/Global/Global.h"
#include "Structures/Structures.h"
#include <sky_draw.h>
#include <Impulse/structures/Structures.h>
#include <EXImpulse/IO/Image.h>

namespace ph
{
  using namespace imp     ;
  using namespace imp::ex ;
  
Vec4 skybox_vertices[] = 
{
    // positions          
    Vec4( -1.0f,  1.0f, -1.0f, 1.0f ),
    Vec4( -1.0f, -1.0f, -1.0f, 1.0f ),
    Vec4(  1.0f, -1.0f, -1.0f, 1.0f ),
    Vec4(  1.0f, -1.0f, -1.0f, 1.0f ),
    Vec4(  1.0f,  1.0f, -1.0f, 1.0f ),
    Vec4( -1.0f,  1.0f, -1.0f, 1.0f ),

    Vec4( -1.0f, -1.0f,  1.0f, 1.0f ) ,
    Vec4( -1.0f, -1.0f, -1.0f, 1.0f ) ,
    Vec4( -1.0f,  1.0f, -1.0f, 1.0f ) ,
    Vec4( -1.0f,  1.0f, -1.0f, 1.0f ) ,
    Vec4( -1.0f,  1.0f,  1.0f, 1.0f ) ,
    Vec4( -1.0f, -1.0f,  1.0f, 1.0f ) ,

     Vec4( 1.0f, -1.0f, -1.0f, 1.0f ),
     Vec4( 1.0f, -1.0f,  1.0f, 1.0f ),
     Vec4( 1.0f,  1.0f,  1.0f, 1.0f ),
     Vec4( 1.0f,  1.0f,  1.0f, 1.0f ),
     Vec4( 1.0f,  1.0f, -1.0f, 1.0f ),
     Vec4( 1.0f, -1.0f, -1.0f, 1.0f ),

    Vec4( -1.0f, -1.0f,  1.0f, 1.0f ),
    Vec4( -1.0f,  1.0f,  1.0f, 1.0f ),
    Vec4(  1.0f,  1.0f,  1.0f, 1.0f ),
    Vec4(  1.0f,  1.0f,  1.0f, 1.0f ),
    Vec4(  1.0f, -1.0f,  1.0f, 1.0f ),
    Vec4( -1.0f, -1.0f,  1.0f, 1.0f ),

    Vec4( -1.0f,  1.0f, -1.0f, 1.0f ),
    Vec4(  1.0f,  1.0f, -1.0f, 1.0f ),
    Vec4(  1.0f,  1.0f,  1.0f, 1.0f ),
    Vec4(  1.0f,  1.0f,  1.0f, 1.0f ),
    Vec4( -1.0f,  1.0f,  1.0f, 1.0f ),
    Vec4( -1.0f,  1.0f, -1.0f, 1.0f ),

    Vec4( -1.0f, -1.0f, -1.0f, 1.0f ),
    Vec4( -1.0f, -1.0f,  1.0f, 1.0f ),
    Vec4(  1.0f, -1.0f, -1.0f, 1.0f ),
    Vec4(  1.0f, -1.0f, -1.0f, 1.0f ),
    Vec4( -1.0f, -1.0f,  1.0f, 1.0f ),
    Vec4(  1.0f, -1.0f,  1.0f, 1.0f )
};

  struct SkyView
  {
    Mat4 vp  ;
    Vec3 pos ;
  };
  
  struct SkyData
  {
    using SkyBoxVertices = VertexArray<API, Vec4> ;
    
    imp::RenderPass<API>            pass            ;
    imp::Pipeline<API>              skybox_pipeline ;
    imp::Texture<API>               skybox          ;
    imp::UniformArray<API, SkyView> viewproj        ;
    imp::Commands<API>*             cmds            ;
    SkyBoxVertices                  vertices        ;
    SkyConfig                       cfg             ;
    
    SkyData( imp::Commands<API>& cmds, const SkyConfig& config ) ;
  };
  
  SkyData::SkyData( imp::Commands<API>& cmds, const SkyConfig& config )
  {
    auto pipe_info  = PipelineInfo()   ;
    auto rp_info    = RenderPassInfo() ;
    auto subpass    = Subpass()        ;
    auto attachment = Attachment()     ;
    auto tex_info   = TextureInfo()    ;
    auto viewport   = Viewport()       ;
    
    this->cmds = &cmds ;

    viewport.setWidth ( 1280 ) ;
    viewport.setHeight( 1024 ) ;
    
    tex_info.setDimensions( 1280, 1280 ) ;
    tex_info.setFormat( Format::RGBA8 ) ;
    tex_info.setIsCubeMap( true ) ;
    
    pipe_info.setFileBytes( bytes::sky_draw, sizeof( bytes::sky_draw ) ) ;
    pipe_info.setDepthTest( true ) ;
    pipe_info.addViewport( viewport ) ;

    attachment.setClearColor( 0, 0, 0, 0    ) ;
    attachment.setFormat    ( Format::RGBA8 ) ;
    
    subpass.addAttachment( attachment ) ;
    subpass.setDepthStencilEnable( true ) ;
    
    rp_info.addSubpass( subpass ) ;
    
    this->pass            = RenderPass <API          >( 0, rp_info            ) ;
    this->skybox_pipeline = Pipeline   <API          >( this->pass, pipe_info ) ;
    this->vertices        = VertexArray<API, Vec4    >( 0, 36, true           ) ;
    this->viewproj        = UniformArray<API, SkyView>( 0, 1, true            ) ;
    
    auto back   = imp::ex::loadRGBA8( g_data().skybox_images[ 0 ].c_str() ) ;
    tex_info.setDimensions( back.width, back.height ) ;
    this->skybox = Texture<API>( 0, tex_info ) ;
    
    
    auto size = back.width * back.height * back.channels ;
    
    auto wb = Array<API, unsigned char>( 0, size, true ) ;
    
    auto tmp_cmds = Commands<API>( 0, SubmitType::Compute ) ;
    
    for( unsigned index = 0; index < 6; index++ )
    {
      auto layer      = this->skybox.layer( index ) ;
      auto loaded_img = imp::ex::loadRGBA8( g_data().skybox_images[ index ].c_str() ) ;
      tmp_cmds.copy( loaded_img.pixels.data(), wb ) ;
      tmp_cmds.copy( wb, layer ) ;
      tmp_cmds.submit() ;
      tmp_cmds.synchronize() ;
    }
    
    tmp_cmds.copy( skybox_vertices, this->vertices ) ;
    tmp_cmds.submit() ;
    tmp_cmds.synchronize() ;

    this->skybox_pipeline.bind( "skybox"  , this->skybox   ) ;
    this->skybox_pipeline.bind( "viewproj", this->viewproj ) ;
  }
  
  Sky::Sky( imp::Commands<API>& cmds, const SkyConfig& config )
  {
    this->data = new SkyData( cmds, config ) ;
  }

  Sky::Sky( imp::Commands<API>& cmds )
  { 
    auto config = SkyConfig() ;
    this->data = new SkyData( cmds, config ) ;
  }

  Sky::~Sky()
  {  
    delete this->data ;
  }

  auto Sky::updateConfig( const ph::SkyConfig& config ) -> void
  {  
    data->cfg = config ;
  }

  auto Sky::pass() -> const imp::RenderPass<API, imp::DefaultAllocator<API>>&
  {  
    return data->pass ;
  }

  auto Sky::update() -> void
  { 
    static SkyView view ;
    
    auto camera = g_data().camera ;
    auto mat = camera->view() ;
    
    // Remove position component.
    mat[ 3 ][ 0 ] = 0.0f ;
    mat[ 3 ][ 1 ] = 0.0f ;
    mat[ 3 ][ 2 ] = 0.0f ;
    mat[ 0 ][ 3 ] = 0.0f ;
    mat[ 1 ][ 3 ] = 0.0f ;
    mat[ 2 ][ 3 ] = 0.0f ;
    
    view.vp  = camera->proj() * mat ;
    view.pos = camera->position()   ;
    
    data->cmds->copy( &view, data->viewproj ) ;
  }

  auto Sky::draw() -> void
  { 
    this->update() ;
    
    data->cmds->attach( data->pass ) ;
    data->cmds->draw( data->skybox_pipeline, data->vertices ) ;
    data->cmds->detach() ;
  }
}
