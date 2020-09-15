#version 450

layout( vertices = 4 ) out;

uniform vec3 uCamPosition;

in vec3 vPositionVS[];
in vec3 vDirectionVS[];
in uint vSecondaryStructureVS[];
in vec3 vColorVS[];

out vec3 vPositionTC[];
out vec3 vDirectionTC[];
out uint vSecondaryStructureTC[];
out vec3 vColorTC[];

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

float getTess( vec3 p_point )
{
	return clamp( ( distance( p_point, uCamPosition ) - MIN_DISTANCE ) / MAX_DISTANCE, 0, 1 );
}
uint getMin( uint p_ss )
{
	switch ( p_ss )
	{
	// A_HELIX_LEFT || A_HELIX_RIGHT
	case 0:
	case 1:
	case 2: return MIN_SUBDIVISION_LEVEL_HELIX;
	// B_SHEET
	case 3: return MIN_SUBDIVISION_LEVEL_SHEET;
	default: return MIN_SUBDIVISION_LEVEL_OTHER;
	}
}
uint getMax( uint p_ss )
{
	switch ( p_ss )
	{
	// A_HELIX_LEFT || A_HELIX_RIGHT
	case 0:
	case 1:
	case 2: return MAX_SUBDIVISION_LEVEL_HELIX;
	// B_SHEET
	case 3: return MAX_SUBDIVISION_LEVEL_SHEET;
	default: return MAX_SUBDIVISION_LEVEL_OTHER;
	}
}

void main()
{
	// Transmit data.
	vPositionTC[ gl_InvocationID ]			 = vPositionVS[ gl_InvocationID ];
	vDirectionTC[ gl_InvocationID ]			 = vDirectionVS[ gl_InvocationID ];
	vSecondaryStructureTC[ gl_InvocationID ] = vSecondaryStructureVS[ gl_InvocationID ];
	vColorTC[ gl_InvocationID ]				 = vColorVS[ gl_InvocationID ];

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

	gl_TessLevelOuter[ 0 ]
		= getTess( vPositionVS[ 1 ] ) * getMax( vSecondaryStructureVS[ 1 ] ) + getMin( vSecondaryStructureVS[ 1 ] );
	gl_TessLevelOuter[ 1 ] = max( getTess( vPositionVS[ 1 ] ), getTess( vPositionVS[ 2 ] ) ) * MAX_SUBDIVISION_LEVEL
							 + MIN_SUBDIVISION_LEVEL;
	gl_TessLevelOuter[ 2 ]
		= getTess( vPositionVS[ 2 ] ) * getMax( vSecondaryStructureVS[ 2 ] ) + getMin( vSecondaryStructureVS[ 2 ] );
	gl_TessLevelOuter[ 3 ] = gl_TessLevelOuter[ 1 ];
	gl_TessLevelInner[ 0 ] = gl_TessLevelOuter[ 1 ];
	gl_TessLevelInner[ 1 ] = max( gl_TessLevelOuter[ 0 ], gl_TessLevelOuter[ 2 ] );
	//}
}
