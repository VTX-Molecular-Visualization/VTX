#version 450 core

#include "../layout_uniforms_camera.glsl"

// In.
layout( location = 0 ) in vec3 inVertexPosition;
layout( location = 1 ) in vec3 inVertexNormal;
layout( location = 2 ) in vec4 inVertexColor;
layout( location = 3 ) in uint inVertexVis;
layout( location = 4 ) in uint inVertexSelected;
layout( location = 5 ) in uint inVertexId;

// Out.
out 
#include "struct_vertex_shader.glsl"
outData;

void main()
{
	if ( inVertexVis == 0 )
	{
		return;
	}

	outData.viewPosition	= vec3( uniformsCamera.matrixView * uniformsCamera.matrixModel * vec4( inVertexPosition, 1.f ) );
	outData.normal			= vec3( uniformsCamera.matrixNormal * vec4( inVertexNormal, 1.f ) );
	outData.color			= inVertexColor;
	outData.selected		= inVertexSelected;
	outData.id				= inVertexId;

	gl_Position =  uniformsCamera.matrixProjection * vec4( outData.viewPosition, 1.f );
}