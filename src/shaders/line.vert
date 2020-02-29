#version 450

layout( location = 0 ) in vec3 vertexPosition;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;

void main() { gl_Position = uMVMatrix * vec4( vertexPosition, 1.f ); }
