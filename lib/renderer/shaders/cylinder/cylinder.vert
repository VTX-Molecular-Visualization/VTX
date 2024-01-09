#version 450 core

#include "../constant.glsl"
#include "../layout_uniforms_camera.glsl"

// In.
layout( location = 0 ) in vec3  inVertexPosition;
layout( location = 1 ) in vec4  inVertexColor;
layout( location = 2 ) in float inVertexRad;
layout( location = 3 ) in uint  inVertexId;
layout( location = 4 ) in uint  inVertexFlag;

// Out.
out
#include "struct_vertex_shader.glsl"
outData;

void main()
{
	outData.vertexColor	   = inVertexColor;
	outData.vertexVisible  = bool( inVertexFlag & ( 1 << FLAG_VISIBILITY ) );
	outData.vertexSelected = bool( inVertexFlag & ( 1 << FLAG_SELECTION ) );
	outData.vertexId	   = inVertexId;

	// Vertex position in view space.
	gl_Position = uniformsCamera.matrixView *  uniformsCamera.matrixModel * vec4( inVertexPosition, 1.f );
}
