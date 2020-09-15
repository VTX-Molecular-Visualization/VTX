#version 450

layout( quads, equal_spacing, ccw ) in;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormalMatrix;

in vec3 vPositionTC[];
in vec3 vDirectionTC[];
in uint vSecondaryStructureTC[];
in vec3 vColorTC[];

out vec3 vViewPositionTE;
out vec3 vNormaleTE;
out vec3 vColorTE;

mat4 bsplineMat = mat4( -1.f, 3.f, -3.f, 1.f, 3.f, -6.f, 3.f, 0.f, -3.f, 0.f, 3.f, 0.f, 1.f, 4.f, 1.f, 0.f );

vec3 evaluateBSpline( mat4x3 p_bspline, float p_offset )
{
	return ( 1.f / 6.f ) * p_bspline * bsplineMat
		   * vec4( p_offset * p_offset * p_offset, p_offset * p_offset, p_offset, 1.f );
}

void main()
{
	mat4x3 bspline	  = mat4x3( vPositionTC[ 0 ], vPositionTC[ 1 ], vPositionTC[ 2 ], vPositionTC[ 3 ] );
	vec3   direction  = mix( vDirectionTC[ 1 ], vDirectionTC[ 2 ], gl_TessCoord.x );
	vec3   normaleCA1 = cross( vPositionTC[ 1 ] - vPositionTC[ 0 ], vDirectionTC[ 1 ] );
	vec3   normaleCA2 = cross( vPositionTC[ 2 ] - vPositionTC[ 1 ], vDirectionTC[ 2 ] );
	vec3   normale	  = -normalize( mix( normaleCA1, normaleCA2, gl_TessCoord.x ) );

	vec3 vertex = evaluateBSpline( bspline, gl_TessCoord.x );

	uint  currentSS = vSecondaryStructureTC[ 1 ];
	float factor	= 1.f;

	// HELIX
	if ( currentSS == 0 || currentSS == 1 || currentSS == 2 )
	{
		factor = 2.f;
	}
	// STRAND
	else if ( currentSS == 3 )
	{
		factor = 8.f;
	}
	// TURN AND COIL
	else
	{
		factor = 0.5f;
	}

	vertex += direction * gl_TessCoord.y * factor;
	// vertex += normale ;//* cos( radians( gl_TessCoord.y * 90.f ) ) ;

	vViewPositionTE = vec3( uMVMatrix * vec4( vertex, 1.f ) );
	vNormaleTE		= normale;
	vColorTE		= mix( vColorTC[ 1 ], vColorTC[ 2 ], gl_TessCoord.x );

	gl_Position = uProjMatrix * vec4( vViewPositionTE, 1.f );
}
