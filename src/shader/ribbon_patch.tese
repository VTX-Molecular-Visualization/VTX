// Implementation of:
// Instant visualization of secondary structures of molecular models
// P. Hermosilla & V. Guallar & A. Vinacua & P.P. Vázquez
// Eurographics Workshop on Visual Computing for Biology and Medicine (2015)

#version 450

layout( quads, fractional_even_spacing ) in;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;
uniform uint u_maxIndice;
uniform vec3 u_camPosition;

in TcOut
{
	vec3				position;
	vec3				direction;
	vec3				normal;
	flat vec3			color;
	flat unsigned short ssType;
	flat unsigned short selection;
}
tcOut[];

out TeOut
{
	vec3				viewPosition;
	vec3				normal;
	vec3				color;
	flat unsigned short selection;
}
teOut;

// clang-format off
const float s = 0.8f;
const mat4 CATMULL_ROM_MAT = mat4( -s,       2.f - s, s - 2.f,        s,
								    2.f * s, s - 3.f, 3.f - 2.f * s, -s,
								   -s,       0.f,     s,              0.f,
								   0.f,      1.f,     0.f,            0.f );									   

const mat4 HERMITE_MAT = mat4 (  0.f,  1.f,  0.f,    0.f,
								-0.5f, 0.f,  0.5f,   0.f,
								 1.f, -2.5f, 2.f,   -0.5f,
								-0.5f, 1.5f, -1.5f,  0.5f );

const mat4	BSPLINE_MAT = ( 1.f / 6.f ) * mat4( -1.f,  3.f, -3.f, 1.f, 
												 3.f, -6.f,  3.f, 0.f, 
												-3.f,  0.f,  3.f, 0.f, 
												 1.f,  4.f,  1.f, 0.f );
// clang-format on

const float DIRECTION_FACTOR[] = float[]( 4.f, // HELIX_ALPHA_RIGHT
										  4.f, // HELIX_ALPHA_LEFT
										  4.f, // HELIX_3_10_RIGHT
										  4.f, // HELIX_3_10_LEFT
										  4.f, // HELIX_PI
										  4.f, // STRAND
										  1.f, // TURN
										  1.f  // COIL
);

const float RADIUS = 0.4f;
const float ARROW_OFFSET = 3.f;

// TODO: simplify
vec3 evaluateBSpline( const mat4x3 p_bspline, const float p_offset )
{
	return p_bspline * BSPLINE_MAT * vec4( p_offset * p_offset * p_offset, p_offset * p_offset, p_offset, 1.f );
}

void main()
{
	const float u = gl_TessCoord.x;

	// Evaluate spline with De Boor's algorithm.
	const float u23 = ( u + 2.f ) / 3.f;
	const float u13 = ( u + 1.f ) / 3.f;
	const float u03 = u / 3.f;
	const float u12 = ( u + 1.f ) / 2.f;
	const float u02 = u / 2.f;

	// Interpolate position along spline.
	const vec3 p01 = mix( tcOut[ 0 ].position, tcOut[ 1 ].position, u23 );
	const vec3 p12 = mix( tcOut[ 1 ].position, tcOut[ 2 ].position, u13 );
	const vec3 p23 = mix( tcOut[ 2 ].position, tcOut[ 3 ].position, u03 );

	const vec3 p02 = mix( p01, p12, u12 );
	const vec3 p13 = mix( p12, p23, u02 );

	vec3 position = mix( p02, p13, u );

	// Get tangent.
	const vec3 tangent = normalize( p13 - p02 );
	
	// Interpolate direction along spline.
	const vec3 d01 = mix( tcOut[ 0 ].direction, tcOut[ 1 ].direction, u23 );
	const vec3 d12 = mix( tcOut[ 1 ].direction, tcOut[ 2 ].direction, u13 );
	const vec3 d23 = mix( tcOut[ 2 ].direction, tcOut[ 3 ].direction, u03 );

	const vec3 d02 = mix( d01, d12, u12 );
	const vec3 d13 = mix( d12, d23, u02 );

	vec3 direction = normalize( mix( d02, d13, u ) );

	// Interpolate normal linearly (only known for the two center control points).
	const vec3 normal = normalize( mix( tcOut[ 1 ].normal, tcOut[ 2 ].normal, gl_TessCoord.x ) );

	// Make direction orthogonal to tangent.
	direction = cross(normal, tangent);
		
	// Interpolate direction factor between the two center control points.
	const float directionFactor
		= mix( DIRECTION_FACTOR[ tcOut[ 1 ].ssType ], DIRECTION_FACTOR[ tcOut[ 2 ].ssType ], gl_TessCoord.x );

	float arrayOffset = 0.f;
	// If last segment of a strand ().
	if ( ( tcOut[ 1 ].ssType == 5us ) && ( tcOut[ 1 ].ssType != tcOut[ 2 ].ssType ) )
		arrayOffset = mix( ARROW_OFFSET, 0.f, gl_TessCoord.x );


	// Move vertex along direction
	const vec3 d = direction * ( directionFactor + arrayOffset ) * RADIUS;
	position +=  d * ( 2.f * gl_TessCoord.y - 1.f );

	// Move vertex along normal
	const vec3 n = normal * (directionFactor + arrayOffset) / directionFactor * RADIUS;
	position += n * ( 2.f * gl_TessCoord.y - 1.f ); // TODO: Check when double sided

	teOut.viewPosition = vec3( u_MVMatrix * vec4( position, 1.f ) );
	teOut.normal = vec3( u_normalMatrix * vec4( normal, 1.f ) );
	teOut.color = mix( tcOut[ 1 ].color, tcOut[ 2 ].color, gl_TessCoord.x );
	teOut.selection = tcOut[ 1 ].selection;

	gl_Position = u_projMatrix * vec4( teOut.viewPosition, 1.f );
}
