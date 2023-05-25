#version 450 core

#include "../global_uniforms.glsl"

layout( location = 0 ) in vec4 aVertexPosition;
layout( location = 1 ) in vec4 aVertexNormal;
layout( location = 2 ) in vec4 aVertexColor;
layout( location = 3 ) in uint aVertexVis;
layout( location = 4 ) in uint aVertexSelected;
layout( location = 5 ) in uint aVertexId;

out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	if ( aVertexVis == 0 )
	{
		//return;
	}

	//dataOut.viewPosition	= vec3( uniforms.matrixView * aVertexPosition );
	//dataOut.normal			= vec3( uniforms.matrixView * aVertexNormal );
	dataOut.color			= vec3( aVertexColor );
	//dataOut.selected		= aVertexSelected;
	//dataOut.id				= aVertexId;

	gl_Position = uniforms.matrixProjection * uniforms.matrixView * aVertexPosition;
}