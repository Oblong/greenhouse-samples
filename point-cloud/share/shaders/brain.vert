#version 120

attribute float pointsize;

void main ()
{ gl_Position = ftransform ();
  gl_PointSize = 1.0;
  gl_FrontColor = vec4(gl_Normal.xyz, 0.5);
}
