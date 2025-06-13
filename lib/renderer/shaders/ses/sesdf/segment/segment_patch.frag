#version 450

// #define SHOW_IMPOSTORS
// #define SHOW_STEPS

layout (depth_greater) out float gl_FragDepth;

struct DisplaySegment
{
	vec3  v1;
	vec3  v2;
	vec4  startAtom;
	vec4  circle; // circle center + radius
	vec4  normal; // normal + segment angle
	vec3  bbPos;
	vec3  bbDim;
	vec4  rot;
	vec4  vSphere;
};

smooth in vec3			 viewImpPos;  // Impostor position in view space.
flat   in DisplaySegment segment;
flat   in vec4			 color;
flat   in vec3			 obbRadius;
flat   in vec3			 obbPos;
flat   in vec4			 orientation;

layout(std140, binding = 0) uniform SesdfSettings
{
	mat4  uMVMatrix;
	mat4  uProjMatrix;
	mat4  uInvMVMatrix;
	float uProbeRadius;
	uint  uMaxProbeNeighborNb;
};

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

const float Epsilon			   = 1e-6;
const float Pi				   = 3.1415926;
const float TwoPi			   = 6.2831853;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * uProjMatrix[ 2 ].z + uProjMatrix[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

void submit(in vec3 p, in vec3 n, in vec3 c)
{
	n = faceforward(n, viewImpPos, n);

	// fill G-buffers.
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( p.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( p.z, n ) );
	viewPositionNormalCompressed.z = packHalf2x16( n.yz );
	viewPositionNormalCompressed.w = 0; // Padding.

	// Output data.
	outViewPositionNormal = viewPositionNormalCompressed;
	gl_FragDepth = computeDepth( p );

	outColor = vec4( c, 32.f ); // w = specular shininess.
}

void handleImpostor()
{
#ifdef SHOW_IMPOSTORS
		submit(viewImpPos, normalize(-viewImpPos), color.rgb);
#else
		discard;
#endif
}

vec3 orthogonalVector(vec3 normal) 
{
	vec3 ref = vec3(1., 0., 0.);
	if (abs(dot(normal, ref)) > 1e-4)
		ref = vec3(0., 1., 0.);
    return cross(normal, ref);
}

// Based on https://github.com/quanchaoyu/MolSurfComp/blob/master/Dist_Computation/fsas.m#L204
float angleBetweenEdges(vec3 u, vec3 v, vec3 n)
{
	const float theta = acos( clamp( dot( u, v ) / ( length( u ) * length( v ) ), -1.f, 1.f ) );
    
    // Use triple product to compute which way of the circle we're currently looking at.
    return determinant(mat3(u, v, n)) > 0. ? theta : TwoPi - theta;
}

vec3 closestPointOnCircle( vec3 circleCenter, vec3 circleNormal, float circleRadius, vec3 point )
{
	const vec3 circleCenterToP1 = point + dot( circleCenter - point, circleNormal ) * circleNormal - circleCenter;
	const float distToP1		= length( circleCenterToP1 );
	if ( abs( distToP1 ) > Epsilon ) 
	{
		return circleCenter + circleRadius * circleCenterToP1 / distToP1;
	}
	else // If p1 == circle center
	{
		const vec3 v = orthogonalVector( circleNormal );
		return circleCenter + circleRadius * v;
	}
}

// https://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
vec4 quatFromAxis(float angle, vec3 axis) 
{
	float halfAngle = angle / 2.;
	return vec4(
		axis.x * sin(halfAngle),
		axis.y * sin(halfAngle),
		axis.z * sin(halfAngle),
		cos(halfAngle)
	);
}

// Using GLM's way to multiply a vector with a quaternion
vec3 quatMult(const in vec4 q, const in vec3 v)
{
	return 2. * dot( q.xyz, v ) * q.xyz + ( q.w * q.w - dot( q.xyz, q.xyz ) ) * v + 2. * q.w * cross( q.xyz, v );
}

float sdCircle(vec3 p, vec3 c, vec3 n, float r)
{
    return length(closestPointOnCircle(c, n, r, p) - p);
}

float sdToroidalPatch(in vec3 p, in vec3 x1, in vec3 x1n, in vec3 x2, in vec3 x2n, out vec3 cl)
{
    cl = closestPointOnCircle(segment.circle.xyz, segment.normal.xyz, segment.circle.w, p);
    
    vec3 v = (cl - segment.circle.xyz) / segment.circle.w;
    float a1 = angleBetweenEdges(segment.v1, v, segment.normal.xyz);
    if( a1 < segment.normal.w )
        return -distance(p, cl) + uProbeRadius;

	if(distance(p, x1) < distance(p, x2))
		return sdCircle(p, x1, x1n, uProbeRadius);
    return sdCircle(p, x2, x2n, uProbeRadius);
}	

const int MaxSteps		 = 100;
const float BaseBound	 = 5e-3; 
bool sphereTracing(const in vec3 ro, const in vec3 rd, const in vec2 bounds, out vec3 closestPoint, inout float t)
{
	const vec3 x1 = segment.circle.xyz + segment.v1.xyz * segment.circle.w;
	const vec3 x2 = segment.circle.xyz + segment.v2.xyz * segment.circle.w;
	const vec3 x1n = cross(segment.normal.xyz, segment.v1);
    const vec3 x2n = cross(segment.normal.xyz, segment.v2);
	const float bound = max(length(ro) * BaseBound * 1e-1, BaseBound);

    int i = 0;
    while(t < bounds.y && i++ < MaxSteps)
    {
        vec3 p  = ro + rd * t;
        float d = abs(sdToroidalPatch(p, x1, x1n, x2, x2n, closestPoint));
        if( d < bound )
            return true;

        t += max(d, bound);
    }

	return false;
}

// By Inigo Quilez
// ray-box intersection
vec2 iBox( in vec3 ro, in vec3 rd, in vec3 pMin, in vec3 pMax ) 
{
    vec3 cen = 0.5*(pMin+pMax);
    vec3 rad = 0.5*(pMax-pMin);

    vec3 m = 1.0/rd;
    vec3 n = m*(ro-cen);
    vec3 k = abs(m)*rad;

    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	
	if( tN > tF || tF < 0.0) return vec2(-1.0);

	return vec2( tN, tF );
}
vec2 iSphere( in vec3 ro, in vec3 rd, in vec4 sph )
{
	vec3 oc = ro - sph.xyz;
	float b = dot( oc, rd );
	float c = dot( oc, oc ) - sph.w*sph.w;
	float h = b*b - c;
	if( h<0.0 ) return vec2(-1., -1.);
	return vec2(-b - sqrt( h ), -b + sqrt( h ));
}

vec2 iOOBB( in vec3 ro, in vec3 rd, in vec3 p, in vec3 dim, in vec4 rot)
{
    ro -= p;
    ro = quatMult(rot, ro), rd = quatMult(rot, rd);
    return iBox(ro, rd, -dim, dim);
}
void main()
{
	const vec3 ro = viewImpPos;
	const vec3 rd = normalize(viewImpPos);
	
	vec2 dist = iOOBB(ro, rd, segment.bbPos, segment.bbDim, segment.rot);
	vec2 sDist = iSphere( ro, rd, segment.vSphere );
    
	if ( dist.x == -1. || sDist.x == -1. )
	{
		discard;
		handleImpostor();
	}
	else
	{
		dist.x = max(dist.x, max(sDist.x, 0.));
		dist.y = min(dist.y, sDist.y);
		vec3 closestPoint;
		if(sphereTracing(ro, rd, dist, closestPoint, dist.x))
		{
			vec3 p = ro + rd * dist.x;
			submit( p, normalize(closestPoint - p), vec3(1.));
		}
		else
		{
			handleImpostor();
		}
	}
}
