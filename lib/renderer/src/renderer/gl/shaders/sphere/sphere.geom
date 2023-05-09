#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4 u_projMatrix;

in 
#include "struct_vertex_shader.glsl"
dataIn[];

out 
#include "struct_geometry_shader.glsl"
dataOut;

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	dataOut.viewImpPos = v1;
	gl_Position		 = u_projMatrix * vec4( dataOut.viewImpPos, 1.f );
	EmitVertex();

	dataOut.viewImpPos = v2;
	gl_Position		 = u_projMatrix * vec4( dataOut.viewImpPos, 1.f );
	EmitVertex();

	dataOut.viewImpPos = v3;
	gl_Position		 = u_projMatrix * vec4( dataOut.viewImpPos, 1.f );
	EmitVertex();

	dataOut.viewImpPos = v4;
	gl_Position		 = u_projMatrix * vec4( dataOut.viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Do not emit primitive if sphere is not visible.
	if ( dataIn[ 0 ].sphereVisible == 0 )
	{
		return;
	}

	// Output data.
	dataOut.viewSpherePos	   = dataIn[ 0 ].viewSpherePos;
	dataOut.sphereColor	   = dataIn[ 0 ].sphereColor;
	dataOut.sphereRadius	   = dataIn[ 0 ].sphereRadius;
	dataOut.dotViewSpherePos = dataIn[ 0 ].dotViewSpherePos;
	dataOut.sphereSelected   = dataIn[ 0 ].sphereSelected;
	dataOut.sphereId		   = dataIn[ 0 ].sphereId;

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - dataIn[ 0 ].vImpU - dataIn[ 0 ].vImpV;
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + dataIn[ 0 ].vImpU - dataIn[ 0 ].vImpV;
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - dataIn[ 0 ].vImpU + dataIn[ 0 ].vImpV;
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + dataIn[ 0 ].vImpU + dataIn[ 0 ].vImpV;

	emitQuad( v1, v2, v3, v4 );
}
