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

layout( binding = 0 ) readonly buffer in_signal
{
  vec2 in_fft[] ;
};

layout( binding = 1 ) writeonly buffer out_signal
{
  float heightmap[] ;
};

layout( binding = 2 ) uniform config
{
  SeaConfig cfg ;
};

void main()
{
  uint index = gl_GlobalInvocationID.x + ( gl_GlobalInvocationID.y * cfg.width ) ;
  
  float sign_correction = bool( ( gl_GlobalInvocationID.x + gl_GlobalInvocationID.y ) & 0x01 ) ? -1.0f : 1.0f ;
  heightmap[ index ] = in_fft[ index ].x * sign_correction ;
} 
