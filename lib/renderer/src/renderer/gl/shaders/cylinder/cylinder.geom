#version 450 core

layout( lines ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4  u_projMatrix;
uniform float u_cylRad;
uniform bool  u_isPerspective;

in 
#include "struct_vertex_shader.glsl"
dataIn[];

out 
#include "struct_geometry_shader.glsl"
dataOut;

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	dataOut.viewImpostorPosition = v1;
	gl_Position				   = u_projMatrix * vec4( dataOut.viewImpostorPosition, 1.f );
	EmitVertex();

	dataOut.viewImpostorPosition = v2;
	gl_Position				   = u_projMatrix * vec4( dataOut.viewImpostorPosition, 1.f );
	EmitVertex();

	dataOut.viewImpostorPosition = v3;
	gl_Position				   = u_projMatrix * vec4( dataOut.viewImpostorPosition, 1.f );
	EmitVertex();

	dataOut.viewImpostorPosition = v4;
	gl_Position				   = u_projMatrix * vec4( dataOut.viewImpostorPosition, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Do not emit primitive if cylinder is not visible.
	if ( dataIn[ 0 ].vertexVisible == 0 || dataIn[ 1 ].vertexVisible == 0 )
	{
		return;
	}

	// Output data.
	dataOut.viewVertices[ 0 ]	  = gl_in[ 0 ].gl_Position.xyz;
	dataOut.viewVertices[ 1 ]	  = gl_in[ 1 ].gl_Position.xyz;
	dataOut.colors[ 0 ]		  = dataIn[ 0 ].vertexColor;
	dataOut.colors[ 1 ]		  = dataIn[ 1 ].vertexColor;
	dataOut.vertexSelected[ 0 ] = dataIn[ 0 ].vertexSelected;
	dataOut.vertexSelected[ 1 ] = dataIn[ 1 ].vertexSelected;
	dataOut.vertexId[ 0 ]		  = dataIn[ 0 ].vertexId;
	dataOut.vertexId[ 1 ]		  = dataIn[ 1 ].vertexId;

	// Flip is vertex 0 is farther than vertex 1.
	vec3 viewImpPos0, viewImpPos1;
	if ( dataOut.viewVertices[ 0 ].z < dataOut.viewVertices[ 1 ].z )
	{
		viewImpPos0 = dataOut.viewVertices[ 1 ];
		viewImpPos1 = dataOut.viewVertices[ 0 ];
	}
	else
	{
		viewImpPos0 = dataOut.viewVertices[ 0 ];
		viewImpPos1 = dataOut.viewVertices[ 1 ];
	}

	if (u_isPerspective){
		// Compute normalized view vector to cylinder center.
		const vec3 view = normalize( ( viewImpPos0 + viewImpPos1 ) * 0.5f );

		// Compute cylinder coordinates system with 'x' orthogonal to 'view'.
		const vec3 z = normalize( viewImpPos1 - viewImpPos0 );
		const vec3 x = normalize( cross( view, z ) );
		const vec3 y = cross( x, z ); // no need to normalize

		// Compute impostor construction vectors.
		const float dV0 = length( viewImpPos0 );
		const float dV1 = length( viewImpPos1 );

		const float sinAngle = u_cylRad / dV0;
		float		angle	 = asin( sinAngle );
		const vec3	y1		 = y * u_cylRad;
		const vec3	x2		 = x * u_cylRad * cos( angle );
		const vec3	y2		 = y1 * sinAngle;
		angle				 = asin( u_cylRad / dV1 );
		const vec3 x3		 = x * ( dV1 - u_cylRad ) * tan( angle );

		// Compute impostors vertices.
		const vec3 v1 = viewImpPos0 - x2 + y2;
		const vec3 v2 = viewImpPos0 + x2 + y2;
		const vec3 v3 = viewImpPos1 - x3 + y1;
		const vec3 v4 = viewImpPos1 + x3 + y1;

		emitQuad( v1, v2, v3, v4 );
	} else { // Orthographic

		/*
		const vec3 dirCyl = normalize( viewImpPos1 - viewImpPos0 );
		const vec3 vertStep = normalize(vec3(-dirCyl.y, dirCyl.x, 0)) * u_cylRad;

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

		const float sinAngle = u_cylRad / dV0;
		float		angle	 = asin( sinAngle );
		const vec3	y1		 = y * u_cylRad;
		const vec3	x2		 = x * u_cylRad * cos( angle );
		const vec3	y2		 = y1 * sinAngle;
		angle				 = asin( u_cylRad / dV1 );
		const vec3 x3		 = x * ( dV1 - u_cylRad ) * tan( angle );

		// Compute impostors vertices.
		const vec3 v1 = viewImpPos0 - x2 + y2;
		const vec3 v2 = viewImpPos0 + x2 + y2;
		const vec3 v3 = viewImpPos1 - x3 + y1;
		const vec3 v4 = viewImpPos1 + x3 + y1;

		emitQuad( v1, v2, v3, v4 );
	}
}
