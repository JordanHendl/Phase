#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec3 f_normal  ;
layout ( location = 1 ) in vec2 f_uv      ;
layout ( location = 2 ) in vec3 f_viewpos ;
layout ( location = 3 ) in vec3 f_fragpos ;

layout( location = 0 ) out vec4 out_color ;

void main()
{
  const vec3 color     = vec3( 0.0, 0.3, 0.5 ) ;
  const vec3 light_dir = vec3( 0, 0.2, 0.8   ) ;
  const vec3 light_pos = vec3( 0, -10, 0     ) ;
  const vec3 fragpos   = f_fragpos.xyz         ;

  float ambient_strength = 0.5 ;
  vec3 ambient = ambient_strength * color ;
  
  vec3 lightdir = light_dir ;
  float diff = abs( dot( f_normal.xyz, lightdir ) ) ;
  vec3 diffuse = diff * color ;
  diffuse *= 0.1 ;
  float spec_strength = 0.5 ;
  vec3 viewdir = normalize( f_viewpos - f_fragpos.xyz ) ;
  vec3 reflect = reflect  ( -lightdir, f_normal.xyz   ) ;
  float spec = pow( abs( max( dot( viewdir, -lightdir ), 0.0 ) ), 64 ) ;
  vec3 specular = spec * vec3( 1.0, 1.0, 1.0 ) ;
  
  
  vec3 result = ( ambient + diffuse + specular ) ;
//  float intensity = dot( lightdir, f_normal ) ;
//       if( intensity > 0.95 ) result *= 1.0 ;
//  else if( intensity > 0.85 ) result *= 0.8 ;
//  else if( intensity > 0.55 ) result *= 0.6 ;
//  else if( intensity > 0.25 ) result *= 0.4 ;
//  else                        result *= 0.2 ;
  
  out_color = vec4( result, 1.0 ) ;
}
