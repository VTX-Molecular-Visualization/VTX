#version 450 core

#include "../layout_uniforms_camera.glsl"
#include "struct_sphere.glsl"
#include "struct_vertex_shader.glsl"
#include "struct_geometry_shader.glsl"

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

// In.
flat in StructVertexShader vsData[];
flat in StructSphere vsSphere[];

// Out.
smooth out StructGeometryShader gsData;
flat out StructSphere gsSphere;

void emitQuad( const vec3 p_v1, const vec3 p_v2, const vec3 p_v3, const vec3 p_v4 )
{
	gsData.viewImpPos = p_v1;
	gl_Position		   = uniformsCamera.matrixProjection * vec4( gsData.viewImpPos, 1.f );
	EmitVertex();

	gsData.viewImpPos = p_v2;
	gl_Position		   = uniformsCamera.matrixProjection * vec4( gsData.viewImpPos, 1.f );
	EmitVertex();

	gsData.viewImpPos = p_v3;
	gl_Position	       = uniformsCamera.matrixProjection * vec4( gsData.viewImpPos, 1.f );
	EmitVertex();

	gsData.viewImpPos = p_v4;
	gl_Position		   = uniformsCamera.matrixProjection * vec4( gsData.viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Do not emit primitive if sphere is not visible.
	if ( vsSphere[ 0 ].isVisible == 0 )
	{
		return;
	}

	// Output data.
	gsSphere = vsSphere[ 0 ];

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vsData[ 0 ].vImpU - vsData[ 0 ].vImpV;
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vsData[ 0 ].vImpU - vsData[ 0 ].vImpV;
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vsData[ 0 ].vImpU + vsData[ 0 ].vImpV;
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vsData[ 0 ].vImpU + vsData[ 0 ].vImpV;

	emitQuad( v1, v2, v3, v4 );
}
