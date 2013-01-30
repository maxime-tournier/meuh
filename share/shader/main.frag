uniform sampler2D tex;
varying vec4 position;

bool in_light( void );
float diffuse(int);
float specular(int);
float fog();

float cell(float);


void main (void)
{

  vec4 color = gl_Color;
  color.rgb *= texture2D(tex, gl_TexCoord[0].st).rgb;

  float diff = diffuse(0);
  float spec = specular(0);
    
  if(in_light() ) {
    
    color =  gl_LightModel.ambient * color + clamp(diff + spec, 0.0, 1.0) * color;
    gl_FragColor = mix(gl_Fog.color, color, fog());
    
  }else {
    color = gl_LightModel.ambient * color /* + 0.1 *  clamp(diff, 0.0, 1.0) *  color; */
      ;
    gl_FragColor = mix(gl_Fog.color, color, fog());
    
  }

}
