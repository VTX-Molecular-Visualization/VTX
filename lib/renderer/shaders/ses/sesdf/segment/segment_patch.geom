#version 450

#include "../../../layout_uniforms_camera.glsl"
#include "struct_segment.glsl"
#include "struct_vertex_shader.glsl"
#include "struct_geometry_shader.glsl"

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

// In.
flat in StructVertexShader vsData[];
flat in StructSegment vsSegment[];

// Out.
smooth out StructGeometryShaderSmooth gsDataSmooth;
smooth out StructGeometryShaderFlat gsDataFlat;
flat out StructSegment gsSegment;

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	gsDataSmooth.viewImpPos	= v1;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsDataSmooth.viewImpPos, 1.f );
	EmitVertex();

	gsDataSmooth.viewImpPos	= v2;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsDataSmooth.viewImpPos, 1.f );
	EmitVertex();

	gsDataSmooth.viewImpPos	= v3;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsDataSmooth.viewImpPos, 1.f );
	EmitVertex();

	gsDataSmooth.viewImpPos	= v4;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsDataSmooth.viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Output data.
	gsSegment = vsSegment[ 0 ];
	gsDataFlat.color = vsData[ 0 ].vColor;

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vsData[ 0 ].vImpU - vsData[ 0 ].vImpV;
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vsData[ 0 ].vImpU - vsData[ 0 ].vImpV;
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vsData[ 0 ].vImpU + vsData[ 0 ].vImpV;
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vsData[ 0 ].vImpU + vsData[ 0 ].vImpV;

	emitQuad( v1, v2, v3, v4 );
}
