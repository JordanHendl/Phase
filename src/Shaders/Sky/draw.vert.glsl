#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec4 vertex ;

layout( location = 0 ) out vec3 tex_coords ;

layout( binding = 0 ) uniform viewproj
{
  mat4 vp       ;
  vec3 view_pos ;
};

void main()
{
  tex_coords = vertex.xyz ;
  
  vec4 tmp = vp * vec4( vertex.xyz, 1.0 ) ;
  gl_Position = tmp.xyzw ;
}
