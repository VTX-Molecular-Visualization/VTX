#version 450

layout( location = 0 ) in vec3 vertexPosition;

uniform mat4 uMVPMatrix;

void main() { gl_Position = uMVPMatrix * vec4( vertexPosition, 1.f ); }
