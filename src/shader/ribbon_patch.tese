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
	2.f,  // HELIX_ALPHA_RIGHT
	2.f,  // HELIX_ALPHA_LEFT
	2.f,  // HELIX_3_10_RIGHT
	2.f,  // HELIX_3_10_LEFT
	2.f,  // HELIX_PI
	10.f, // STRAND
	1.f,  // TURN
	1.f	  // COIL
};

vec3 evaluateBSpline( mat4x3 p_bspline, float p_offset )
{
	return ( 1.f / 6.f ) * p_bspline * BSPLINE_MAT
		   * vec4( p_offset * p_offset * p_offset, p_offset * p_offset, p_offset, 1.f );
}

void main()
{
	mat4x3 bspline	  = mat4x3( tc_position[ 0 ], tc_position[ 1 ], tc_position[ 2 ], tc_position[ 3 ] );
	vec3   direction  = mix( tc_direction[ 1 ], tc_direction[ 2 ], gl_TessCoord.x );
	vec3   normalCA1 = cross( tc_position[ 1 ] - tc_position[ 0 ], tc_direction[ 1 ] );
	vec3   normalCA2 = cross( tc_position[ 2 ] - tc_position[ 1 ], tc_direction[ 2 ] );
	vec3   normal	  = normalize( mix( normalCA1, normalCA2, gl_TessCoord.x ) );

	vec3 vertex = evaluateBSpline( bspline, gl_TessCoord.x );

	vertex += direction * gl_TessCoord.y * DIRECTION_FACTOR[ tc_secondaryStructure[ 1 ] ];
	// vertex += normale ;//* cos( radians( gl_TessCoord.y * 90.f ) ) ;

	te_viewPosition = vec3( u_MVMatrix * vec4( vertex, 1.f ) );
	te_normal		= normal;
	te_color		= mix( tc_color[ 1 ], tc_color[ 2 ], gl_TessCoord.x );

	gl_Position = u_projMatrix * vec4( te_viewPosition, 1.f );
}
