#version 450

layout(location = 0) in vec2 aVertexPosition;

out vec2 vTexCoords;

void main()
{
	gl_Position = vec4(aVertexPosition, 0.f, 1.f);
}
