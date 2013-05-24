#version 120

void main ()
{ gl_Position = ftransform ();
  gl_PointSize = 4.0;
  gl_FrontColor = vec4(gl_Normal.xyz, 0.5);
}
