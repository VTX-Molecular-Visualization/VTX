#version 460 core

#include "../../../layout_uniforms_camera.glsl"
#include "struct_vertex_shader.glsl"
#include "struct_geometry_shader.glsl"
#include "struct_convex_patch.glsl"

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

// In.
flat in StructVertexShader vsData[];
flat in StructConvexPatch vsPatchData[];

// Out.
smooth out StructGeometryShader gsData;
flat out StructConvexPatch gsPatchData;

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	gsData.viewImpPos	= v1;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsData.viewImpPos, 1.f );
	EmitVertex();

	gsData.viewImpPos	= v2;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsData.viewImpPos, 1.f );
	EmitVertex();

	gsData.viewImpPos	= v3;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsData.viewImpPos, 1.f );
	EmitVertex();

	gsData.viewImpPos	= v4;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsData.viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Output data.
	gsPatchData = vsPatchData[ 0 ];

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vsData[ 0 ].vImpU - vsData[ 0 ].vImpV;
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vsData[ 0 ].vImpU - vsData[ 0 ].vImpV;
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vsData[ 0 ].vImpU + vsData[ 0 ].vImpV;
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vsData[ 0 ].vImpU + vsData[ 0 ].vImpV;

	emitQuad( v1, v2, v3, v4 );
}
