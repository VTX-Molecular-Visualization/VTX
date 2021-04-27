#version 450

layout( lines ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4  u_projMatrix;
uniform float u_cylRad;

in VsOut
{
	flat vec3			vertexColor;
	flat unsigned short vertexVisible;
	flat unsigned short vertexSelected;
}
vsIn[];

out GsOut
{
	smooth vec3			viewImpostorPosition; // Impostor position in view space.
	flat vec3			viewVertices[ 2 ];	  // Cylinder vertices position in view space.
	flat vec3			colors[ 2 ];
	flat unsigned short vertexSelected[ 2 ];
}
gsOut;

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	gsOut.viewImpostorPosition = v1;
	gl_Position				   = u_projMatrix * vec4( gsOut.viewImpostorPosition, 1.f );
	EmitVertex();

	gsOut.viewImpostorPosition = v2;
	gl_Position				   = u_projMatrix * vec4( gsOut.viewImpostorPosition, 1.f );
	EmitVertex();

	gsOut.viewImpostorPosition = v3;
	gl_Position				   = u_projMatrix * vec4( gsOut.viewImpostorPosition, 1.f );
	EmitVertex();

	gsOut.viewImpostorPosition = v4;
	gl_Position				   = u_projMatrix * vec4( gsOut.viewImpostorPosition, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Do not emit primitive if cylinder is not visible.
	if ( vsIn[ 0 ].vertexVisible == 0us || vsIn[ 1 ].vertexVisible == 0us )
	{
		return;
	}

	// Output data.
	gsOut.viewVertices[ 0 ]	  = gl_in[ 0 ].gl_Position.xyz;
	gsOut.viewVertices[ 1 ]	  = gl_in[ 1 ].gl_Position.xyz;
	gsOut.colors[ 0 ]		  = vsIn[ 0 ].vertexColor;
	gsOut.colors[ 1 ]		  = vsIn[ 1 ].vertexColor;
	gsOut.vertexSelected[ 0 ] = vsIn[ 0 ].vertexSelected;
	gsOut.vertexSelected[ 1 ] = vsIn[ 1 ].vertexSelected;

	// Flip is vertex 0 is farther than vertex 1.
	vec3 viewImpPos0, viewImpPos1;
	if ( gsOut.viewVertices[ 0 ].z < gsOut.viewVertices[ 1 ].z )
	{
		viewImpPos0 = gsOut.viewVertices[ 1 ];
		viewImpPos1 = gsOut.viewVertices[ 0 ];
	}
	else
	{
		viewImpPos0 = gsOut.viewVertices[ 0 ];
		viewImpPos1 = gsOut.viewVertices[ 1 ];
	}

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
}
