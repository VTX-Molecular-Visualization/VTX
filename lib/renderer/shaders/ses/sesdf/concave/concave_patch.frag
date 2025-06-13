#version 450

// #define SHOW_IMPOSTORS

layout (depth_greater) out float gl_FragDepth;

struct Plane 
{
	vec3 n;
	float d;
};

struct DisplayTetrahedron
{
	vec4 point;			  // (x, y, z): coordinate of the point
	Plane plane1;
	Plane plane2;
	Plane plane3;
	int  startNeighborId;
	int  neighborNb;
	vec3 color;
};

smooth in vec3				 viewImpPos;  // Impostor position in view space.
flat   in DisplayTetrahedron tetrahedron;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

layout(std140, binding=0) uniform SesdfSettings
{
	mat4  uMVMatrix;
	mat4  uProjMatrix;
	mat4  uInvMVMatrix;
	float uProbeRadius;
	uint  uMaxProbeNeighborNb;
};

layout (std140, binding = 5) restrict readonly buffer ProbeNeighborsBuffer
{
	vec4 probeNeighbors[];
};

const float Epsilon = 1e-8;
const float Pi		= 3.1415926;
const float TwoPi   = 6.2831853;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * uProjMatrix[ 2 ].z + uProjMatrix[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}


void handleImpostor()
{
#ifdef SHOW_IMPOSTORS
		// Show impostors for debugging purpose.
		uvec4 colorNormal = uvec4( 0 );
		// fill G-buffers.
		vec3 normal = normalize(-viewImpPos);
		uvec4 viewPositionNormalCompressed;
		viewPositionNormalCompressed.x = packHalf2x16( viewImpPos.xy );
		viewPositionNormalCompressed.y = packHalf2x16( vec2( viewImpPos.z, normal.x ) );
		viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
		viewPositionNormalCompressed.w = 0; // Padding.

		// Output data.
		outViewPositionNormal = viewPositionNormalCompressed;
		outColor			  = vec4( tetrahedron.color, 32.f ); // w = specular shininess.

		gl_FragDepth = computeDepth( viewImpPos );
#else
		discard;
#endif
}

float sdPlane( in vec3 pos, in Plane plane )
{
	return dot(pos, plane.n) - plane.d;
}

bool isInTriangleFrustrum(const in vec3 pos, const in Plane p1, 
						  const in Plane p2, const in Plane p3, 
						  const in float sensibility)
{
	return sdPlane( pos, p1 ) > 1e-4 && sdPlane( pos, p2 ) > 1e-4 && sdPlane( pos, p3 ) > 1e-4;
}

bool fConcavePatchSingularities( const in vec3 point, const in DisplayTetrahedron tetrahedron )
{
	uint baseId = tetrahedron.startNeighborId;
	uint maxId  = baseId + tetrahedron.neighborNb;
	
	const vec3 vPoint = (uInvMVMatrix * vec4(point, 1.)).xyz;

	bool valid = true;
	for(uint otherId = baseId; otherId < maxId; otherId++)
	{
		const vec3 otherIntersection = probeNeighbors[otherId].xyz;
		valid = valid && distance( vPoint, otherIntersection ) - uProbeRadius > -1e-3;
	}

	return valid;
}

bool isInConcavePatch( const in vec3 point, const in float sensibility )
{
	return isInTriangleFrustrum( point, tetrahedron.plane1, tetrahedron.plane2, tetrahedron.plane3, sensibility ) 
		&& fConcavePatchSingularities(point, tetrahedron);
}

void main()
{
	const float sensibility = length(viewImpPos);
	const vec3 ro = viewImpPos;
	const vec3 rd = normalize(viewImpPos);

	const vec3 oc = ro - tetrahedron.point.xyz;
	const float b = dot( oc, rd );
	const float c = dot( oc, oc ) - uProbeRadius*uProbeRadius;
	const float h = b*b - c;

	if( h < 0. )
	{
		handleImpostor();
		return;
	}
	
	vec3 hit;
	float t = -1.;

	const float d = sqrt( h );

	const float tS1 = -b + d;
	vec3 temp = ro + rd * tS1;
	if ( isInConcavePatch(temp, sensibility) )
	{
		hit = temp;
		t = tS1;
	}
	
	const float tS2 = -b - d;
	temp = ro + rd * tS2;
	if( ( t < 0. || tS2 <= t) && isInConcavePatch(temp, sensibility))
	{
		hit = temp;
		t = tS2;
	}
	
	if ( t < 0.f  )
	{
		handleImpostor();
	}
	else
	{
		vec3 normal = normalize(hit - tetrahedron.point.xyz);

		// Both sides of the patch can be seen
		// so we make sure that its normal face the camera
		normal *= -sign(dot(normal, rd));

		// Fill depth buffer.
		gl_FragDepth = computeDepth( hit );

		// Compress position and normal.
		uvec4 viewPositionNormalCompressed;
		viewPositionNormalCompressed.x = packHalf2x16( hit.xy );
		viewPositionNormalCompressed.y = packHalf2x16( vec2( hit.z, normal.x ) );
		viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
		viewPositionNormalCompressed.w = 0; // Padding.
			
		// Output data.
		outViewPositionNormal = viewPositionNormalCompressed;
		outColor = vec4(tetrahedron.color, 32.f); // w = specular shininess.
	}
}