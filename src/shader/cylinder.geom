#version 450

layout( lines ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4  u_projMatrix;
uniform float u_cylRad;

flat in vec3		   vVertexColor[]; // One color per atom.
flat in uint vVertexVis[];
flat in uint vVertexSel[];

smooth out vec3			viewImpPos;		  // Impostor position in view space.
flat out vec3			viewCylVert[ 2 ]; // Cylinder vertices position in view space.
flat out vec3			colors[ 2 ];
flat out uint vertexSel[ 2 ];

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	viewImpPos	= v1;
	gl_Position = u_projMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	viewImpPos	= v2;
	gl_Position = u_projMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	viewImpPos	= v3;
	gl_Position = u_projMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	viewImpPos	= v4;
	gl_Position = u_projMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Do not emit primitive if cylinder is not visible.
	if ( vVertexVis[ 0 ] == 0 || vVertexVis[ 1 ] == 0 )
	{
		return;
	}

	// Output data.
	viewCylVert[ 0 ] = gl_in[ 0 ].gl_Position.xyz;
	viewCylVert[ 1 ] = gl_in[ 1 ].gl_Position.xyz;
	colors[ 0 ]		 = vVertexColor[ 0 ];
	colors[ 1 ]		 = vVertexColor[ 1 ];
	vertexSel[ 0 ]	 = vVertexSel[ 0 ];
	vertexSel[ 1 ]	 = vVertexSel[ 1 ];

	// Flip is vertex 0 is farther than vertex 1.
	vec3 viewImpPos0, viewImpPos1;
	if ( viewCylVert[ 0 ].z < viewCylVert[ 1 ].z )
	{
		viewImpPos0 = viewCylVert[ 1 ];
		viewImpPos1 = viewCylVert[ 0 ];
	}
	else
	{
		viewImpPos0 = viewCylVert[ 0 ];
		viewImpPos1 = viewCylVert[ 1 ];
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
