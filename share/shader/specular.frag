varying vec3 normal;
varying vec4 position;

float specular(int light_id)  {
  vec3 pos = position.xyz/position.w;
  vec3 light_pos = 
    gl_LightSource[light_id].position.xyz / 
    gl_LightSource[light_id].position.w;
 
  vec3 dir = pos - light_pos;
  
  vec3 light = normalize(dir);
  vec3 eye = normalize(pos);
  vec3 half = normalize(-eye - light);
  vec3 n = normalize(normal);
  
  float spec = max(0.0, dot(n, half)); 
  float att = 1.0 / (1.0 + 0.001 * sqrt( dot(dir, dir) ) );

  return att * pow(spec, 5.0);
    
}
