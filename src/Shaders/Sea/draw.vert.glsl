#version 450 core
#extension GL_ARB_separate_shader_objects : enable

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

layout ( location = 0 ) in vec4 vertex ;
layout ( location = 1 ) in vec4 normal ;
layout ( location = 2 ) in vec2 uv     ;

layout ( location = 0 ) out vec3 f_normal  ;
layout ( location = 1 ) out vec2 f_uv      ;
layout ( location = 2 ) out vec3 f_viewpos ;
layout ( location = 3 ) out vec3 f_fragpos ;

layout( binding = 0 ) uniform viewproj
{
  mat4 vp       ;
  vec3 view_pos ;
};

layout( binding = 1 ) uniform sampler2D heightmap ;
layout( binding = 2 ) uniform sampler2D normalmap ;

layout( binding = 3 ) uniform config
{
  SeaConfig cfg ;
};

void main()
{
  float voff  = texture( heightmap, uv ).r  ;
  vec2  slope = texture( normalmap, uv ).xy ;
  
  vec3 n = normalize( cross( vec3( 0.0, slope.y * cfg.h_scale, 2.0 / cfg.width ), vec3( 2.0 / cfg.height, slope.x * cfg.h_scale, 0.0 ) ) ) ;
  f_normal  = normal.xyz ;
  f_uv      = uv         ;
  f_viewpos = view_pos   ;
  
  vec3 tmp = vertex.xyz ;
  tmp.y = voff * cfg.h_scale ;
  
  const int instance_width  = 5 ;
  const int instance_height = 5 ;
  
  tmp.x = tmp.x + ( ( cfg.width  - 1 )  * ( gl_InstanceIndex % instance_width ) ) ;
  tmp.z = tmp.z + ( ( cfg.height - 1 ) * ( gl_InstanceIndex / instance_height ) ); 
  
  f_normal  = n   ;
  f_fragpos = tmp ;
  gl_Position = vp * vec4( tmp, 1.0 ) ;
}
