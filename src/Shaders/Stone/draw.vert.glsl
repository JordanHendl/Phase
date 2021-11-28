#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec4 vertex ;
layout ( location = 1 ) in vec2 in_uvs ;

layout( location = 0 ) out vec2 tex_coords ;


layout( binding = 0 ) uniform sampler2D heightmap ;

layout( binding = 1 ) uniform viewproj
{
  mat4 vp       ;
  vec3 view_pos ;
}; 

void main()
{
  const float height_scale = 500.0 ;
  tex_coords = in_uvs ;
  vec4 height = texture( heightmap, in_uvs ) ;
  
  vec4 v = vertex ;
  v.y = height.r * height_scale ;
  gl_Position = vp * v ;
}
