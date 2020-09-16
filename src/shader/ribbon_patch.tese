#version 450

layout( quads, equal_spacing, ccw ) in;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 uNormalMatrix;

in vec3 tc_position[];
in vec3 tc_direction[];
in uint tc_secondaryStructure[];
in vec3 tc_color[];

out vec3 te_viewPosition;
out vec3 te_normal;
out vec3 te_color;

const mat4	BSPLINE_MAT = mat4( -1.f, 3.f, -3.f, 1.f, 3.f, -6.f, 3.f, 0.f, -3.f, 0.f, 3.f, 0.f, 1.f, 4.f, 1.f, 0.f );
const float DIRECTION_FACTOR[] = {
	1.0, // HELIX_ALPHA_RIGHT
	1.0, // HELIX_ALPHA_LEFT
	1.0, // HELIX_3_10_RIGHT
	1.0, // HELIX_3_10_LEFT
	1.0, // HELIX_PI
	1.0, // STRAND
	3.0, // TURN
	3.0	 // COIL
};

vec3 evaluateBSpline( const mat4x3 p_bspline, const float p_offset )
{
	return ( 1.f / 6.f ) * p_bspline * BSPLINE_MAT
		   * vec4( p_offset * p_offset * p_offset, p_offset * p_offset, p_offset, 1.f );
}

void main()
{
	mat4x3 positionBSpline	= mat4x3( tc_position[ 0 ], tc_position[ 1 ], tc_position[ 2 ], tc_position[ 3 ] );
	mat4x3 directionBSpline = mat4x3( tc_direction[ 0 ], tc_direction[ 1 ], tc_direction[ 2 ], tc_direction[ 3 ] );

	// Evaluate bplines.
	vec3 position  = evaluateBSpline( positionBSpline, gl_TessCoord.x );
	vec3 direction = normalize( evaluateBSpline( directionBSpline, gl_TessCoord.x ) );
	vec3 normal	   = normalize( cross( tc_position[ 2 ] - position, direction ) );

	// Move vertex along [-direction, direction].
	float directionFactor = DIRECTION_FACTOR[ tc_secondaryStructure[ 1 ] ];
	//position += direction * gl_TessCoord.y * directionFactor;
	 position -= direction * directionFactor;
	 position += direction * gl_TessCoord.y * 2.f * directionFactor;
	// position += normal * gl_TessCoord.y;

	te_viewPosition = vec3( u_MVMatrix * vec4( position, 1.f ) );
	te_normal		= normal;
	te_color		= mix( tc_color[ 1 ], tc_color[ 2 ], gl_TessCoord.x );

	gl_Position = u_projMatrix * vec4( te_viewPosition, 1.f );
}
