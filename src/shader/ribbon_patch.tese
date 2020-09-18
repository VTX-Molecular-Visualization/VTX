#version 450

layout( quads, equal_spacing, ccw ) in;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;
uniform uint u_maxIndice;

in vec3 tc_position[];
in vec3 tc_direction[];
in vec3 tc_normal[];
in uint tc_secondaryStructure[];
in vec3 tc_color[];
in uint tc_primitiveId[];

out vec3 te_viewPosition;
out vec3 te_normal;
out vec3 te_color;

#define PI 3.1415926535897932384626433832795
const mat4	BSPLINE_MAT = mat4( -1.f, 3.f, -3.f, 1.f, 3.f, -6.f, 3.f, 0.f, -3.f, 0.f, 3.f, 0.f, 1.f, 4.f, 1.f, 0.f );
const float DIRECTION_FACTOR[] = {
	2.f,  // HELIX_ALPHA_RIGHT
	2.f,  // HELIX_ALPHA_LEFT
	2.f,  // HELIX_3_10_RIGHT
	2.f,  // HELIX_3_10_LEFT
	2.f,  // HELIX_PI
	3.f,  // STRAND
	0.5f, // TURN
	0.5f  // COIL
};

vec3 evaluateBSpline( const mat4x3 p_bspline, const float p_offset )
{
	return ( 1.f / 6.f ) * p_bspline * BSPLINE_MAT
		   * vec4( p_offset * p_offset * p_offset, p_offset * p_offset, p_offset, 1.f );
}

void main()
{
	// Evaluate position.

	mat4x3 positionBSpline = mat4x3( tc_position[ 0 ], tc_position[ 1 ], tc_position[ 2 ], tc_position[ 3 ] );
	vec3   position		   = evaluateBSpline( positionBSpline, gl_TessCoord.x );
	vec3   positionOld	   = position;
	// Evaluate direction.

	mat4x3 directionBSpline = mat4x3( tc_direction[ 0 ], tc_direction[ 1 ], tc_direction[ 2 ], tc_direction[ 3 ] );
	vec3   direction		= normalize( evaluateBSpline( directionBSpline, gl_TessCoord.x ) );

	// vec3 direction = normalize( mix( tc_direction[ 1 ], tc_direction[ 2 ], gl_TessCoord.x ) );

	// Evaluate normal.

	// vec3 normalCA1 = cross( tc_direction[ 1 ], tc_position[ 2 ] - tc_position[ 1 ] );
	// vec3 normalCA2 = cross( tc_direction[ 2 ], tc_position[ 3 ] - tc_position[ 2 ] );
	// vec3 normal	   = normalize( mix( normalCA1, normalCA2, gl_TessCoord.x ) );

	mat4x3 normalBSpline = mat4x3( tc_normal[ 0 ], tc_normal[ 1 ], tc_normal[ 2 ], tc_normal[ 3 ] );
	vec3   normal		 = normalize( evaluateBSpline( normalBSpline, gl_TessCoord.x ) );

	// vec3 normal = normalize( mix( tc_normal[ 1 ], tc_normal[ 2 ], gl_TessCoord.x ) );

	float directionFactor = 2.f; // DIRECTION_FACTOR[ tc_secondaryStructure[ 1 ] ];
	float radius		  = 0.5f;

	/*
	if ( gl_PrimitiveID < 32 )
	{
		normal			= -normal;
	}
	*/

	// Move vertex along [-direction, direction].

	float directionStep = directionFactor * radius;
	position -= direction * directionStep;
	position += direction * directionStep * 2.f * gl_TessCoord.y;

	// Move vertex along normal.

	// 0.00001
	float normalStep = radius;
	// position += normal * normalStep * sin( gl_TessCoord.y * PI );

	// View position.

	te_viewPosition = vec3( u_MVMatrix * vec4( position, 1.f ) );

	// Shading normal.
	// vec3 shadingNormal = normalize( normal * sin( gl_TessCoord.y * PI ) );
	// vec3 shadingNormal = normalize( position - positionOld );
	te_normal = vec3( u_normalMatrix * vec4( normal, 1.f ) );

	// Mix colors.

	te_color = mix( tc_color[ 1 ], tc_color[ 2 ], gl_TessCoord.x );

	gl_Position = u_projMatrix * vec4( te_viewPosition, 1.f );
}
