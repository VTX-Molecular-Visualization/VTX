#version 460 core

#include "../../../layout_uniforms_camera.glsl"
#include "../../../layout_uniforms_representation.glsl"
#include "../../../struct_data_packed.glsl"
#include "struct_circle.glsl"
#include "struct_geometry_shader.glsl"

// #define SHOW_IMPOSTORS

layout (depth_greater) out float gl_FragDepth;

// In.
smooth in StructGeometryShader gsData;
flat in StructCircle gsCircle;

 // Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;

const float Epsilon			   = 1e-8;
const float Pi				   = 3.1415926;
const float TwoPi			   = 6.2831853;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * uniformsCamera.matrixProjection[ 2 ].z + uniformsCamera.matrixProjection[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

void submit(in vec3 p, in vec3 n, in vec3 c)
{
	n = faceforward(n, gsData.viewImpPos, n);

	// fill G-buffers.
	packData( p, n, 0, outDataPacked );

	// Output data.	
	gl_FragDepth = computeDepth( p );

	//outColor = vec4( c, 32.f ); // w = specular shininess.
	outColor = vec4( 1.f );
}

void handleImpostor()
{
#ifdef SHOW_IMPOSTORS
		submit(gsData.viewImpPos, normalize(-gsData.viewImpPos), vec3(1.));
#else
		discard;
#endif
}

// Using GLM's way to multiply a vector with a quaternion
vec3 quatMult(const in vec4 q, const in vec3 v)
{
	return 2. * dot( q.xyz, v ) * q.xyz + ( q.w * q.w - dot( q.xyz, q.xyz ) ) * v + 2. * q.w * cross( q.xyz, v );
}

vec3 orthogonalVector(vec3 normal) 
{
	vec3 ref = vec3(1., 0., 0.);
	if (abs(dot(normal, ref)) < 1e-4)
		ref = vec3(0., 1., 0.);
    return normalize(cross(normal, ref));
}

vec3 closestPointOnCircle( vec3 circleCenter, vec3 circleNormal, float circleRadius, vec3 point )
{
	const vec3 p1 = point + dot( circleCenter - point, circleNormal ) * circleNormal;

	const vec3 circleCenterToP1 = p1 - circleCenter;
	const float distToP1 = length( circleCenterToP1 );
	if ( abs( distToP1 ) > Epsilon )
	{
		return circleCenter + circleRadius * circleCenterToP1 / distToP1;
	}
	else
	{
		const vec3 v = orthogonalVector( circleNormal );
		return circleCenter + circleRadius * v;
	}
}

float sdToroidalPatch(const in vec3 point, out vec3 closestPoint )
{
	closestPoint = closestPointOnCircle(gsCircle.center.xyz, gsCircle.normal.xyz, gsCircle.center.w, point);
	return -distance(point, closestPoint) + uniformsRepresentation[ 0 ].SESProbeRadius;
}

const int MaxSteps		 = 100;
const float BaseBound	 = 5e-3; 
bool sphereTracing(const in vec3 ro, const in vec3 rd, const in vec2 bounds, out vec3 closestPoint, inout float t)
{
	const float bound = BaseBound;//max(length(ro) * BaseBound * 1e-1, BaseBound);

    int i = 0;
    while(t < bounds.y && i++ < MaxSteps)
    {
        vec3 p  = ro + rd * t;
        float d = abs(sdToroidalPatch(p, closestPoint));
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
	if( h<0.0 ) return vec2(-1.);
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
	const vec3 ro = gsData.viewImpPos;
	const vec3 rd = normalize(gsData.viewImpPos);
	
	vec2 dist = iOOBB(ro, rd, gsCircle.bbPos, gsCircle.bbDim, gsCircle.rot);
	vec2 sDist = iSphere(ro, rd, gsCircle.vSphere);
	
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
			submit( p, normalize(closestPoint - p), vec3(1.) );
		}
		else
		{
			//discard;
			handleImpostor();
		}
	}
}
