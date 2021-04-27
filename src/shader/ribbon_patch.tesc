// Implementation of:
// Instant visualization of secondary structures of molecular models
// P. Hermosilla & V. Guallar & A. Vinacua & P.P. Vázquez
// Eurographics Workshop on Visual Computing for Biology and Medicine (2015)

#version 450

layout( vertices = 4 ) out;

uniform vec3 u_camPosition;

in VsOut
{
	vec4				position;
	vec3				direction;
	flat vec3			color;
	flat unsigned short ssType;
	flat unsigned short selection;
}
vsIn[];

out TcOut
{
	vec3				position;
	vec3				direction;
	vec3				normal;
	flat vec3			color;
	flat unsigned short ssType;
	flat unsigned short selection;
}
tcOut[];

// Values from original paper.
const float MAX_DISTANCE   = 160.f;
const float MIN_DISTANCE   = 10.f;
const float RANGE_DISTANCE = MAX_DISTANCE - MIN_DISTANCE;

// Tessellation level limits in u
const float MIN_TESS_BSPLINE   = 2.f;
const float MAX_TESS_BSPLINE   = 18.f;
const float RANGE_TESS_BSPLINE = MAX_TESS_BSPLINE - MIN_TESS_BSPLINE;

const float MAX_TESS_HELIX	= 18.f;
const float MIN_TESS_HELIX	= 2.f;
const float MAX_TESS_STRAND = 18.f;
const float MIN_TESS_STRAND = 2.f;
const float MAX_TESS_OTHER	= 12.f;
const float MIN_TESS_OTHER	= 2.f;

// Map with ss.
const float[] MIN_TESS_SS = float[]( MIN_TESS_HELIX,  // HELIX_ALPHA_RIGHT
									 MIN_TESS_HELIX,  // HELIX_ALPHA_LEFT
									 MIN_TESS_HELIX,  // HELIX_3_10_RIGHT
									 MIN_TESS_HELIX,  // HELIX_3_10_LEFT
									 MIN_TESS_HELIX,  // HELIX_PI
									 MIN_TESS_STRAND, // STRAND
									 MIN_TESS_OTHER,  // TURN
									 MIN_TESS_OTHER	  // COIL
);

const float[] MAX_TESS_SS = float[]( MAX_TESS_HELIX,  // HELIX_ALPHA_RIGHT
									 MAX_TESS_HELIX,  // HELIX_ALPHA_LEFT
									 MAX_TESS_HELIX,  // HELIX_3_10_RIGHT
									 MAX_TESS_HELIX,  // HELIX_3_10_LEFT
									 MAX_TESS_HELIX,  // HELIX_PI
									 MAX_TESS_STRAND, // STRAND
									 MAX_TESS_OTHER,  // TURN
									 MAX_TESS_OTHER	  // COIL
);

float computeTessellationFactor( vec3 p_point )
{
	return 1.f - clamp( ( length( p_point - u_camPosition ) - MIN_DISTANCE ) / RANGE_DISTANCE, 0.f, 1.f );
}

void main()
{
	// Transmit data.
	tcOut[ gl_InvocationID ].position  = vsIn[ gl_InvocationID ].position.xyz;
	tcOut[ gl_InvocationID ].direction = vsIn[ gl_InvocationID ].direction;
	tcOut[ gl_InvocationID ].ssType	   = vsIn[ gl_InvocationID ].ssType;
	tcOut[ gl_InvocationID ].color	   = vsIn[ gl_InvocationID ].color;
	tcOut[ gl_InvocationID ].selection = vsIn[ gl_InvocationID ].selection;

	// Normals are known only for the two center controls points.
	if ( gl_InvocationID == 1 || gl_InvocationID == 2 )
	{
		// dir = 1 in backbones direction and -1 in the reverse direction.
		const float dir = vsIn[ gl_InvocationID + 1 ].position.w - vsIn[ gl_InvocationID ].position.w;
		tcOut[ gl_InvocationID ].direction *= dir;
		const vec3 v02
			= normalize( ( vsIn[ gl_InvocationID + 1 ].position.xyz - vsIn[ gl_InvocationID - 1 ].position.xyz ) );

		tcOut[ gl_InvocationID ].normal = normalize( cross( v02, tcOut[ gl_InvocationID ].direction ) );
	}

	// Compute tessellation levels
	if ( gl_InvocationID == 0 )
	{
		// From OpenGL SuperBible 6th
		/*
			(0,1)            outer3           (1,1)
			  -----------------------------------
			  |              inner1             |
			  |        -----------------        |
			o |     i |                 | i     | o
			u |     n |                 | n     | u
			t |     n |                 | n     | t
			e + c0  e + c1           c2 + e  c3 + e
			r |     r |                 | r     | r
			0 |     0 |                 | 0     | 2
			  |       |                 |       |
			  |        -----------------        |
		v     |              inner1             |
		^	  -----------------------------------
		|	(0,0)            outer1           (1,0)
		|
		+-----> u
		*/

		// Patch is defined for the two center controls points.
		const vec3 c1 = vsIn[ 1 ].position.xyz;
		const vec3 c2 = vsIn[ 2 ].position.xyz;

		// Adaptive tessellation factors wrt the distance of the central control points and the camera.
		const float tessFactorC1 = computeTessellationFactor( c1 );
		const float tessFactorC2 = computeTessellationFactor( c2 );

		// Tessellation limits wrt secondary structure type.
		const float tessMinC1 = MIN_TESS_SS[ vsIn[ 1 ].ssType ];
		const float tessMaxC1 = MAX_TESS_SS[ vsIn[ 1 ].ssType ];
		const float tessMinC2 = MIN_TESS_SS[ vsIn[ 2 ].ssType ];
		const float tessMaxC2 = MAX_TESS_SS[ vsIn[ 2 ].ssType ];

		const float vTessC1			  = tessFactorC1 * ( tessMaxC1 - tessMinC1 ) + tessMinC1;
		const float vTessC2			  = tessFactorC2 * ( tessMaxC2 - tessMinC2 ) + tessMinC2;
		const float uTesselationLevel = max( tessFactorC1, tessFactorC2 ) * ( RANGE_TESS_BSPLINE ) + MIN_TESS_BSPLINE;

		gl_TessLevelOuter[ 0 ] = vTessC1;
		gl_TessLevelOuter[ 1 ] = uTesselationLevel;
		gl_TessLevelOuter[ 2 ] = vTessC2;
		gl_TessLevelOuter[ 3 ] = uTesselationLevel;
		gl_TessLevelInner[ 0 ] = max( vTessC1, vTessC2 );
		gl_TessLevelInner[ 1 ] = uTesselationLevel;
	}
}
