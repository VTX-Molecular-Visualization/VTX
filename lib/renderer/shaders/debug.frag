#version 450 core

// In.
layout ( std140, binding = 1 ) uniform Uniforms
{
	vec4 color;
} uniforms;

// Out.
out vec4 outFragColor;

void main()
{
	outFragColor = uniforms.color;
}