#version 450 core
#extension GL_ARB_separate_shader_objects : enable

#define BLOCK_SIZE_X 32 
#define BLOCK_SIZE_Y 32 
#define BLOCK_SIZE_Z 1 
#define PI 3.1415926538

layout( local_size_x = BLOCK_SIZE_X, local_size_y = BLOCK_SIZE_Y, local_size_z = BLOCK_SIZE_Z ) in ; 

struct SeaConfig
{
  // Simulation parameters
  uint  width       ;
  uint  height      ;
  float gravity     ; // gravitational constant
  float wave_scale  ; // wave scale factor
  float patch_size  ; // patch size
  float wind_speed  ;
  float wind_dir    ;
  float dir_depend  ;
  float h_scale     ;
  float time        ;
};

layout( binding = 0 ) readonly buffer in_buffer
{
  float height[] ;
};

layout( binding = 1 ) writeonly buffer out_buffer
{
  vec2 slopes[] ;
};

layout( binding = 2 ) uniform config
{
  SeaConfig cfg ;
};

void main()
{
  uint x = gl_GlobalInvocationID.x ;
  uint y = gl_GlobalInvocationID.y ;
  uint z = gl_GlobalInvocationID.z ;
  
  uint index = x + ( y * cfg.width ) ;
  
  vec2 slope = vec2( 0.0, 0.0 ) ;
  
  if( ( x > 0 ) && ( y > 0 ) && ( x < cfg.width - 1 ) && ( y < cfg.height - 1 ) )
  {
    slope.x = height[ index + 1         ] - height[ index - 1         ] ;
    slope.y = height[ index + cfg.width ] - height[ index - cfg.width ] ;
  }
  
  slopes[ index ] = slope ;
} 
