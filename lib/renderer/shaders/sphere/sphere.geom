#version 450 core

#include "../layout_uniforms_camera.glsl"

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

// In.
in 
#include "struct_vertex_shader.glsl"
inData[];

// Out.
out 
#include "struct_geometry_shader.glsl"
outData;

void emitQuad( const vec3 p_v1, const vec3 p_v2, const vec3 p_v3, const vec3 p_v4 )
{
	outData.viewImpPos = p_v1;
	gl_Position		   = uniformsCamera.matrixProjection * vec4( outData.viewImpPos, 1.f );
	EmitVertex();

	outData.viewImpPos = p_v2;
	gl_Position		   = uniformsCamera.matrixProjection * vec4( outData.viewImpPos, 1.f );
	EmitVertex();

	outData.viewImpPos = p_v3;
	gl_Position	       = uniformsCamera.matrixProjection * vec4( outData.viewImpPos, 1.f );
	EmitVertex();

	outData.viewImpPos = p_v4;
	gl_Position		   = uniformsCamera.matrixProjection * vec4( outData.viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Do not emit primitive if sphere is not visible.
	if ( inData[ 0 ].sphereVisible == 0 )
	{
		return;
	}

	// Output data.
	outData.viewSpherePos	 = inData[ 0 ].viewSpherePos;
	outData.sphereColor		 = inData[ 0 ].sphereColor;
	outData.sphereRadius	 = inData[ 0 ].sphereRadius;
	outData.dotViewSpherePos = inData[ 0 ].dotViewSpherePos;
	outData.sphereSelected   = inData[ 0 ].sphereSelected;
	outData.sphereId		 = inData[ 0 ].sphereId;

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - inData[ 0 ].vImpU - inData[ 0 ].vImpV;
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + inData[ 0 ].vImpU - inData[ 0 ].vImpV;
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - inData[ 0 ].vImpU + inData[ 0 ].vImpV;
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + inData[ 0 ].vImpU + inData[ 0 ].vImpV;

	emitQuad( v1, v2, v3, v4 );
}
