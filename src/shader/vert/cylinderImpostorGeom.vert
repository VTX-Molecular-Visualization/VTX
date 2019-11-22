#version 450

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;

flat out vec3 vVertexColor;

void main()
{
	vVertexColor = aVertexColor;

	gl_Position = uMVMatrix * vec4(aVertexPosition, 1.f);
}
