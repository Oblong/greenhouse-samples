#version 120

uniform float fog_radius;
uniform float system_distance;
uniform vec4 camera_position;
attribute float pointsize;

const float sharp = 2.; // how sharp our fog dropoff is....

void main ()
{ gl_Position = ftransform();
  gl_FrontColor = gl_Color;

  float individualPointDistToCamera = length (gl_ModelViewMatrix * gl_Vertex);
  float ratio = (individualPointDistToCamera - (system_distance - fog_radius))
                / (2 * fog_radius);
  ratio = clamp (ratio, 0, 1);
  gl_FrontColor.a = gl_FrontColor.a * exp (-sharp * ratio);
  gl_PointSize = pointsize;
}
