#version 450 core

#include "../layout_uniforms_camera.glsl"

// In.
layout( location = 0 ) in vec3  inVertexPosition;
layout( location = 1 ) in vec4  inVertexColor;
layout( location = 2 ) in float inVertexRad;
layout( location = 3 ) in uint  inVertexVis;
layout( location = 4 ) in uint  inVertexSel;
layout( location = 5 ) in uint  inVertexId;

// Out.
out
#include "struct_vertex_shader.glsl"
outData;

void main()
{
	outData.vertexColor	   = inVertexColor;
	outData.vertexVisible  = inVertexVis;
	outData.vertexSelected = inVertexSel;
	outData.vertexId	   = inVertexId;

	// Vertex position in view space.
	gl_Position = uniformsCamera.matrixView *  uniformsCamera.matrixModel * vec4( inVertexPosition, 1.f );
}
