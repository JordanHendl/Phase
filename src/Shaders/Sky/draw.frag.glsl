#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec3 tex_coords ;
layout ( location = 0 ) out vec4 frag_color ;

layout( binding = 1 ) uniform samplerCube skybox ;

void main()
{
  frag_color = texture( skybox, tex_coords ) ;
}
