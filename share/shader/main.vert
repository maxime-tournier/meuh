varying vec4 light_proj;
varying vec3 normal;
varying vec4 position;


void main(void)
{
  normal  = gl_NormalMatrix * gl_Normal;
 

  position = gl_ModelViewMatrix * gl_Vertex;
  
  gl_TexCoord[0] =  gl_TextureMatrix[0] * gl_MultiTexCoord0;
  light_proj =  gl_TextureMatrix[1] * position;
  
  gl_FrontColor = gl_Color;
  gl_Position = ftransform();	/* TODO optimize :) */
    
}
