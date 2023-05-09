#version 450 core

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec4 aVertexColor;
layout( location = 2 ) in float aVertexRad;
layout( location = 3 ) in uint aVertexVis;
layout( location = 4 ) in uint aVertexSel;
layout( location = 5 ) in uint aVertexId;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;

out
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	dataOut.vertexColor	 = aVertexColor;
	dataOut.vertexVisible	 = aVertexVis;
	dataOut.vertexSelected = aVertexSel;
	dataOut.vertexId		 = aVertexId;

	// Vertex position in view space.
	gl_Position = u_MVMatrix * vec4( aVertexPosition, 1.f );
}
