uniform sampler2DShadow light_map;
varying vec4 light_proj;	/* texture coordinates of current fragment in depthmap */


bool in_light(void) {
  vec4 proj = light_proj;
  proj.z += 0.0000001 * proj.w;
  
  return bool(shadow2DProj(light_map, proj).r);
}


