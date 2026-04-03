#version 460 core

#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_representation.glsl"
#include "struct_geometry_shader.glsl"
#include "struct_cylinder.glsl"

layout( lines ) in;
layout( triangle_strip, max_vertices = 4 ) out;

// In.
flat in StructCylinder vsCylinder[];

// Out.
smooth out StructGeometryShaderSmooth gsDataSmooth;
flat out StructGeometryShaderFlat gsDataFlat[ 2 ];
flat out StructCylinder gsCylinder[ 2 ];

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	gsDataSmooth.viewImpostorPosition = v1;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsDataSmooth.viewImpostorPosition, 1.f );
	EmitVertex();

	gsDataSmooth.viewImpostorPosition = v2;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsDataSmooth.viewImpostorPosition, 1.f );
	EmitVertex();

	gsDataSmooth.viewImpostorPosition = v3;
	gl_Position = uniformsCamera.matrixProjection * vec4( gsDataSmooth.viewImpostorPosition, 1.f );
	EmitVertex();

	gsDataSmooth.viewImpostorPosition = v4;
	gl_Position	= uniformsCamera.matrixProjection * vec4( gsDataSmooth.viewImpostorPosition, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Output data.
	gsDataFlat[ 0 ].viewVertice = gl_in[ 0 ].gl_Position.xyz;
	gsDataFlat[ 1 ].viewVertice = gl_in[ 1 ].gl_Position.xyz;
	gsCylinder[ 0 ]				= vsCylinder[ 0 ];
	gsCylinder[ 1 ]				= vsCylinder[ 1 ];

	// Flip is vertex 0 is farther than vertex 1.
	vec3 viewImpPos0, viewImpPos1;
	if ( gsDataFlat[ 0 ].viewVertice.z < gsDataFlat[ 1 ].viewVertice.z )
	{
		viewImpPos0 = gsDataFlat[ 1 ].viewVertice;
		viewImpPos1 = gsDataFlat[ 0 ].viewVertice;
	}
	else
	{
		viewImpPos0 = gsDataFlat[ 0 ].viewVertice;
		viewImpPos1 = gsDataFlat[ 1 ].viewVertice;
	}

	float radiusCylinder = uniformsRepresentation[ vsCylinder[ 0 ].representation ].radiusCylinder;

	if ( uniformsCamera.isCameraPerspective == 1 )
	{
		// Compute normalized view vector to cylinder center.
		const vec3 view = normalize( ( viewImpPos0 + viewImpPos1 ) * 0.5f );

		// Compute cylinder coordinates system with 'x' orthogonal to 'view'.
		const vec3 z = normalize( viewImpPos1 - viewImpPos0 );
		const vec3 x = normalize( cross( view, z ) );
		const vec3 y = cross( x, z ); // no need to normalize

		// Compute impostor construction vectors.
		const float dV0 = length( viewImpPos0 );
		const float dV1 = length( viewImpPos1 );

		
		const float sinAngle = radiusCylinder / dV0;
		float		angle	 = asin( sinAngle );
		const vec3	y1		 = y * radiusCylinder;
		const vec3	x2		 = x * radiusCylinder * cos( angle );
		const vec3	y2		 = y1 * sinAngle;
		angle				 = asin( radiusCylinder / dV1 );
		const vec3 x3		 = x * ( dV1 - radiusCylinder ) * tan( angle );

		// Compute impostors vertices.
		const vec3 v1 = viewImpPos0 - x2 + y2;
		const vec3 v2 = viewImpPos0 + x2 + y2;
		const vec3 v3 = viewImpPos1 - x3 + y1;
		const vec3 v4 = viewImpPos1 + x3 + y1;

		emitQuad( v1, v2, v3, v4 );
	} else { // Orthographic

		/*
		const vec3 dirCyl = normalize( viewImpPos1 - viewImpPos0 );
		const vec3 vertStep = normalize(vec3(-dirCyl.y, dirCyl.x, 0)) * radiusCylinder;

		// Compute impostors vertices.
		const vec3 v1 = viewImpPos0 + vertStep;
		const vec3 v2 = viewImpPos0 - vertStep;
		const vec3 v3 = viewImpPos1 + vertStep;
		const vec3 v4 = viewImpPos1 - vertStep;

		emitQuad( v1, v2, v3, v4 );
		*/

		// Compute normalized view vector to cylinder center.
		const float dist = ( viewImpPos0.z + viewImpPos1.z ) * 0.5f;
		const vec3 view = normalize( vec3( 0.f, 0.f, dist ) );

		// Compute cylinder coordinates system with 'x' orthogonal to 'view'.
		const vec3 z = normalize( viewImpPos1 - viewImpPos0 );
		const vec3 x = normalize( cross( view, z ) );
		const vec3 y = cross( x, z ); // no need to normalize

		// Compute impostor construction vectors.
		const float dV0 = length( viewImpPos0 );
		const float dV1 = length( viewImpPos1 );

		const float sinAngle = radiusCylinder / dV0;
		float		angle	 = asin( sinAngle );
		const vec3	y1		 = y * radiusCylinder;
		const vec3	x2		 = x * radiusCylinder * cos( angle );
		const vec3	y2		 = y1 * sinAngle;
		angle				 = asin( radiusCylinder / dV1 );
		const vec3 x3		 = x * ( dV1 - radiusCylinder ) * tan( angle );

		// Compute impostors vertices.
		const vec3 v1 = viewImpPos0 - x2 + y2;
		const vec3 v2 = viewImpPos0 + x2 + y2;
		const vec3 v3 = viewImpPos1 - x3 + y1;
		const vec3 v4 = viewImpPos1 + x3 + y1;

		emitQuad( v1, v2, v3, v4 );
	}
}
