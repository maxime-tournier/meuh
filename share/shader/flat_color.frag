uniform vec3 color;
varying vec4 position;

float fog();

void main( void ) {

  gl_FragColor.rgb = mix(gl_Fog.color.rgb, color, fog());
  /* gl_FragColor.a = 1.0; */

  /* gl_FragColor.rgb = color; */
}
