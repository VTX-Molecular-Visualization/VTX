#version 450

flat in vec3 sphereColor;

out vec4 fragColor;

void main() { fragColor = vec4( sphereColor, 1.f ); }
