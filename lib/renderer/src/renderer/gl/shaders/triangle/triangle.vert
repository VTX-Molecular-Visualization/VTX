#version 450 core

layout( location = 0 ) in vec4 aVertexPosition;
layout( location = 1 ) in vec4 aVertexNormal;
layout( location = 2 ) in vec3 aVertexColor;
layout( location = 3 ) in uint aVertexVis;
layout( location = 4 ) in uint aVertexSelected;
layout( location = 5 ) in uint aVertexId;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;

out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	if ( aVertexVis == 0 )
	{
		return;
	}

	dataOut.viewPosition = vec3( u_MVMatrix * aVertexPosition );
	dataOut.normal	   = vec3( u_normalMatrix * aVertexNormal );
	dataOut.color		   = aVertexColor;
	dataOut.selected	   = aVertexSelected;
	dataOut.id		   = aVertexId;

	gl_Position = u_projMatrix * vec4( dataOut.viewPosition, 1.f );
}