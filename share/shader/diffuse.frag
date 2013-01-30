varying vec3 normal;
varying vec4 position;

float diffuse(int light_id) {
  vec3 pos = position.xyz/position.w;
  vec3 light_pos = gl_LightSource[light_id].position.xyz / gl_LightSource[light_id].position.w;
  
  vec3 dir = pos - light_pos;
  
  float att = 1.0 / ( 1.0 + 0.005 * sqrt( dot(dir, dir) ) );
  vec3 light = normalize( dir.xyz );
  vec3 n = normalize(normal);
  
  return att * max( 0.0, dot(n, -light) );
}
