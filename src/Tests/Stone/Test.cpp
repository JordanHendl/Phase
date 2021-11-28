#include <Phase.h>

#include <EXImpulse/Math/Math.h>
#include <EXImpulse/Camera/Camera.h>
#include "Structures/Structures.h"
#include <Impulse/vk/Vulkan.h>
#include <Impulse/structures/Structures.h>
#include <Impulse/window/Window.h>
#include <Impulse/window/Event.h>

using namespace ph  ;
using namespace imp ;
using API = ivk::Vulkan ;

static bool running = true ;

static void eventCallback( const imp::Event& event )
{
  if( event.type() == imp::Event::Type::WindowExit ) running = false ;
  if( event.key() == imp::Key::ESC ) running = false ;
}

int main()
{
  // Initialize Impulse stuff
  API::addValidationLayer( "VK_LAYER_KHRONOS_validation"         ) ;
  API::addValidationLayer( "VK_LAYER_LUNARG_standard_validation" ) ;
  API::initialize() ;
  
  auto window    = os::Window( "Phase- Sea", 1280, 1024 ) ;
  auto swapchain = Swapchain <API>( 0, window.handle()      ) ;
  auto cmd       = Commands  <API>( 0, SubmitType::Graphics ) ;
  
  window.setCaptureMouse( true ) ;
  
  // Register events
  imp::EventManager manager ;
  manager.enroll( &eventCallback, "Quit Callback" ) ;
  
  auto graphics_camera = imp::ex::Camera( 1.0 ) ;
  auto sea_camera      = ph::Camera()      ;
  
  Phase::initalize( "config_path.json", sea_camera ) ;  
  auto proj = imp::ex::perspective( imp::ex::radians( 90.0f ), ( 1280.f / 1024.f ), 0.1f, 10000.f ) ;
  auto view = imp::ex::Mat4( 1.0f ) ;
  
  sea_camera.setProjection( proj ) ;
  sea_camera.setView      ( view ) ;
  
  auto sea_config = ph::StoneConfig() ;
  auto sea = ph::Stone( cmd, sea_config ) ;
  
  swapchain.wait( cmd ) ;
  
  cmd.begin() ;
  sea.draw() ;
  cmd.blit( sea.pass(), swapchain, imp::Filter::Linear ) ;
  
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
      cmd.blit( sea.pass(), swapchain, imp::Filter::Linear ) ;
    }
    
    window.poll() ;
  }
  
  cmd.synchronize() ;
}
