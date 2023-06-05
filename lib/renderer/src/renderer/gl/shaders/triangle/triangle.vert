#version 450 core

#include "../global_uniforms.glsl"

// In.
layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec4 aVertexColor;
layout( location = 3 ) in uint aVertexVis;
layout( location = 4 ) in uint aVertexSelected;
layout( location = 5 ) in uint aVertexId;

// Out.
out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	if ( aVertexVis == 0 )
	{
		return;
	}

	dataOut.viewPosition	= vec3( uniforms.matrixView * uniforms.matrixModel * vec4( aVertexPosition, 1.f ) );
	dataOut.normal			= vec3( uniforms.matrixNormal * vec4( aVertexNormal, 1.f ) );
	dataOut.color			= vec3( aVertexColor );
	dataOut.selected		= aVertexSelected;
	dataOut.id				= aVertexId;

	gl_Position = uniforms.matrixProjection * vec4( dataOut.viewPosition, 1.f );
}