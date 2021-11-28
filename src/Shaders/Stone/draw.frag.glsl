#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec2 tex_coords ;
layout ( location = 0 ) out vec4 frag_color ;

layout( binding = 0 ) uniform sampler2D heightmap ;

void main()
{
  vec4 s = texture( heightmap, tex_coords ) ;
  frag_color = vec4( s.xyz, 1 ) ;
}
