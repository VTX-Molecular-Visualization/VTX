#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexColor;
layout( location = 3 ) in uint aVertexVis;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;

flat out vec3 vVertexColor;
flat out uint vVertexVis;

void main()
{
	vVertexColor = aVertexColor;
	vVertexVis	 = aVertexVis;

	// Vertex position in view space.
	gl_Position = uMVMatrix * vec4( aVertexPosition, 1.f );
}
