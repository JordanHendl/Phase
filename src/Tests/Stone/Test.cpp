#include <Phase.h>

#include <CatalystEX/Math/Math.h>
#include <CatalystEX/Camera/Camera.h>
#include "Structures/Structures.h"
#include <Catalyst/vk/Vulkan.h>
#include <Catalyst/structures/Structures.h>
#include <Catalyst/window/Window.h>
#include <Catalyst/window/Event.h>

using namespace ph  ;
using namespace cata ;
using API = ivk::Vulkan ;

static bool running = true ;

static void eventCallback( const cata::Event& event )
{
  if( event.type() == cata::Event::Type::WindowExit ) running = false ;
  if( event.key() == cata::Key::ESC ) running = false ;
}

int main()
{
  // Initialize Catalyst stuff
  API::addValidationLayer( "VK_LAYER_KHRONOS_validation"         ) ;
  API::addValidationLayer( "VK_LAYER_LUNARG_standard_validation" ) ;
  API::initialize() ;
  
  auto window    = os::Window( "Phase- Sea", 1280, 1024 ) ;
  auto swapchain = Swapchain <API>( 0, window.handle()      ) ;
  auto cmd       = Commands  <API>( 0, SubmitType::Graphics ) ;
  
  window.setCaptureMouse( true ) ;
  
  // Register events
  cata::EventManager manager ;
  manager.enroll( &eventCallback, "Quit Callback" ) ;
  
  auto graphics_camera = cata::ex::Camera( 1.0 ) ;
  auto sea_camera      = ph::Camera()      ;
  
  Phase::initalize( "config_path.json", sea_camera ) ;  
  auto proj = cata::ex::perspective( cata::ex::radians( 90.0f ), ( 1280.f / 1024.f ), 0.1f, 10000.f ) ;
  auto view = cata::ex::Mat4( 1.0f ) ;
  
  sea_camera.setProjection( proj ) ;
  sea_camera.setView      ( view ) ;
  
  auto sea_config = ph::StoneConfig() ;
  auto sea = ph::Stone( cmd, sea_config ) ;
  
  swapchain.wait( cmd ) ;
  
  cmd.begin() ;
  sea.draw() ;
  cmd.blit( sea.pass(), swapchain, cata::Filter::Linear ) ;
  
  while( running )
  {
    graphics_camera.update() ;
    sea_camera.setView    ( graphics_camera.view() ) ;
    sea_camera.setPosition( graphics_camera.pos()  ) ;
    sea.update() ;

    try
    {
      swapchain.present() ;
    }
    catch( LibraryError& e )
    {
      cmd.begin() ;
      sea.draw() ;
      cmd.blit( sea.pass(), swapchain, cata::Filter::Linear ) ;
    }
    
    window.poll() ;
  }
  
  cmd.synchronize() ;
}
