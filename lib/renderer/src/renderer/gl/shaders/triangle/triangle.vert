#version 450 core

#include "../global_uniforms.glsl"

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

	outData.viewPosition	= vec3( uniforms.matrixView * uniforms.matrixModel * vec4( inVertexPosition, 1.f ) );
	outData.normal			= vec3( uniforms.matrixNormal * vec4( inVertexNormal, 1.f ) );
	outData.color			= vec3( inVertexColor );
	outData.selected		= inVertexSelected;
	outData.id				= inVertexId;

	gl_Position =  uniforms.matrixProjection * vec4( outData.viewPosition, 1.f );
}