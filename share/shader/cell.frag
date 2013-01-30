

float cell(float intensity) {
  if(intensity > 0.9) return 1.0;
  if(intensity > 0.6) return 0.6;
  if(intensity > 0.3) return 0.3;
  return 0.0;
}


// vec3 cell(vec3 color) {
//   float intensity = sqrt( dot(color, color) );
  
//   if(n >= 1.0) n = 1.0;
//   else if(n > 0.7) n = 0.7;
//   else if(n > 0.4) n = 0.4;
//   else if(n > 0.1) n = 0.1;
//   else n = 0.0;

//   return (n * sqrt(3.0) * color);
// }

