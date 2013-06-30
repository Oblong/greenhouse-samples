#version 120

uniform float fog_radius;
uniform float system_distance;
uniform vec4 camera_position;
uniform float feld_size;
attribute float pointsize;

const float sharp = 2; // how sharp our fog dropoff is....

void main ()
{ gl_Position = ftransform ();
  gl_FrontColor = gl_Color;

  // Notes: point size values above 8 are catastrophically SLOW
  // on Mac laptop chipsets we have tested.
  // Cuts fps right in half on Macbook 2012; on Macbook 2011
  // cuts it by factor of 7 (by half at pointsize=4).
  // pointsize 4 ensures OK performance across all common Macbook Pros.
  // gl_PointSize = min (4, pointsize * (460000 / feld_size) / gl_Position.z);

  // for a faster computer, a max of 160 will give you a more dynamic data vis
  gl_PointSize = min (160, (pointsize * 460000 / feld_size ) / gl_Position.z);
                           //radius of point * constant         /  dist from cam

  float individualPointDistToCamera = length (gl_ModelViewMatrix * gl_Vertex);
  float ratio = (individualPointDistToCamera - (system_distance - fog_radius))
                / (2 * fog_radius);
  ratio = clamp (ratio, 0, 1);
  gl_FrontColor.a = gl_FrontColor.a * exp (-sharp * ratio);
}
