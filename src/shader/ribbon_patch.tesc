#version 450

layout( vertices = 4 ) out;

uniform vec3 u_camPosition;

in vec3 vs_position[];
in vec3 vs_direction[];
in uint vs_secondaryStructure[];
in vec3 vs_color[];

out vec3 tc_position[];
out vec3 tc_direction[];
out uint tc_secondaryStructure[];
out vec3 tc_color[];

// Values from original paper.
const float MAX_DISTANCE				= 160.f;
const float MIN_DISTANCE				= 10.f;
const uint	MAX_SUBDIVISION_LEVEL		= 12;
const uint	MIN_SUBDIVISION_LEVEL		= 2;
const int	MAX_SUBDIVISION_LEVEL_HELIX = 8;
const int	MIN_SUBDIVISION_LEVEL_HELIX = 2;
const int	MAX_SUBDIVISION_LEVEL_SHEET = 8;
const int	MIN_SUBDIVISION_LEVEL_SHEET = 2;
const int	MAX_SUBDIVISION_LEVEL_OTHER = 8;
const int	MIN_SUBDIVISION_LEVEL_OTHER = 2;

// Map with ss.
const int[] MIN = {
	MIN_SUBDIVISION_LEVEL_HELIX, // HELIX_ALPHA_RIGHT
	MIN_SUBDIVISION_LEVEL_HELIX, // HELIX_ALPHA_LEFT
	MIN_SUBDIVISION_LEVEL_HELIX, // HELIX_3_10_RIGHT
	MIN_SUBDIVISION_LEVEL_HELIX, // HELIX_3_10_LEFT
	MIN_SUBDIVISION_LEVEL_HELIX, // HELIX_PI
	MIN_SUBDIVISION_LEVEL_SHEET, // STRAND
	MIN_SUBDIVISION_LEVEL_OTHER, // TURN
	MIN_SUBDIVISION_LEVEL_OTHER, // COIL
};

const int[] MAX = {
	MAX_SUBDIVISION_LEVEL_HELIX, // HELIX_ALPHA_RIGHT
	MAX_SUBDIVISION_LEVEL_HELIX, // HELIX_ALPHA_LEFT
	MAX_SUBDIVISION_LEVEL_HELIX, // HELIX_3_10_RIGHT
	MAX_SUBDIVISION_LEVEL_HELIX, // HELIX_3_10_LEFT
	MAX_SUBDIVISION_LEVEL_HELIX, // HELIX_PI
	MAX_SUBDIVISION_LEVEL_SHEET, // STRAND
	MAX_SUBDIVISION_LEVEL_OTHER, // TURN
	MAX_SUBDIVISION_LEVEL_OTHER, // COIL
};

float getDistance( vec3 p_point )
{
	return clamp( ( distance( p_point, u_camPosition ) - MIN_DISTANCE ) / MAX_DISTANCE, 0, 1 );
}

void main()
{
	// Transmit data.
	tc_position[ gl_InvocationID ]			 = vs_position[ gl_InvocationID ];
	tc_direction[ gl_InvocationID ]			 = vs_direction[ gl_InvocationID ];
	tc_secondaryStructure[ gl_InvocationID ] = vs_secondaryStructure[ gl_InvocationID ];
	tc_color[ gl_InvocationID ]				 = vs_color[ gl_InvocationID ];

	// if ( gl_InvocationID == 0 )
	//{
	/*
	https://gamedev.stackexchange.com/questions/87616/opengl-quad-tessellation-control-shader

	Y (V)   1-----2
	^       |     |
	|       |     |
	|       0-----3
	|
	+-------> X (U)

	gl_TessLevelOuter[0] - for edge defined by u=0 (i.e. edge 0-1)
	gl_TessLevelOuter[1] - for edge defined by v=0 (i.e. edge 0-3)
	gl_TessLevelOuter[2] - for edge defined by u=1 (i.e. edge 2-3)
	gl_TessLevelOuter[3] - for edge defined by v=1 (i.e. edge 1-2)
	gl_TessLevelInner[0] - tess. in horizontal (u) direction
	gl_TessLevelInner[1] - tess. in vertical (v) direction
	*/

	gl_TessLevelOuter[ 0 ] = 10.f;
	gl_TessLevelOuter[ 1 ] = 10.f;
	gl_TessLevelOuter[ 2 ] = 10.f;
	gl_TessLevelOuter[ 3 ] = 10.f;
	gl_TessLevelInner[ 0 ] = 10.f;
	gl_TessLevelInner[ 1 ] = 10.f;

	/*
	gl_TessLevelOuter[ 0 ]
		= getDistance( vs_position[ 1 ] ) * MAX[ vs_secondaryStructure[ 1 ] ] + MIN[ vs_secondaryStructure[ 1 ] ];
	gl_TessLevelOuter[ 1 ]
		= max( getDistance( vs_position[ 1 ] ), getDistance( vs_position[ 2 ] ) ) * MAX_SUBDIVISION_LEVEL
		  + MIN_SUBDIVISION_LEVEL;
	gl_TessLevelOuter[ 2 ]
		= getDistance( vs_position[ 2 ] ) * MAX[ vs_secondaryStructure[ 2 ] ] + MIN[ vs_secondaryStructure[ 2 ] ];
	gl_TessLevelOuter[ 3 ] = gl_TessLevelOuter[ 1 ];
	gl_TessLevelInner[ 0 ] = gl_TessLevelOuter[ 1 ];
	gl_TessLevelInner[ 1 ] = max( gl_TessLevelOuter[ 0 ], gl_TessLevelOuter[ 2 ] );
	*/
	//}
}
