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
  vec2 out_time[] ;
};

layout( binding = 2 ) uniform config
{
  SeaConfig cfg ;
};

// complex math functions
vec2 conjugate(vec2 arg)
{
    return vec2(arg.x, -arg.y);
}

vec2 complex_exp(float arg)
{
    return vec2( cos( arg ), sin( arg ) ) ;
}

vec2 complex_add(vec2 a, vec2 b)
{
    return vec2( a.x + b.x, a.y + b.y ) ;
}

vec2 complex_mult(vec2 ab, vec2 cd)
{
    return vec2(ab.x * cd.x - ab.y * cd.y, ab.x * cd.y + ab.y * cd.x);
}

void main()
{
  uint in_width = cfg.width + 4 ;
  
  uint x = gl_GlobalInvocationID.x ;
  uint y = gl_GlobalInvocationID.y ; 
  uint in_index = y*in_width+x;
  uint in_mindex = ( cfg.height - y ) * in_width + ( cfg.width - x ) ; // mirrored
  uint out_index = y * cfg.width + x ;

  // calculate wave vector
  vec2 k;
  k.x = (-int( cfg.width ) / 2.0f + x) * (2.0f * PI / cfg.patch_size ) ;
  k.y = (-int( cfg.width ) / 2.0f + y) * (2.0f * PI / cfg.patch_size ) ;

  // calculate dispersion w(k)
  float k_len = sqrt(k.x*k.x + k.y*k.y);
  float w = sqrt(9.81f * k_len);

  if ((x < cfg.width) && (y < cfg.height))
  {
      vec2 h0_k  = in_fft[in_index];
      vec2 h0_mk = in_fft[in_mindex];

      // output frequency-space complex values
      out_time[ out_index ] = complex_add( complex_mult( h0_k, complex_exp( w * cfg.time ) ), complex_mult( conjugate( h0_mk ), complex_exp( -w * cfg.time ) ) ) ;
      //ht[out_index] = h0_k;
  }
} 
