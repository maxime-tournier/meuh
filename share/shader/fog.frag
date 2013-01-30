varying vec4 position;

const float LOG2 = 1.442695;

/* returns fog coefficient 0 = total fog, 1 = clear*/
float fog() {
  
  /* fragment position */
  vec3 pos = position.xyz/position.w;
  float dist2 = dot(pos, pos);
  
  return exp2( -gl_Fog.density * gl_Fog.density * 
	       dist2 *  
	       LOG2 );
}
