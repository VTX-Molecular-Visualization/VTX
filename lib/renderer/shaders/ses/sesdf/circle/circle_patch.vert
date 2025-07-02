#version 450 core

#include "../../../constant.glsl"
#include "../../../layout_uniforms_camera.glsl"
#include "../../../layout_uniforms_color.glsl"
#include "../../../layout_uniforms_model.glsl"
#include "../../../layout_uniforms_representation.glsl"
#include "struct_circle.glsl"
#include "struct_vertex_shader.glsl"

// In.
layout( location = 0 ) in uvec2 atomsId;
layout( std140, binding = 1 ) readonly buffer SortedAtoms {
	vec4 atoms[];
};

// Out.
flat out StructVertexShader vsData;
flat out StructCircle vsCircle;

float length2(vec3 v){return dot(v,v);}

vec3 computeCircleCenter( vec4 c1, vec4 c2 )
{
	c1.w += uniformsRepresentation[ 0 ].SESProbeRadius;
	c2.w += uniformsRepresentation[ 0 ].SESProbeRadius;
	const float d = length2( c1.xyz - c2.xyz );
	const float t = ( c1.w * c1.w - c2.w * c2.w + d ) / ( 2.f * d );
	return c1.xyz + ( c2.xyz - c1.xyz ) * t;
}

// Based on GLM implementation
vec3 quatMult(vec4 q, vec3 v)
{
	const vec3 qAxis = q.xyz;
	return 2. * dot( qAxis, v ) * qAxis + ( q.w * q.w - dot( qAxis, qAxis ) ) * v
			+ 2. * q.w * cross( qAxis, v );
}

vec4 toLocalSpaceTransform( const vec3 n )
{
    const vec3 ref = vec3( 0.f, 0.f, 1.f );
    if ( dot( n, ref ) < -1.f + 1e-4f )
        return vec4( 1.f, 0.f, 0.f, 0.f );

    float angle = 1.f + dot( n, ref ); 
    vec3 axis   = cross( n, ref );
    return normalize( vec4( axis, angle ) );
}

vec4 computeSmallCircle(vec4 atom, vec3 n, vec3 x)
{
    vec3 sCircle = atom.xyz + n * dot(n, normalize(x - atom.xyz) * atom.w);
    float sqCircleDistance = length2(atom.xyz - sCircle);
	float r = sqrt( atom.w * atom.w - sqCircleDistance );
    return vec4(sCircle, r);
}

vec3 orthogonalVector(vec3 normal) 
{
	vec3 ref = vec3(1., 0., 0.);
	if (abs(dot(normal, ref)) < 1e-4)
		ref = vec3(0., 1., 0.);
    return normalize(cross(normal, ref));
}

struct Bound
{
    vec3 pMin;
    vec3 pMax;
};
Bound getCircleBoundingBox(vec3 n)
{
	// https://iquilezles.org/articles/diskbbox/
	vec3 v = sqrt(1. - n*n );
	return Bound(-v, v);
}

void main()
{
	const vec4 atom1 = atoms[atomsId.x];
	const vec4 atom2 = atoms[atomsId.y];
	vsCircle.firstAtom  = vec4(( uniformsModel[ 0 ].matrixModelView * vec4( atom1.xyz, 1.f ) ).xyz, atom1.w);
	const float ithExtendedRadius = vsCircle.firstAtom.w + uniformsRepresentation[ 0 ].SESProbeRadius;
	vsCircle.secondAtom = vec4(( uniformsModel[ 0 ].matrixModelView * vec4( atom2.xyz, 1.f ) ).xyz, atom2.w);

	const vec3  circleCenter = computeCircleCenter(vsCircle.firstAtom, vsCircle.secondAtom);
	const vec3  circleToI    = vsCircle.firstAtom.xyz - circleCenter;
	const float squaredCircleDistance = dot( circleToI, circleToI );
	const float radius = sqrt( ithExtendedRadius * ithExtendedRadius - squaredCircleDistance );

	vsCircle.center = vec4( circleCenter, radius );
	vsCircle.normal = vec4( normalize( vsCircle.secondAtom.xyz - vsCircle.firstAtom.xyz ), 0. );

	vsCircle.rot = toLocalSpaceTransform(vsCircle.normal.xyz);
	const Bound bound = Bound(-vec3(1., 1., 0.), vec3(1., 1., 0.)); // getCircleBoundingBox(vec3(0., 0., 1.));

	const vec3 x1p = circleCenter + orthogonalVector(vsCircle.normal.xyz) * radius;
	const vec4 sCircle  = computeSmallCircle(vsCircle.firstAtom,  vsCircle.normal.xyz, x1p);
    const vec4 sCircle2 = computeSmallCircle(vsCircle.secondAtom, vsCircle.normal.xyz, x1p);
	
    const float rad = max(sCircle.w, sCircle2.w);
    vec3 sMin = min(bound.pMin * rad, bound.pMin * max(0., radius - uniformsRepresentation[ 0 ].SESProbeRadius));
    vec3 sMax = max(bound.pMax * rad, bound.pMax * max(0., radius - uniformsRepresentation[ 0 ].SESProbeRadius));

    const vec2 dim = abs((sMax - sMin).xy * .5);
    vsCircle.bbDim = vec3(dim, length(sCircle.xyz - sCircle2.xyz) * .5);

    sMin = quatMult(vec4(-vsCircle.rot.xyz, vsCircle.rot.w), sMin);
    sMax = quatMult(vec4(-vsCircle.rot.xyz, vsCircle.rot.w), sMax);

	vsCircle.bbPos = (sCircle.xyz + sCircle2.xyz) * .5 + (sMax + sMin) * .5;

	// Compute normalized view vector.
	const float dotViewSpherePos  = dot( circleCenter, circleCenter );
	const float dSphereCenter	  = sqrt( dotViewSpherePos );
	const vec3	view			  = circleCenter / dSphereCenter;

	vec3 p = x1p;
	vec3 x = normalize(p - vsCircle.firstAtom.xyz) * vsCircle.firstAtom.w;
	vec3 c = (length(p - vsCircle.firstAtom.xyz) / (length(p - vsCircle.secondAtom.xyz) + length(p - vsCircle.firstAtom.xyz))) * (vsCircle.secondAtom.xyz - vsCircle.firstAtom.xyz);
	float d = length(x - c);
	c = c + vsCircle.firstAtom.xyz;
	vsCircle.vSphere = vec4(c, d);

	// Impostor in front of the sphere.
	const vec3 viewImpPos = vsCircle.vSphere.xyz - vsCircle.vSphere.w * view;

	// Compute impostor size.
	const float sinAngle = vsCircle.vSphere.w / dSphereCenter;
	const float tanAngle = tan( asin( sinAngle ) );
	const float impSize	 = tanAngle * length( viewImpPos );

	// Compute impostor vectors.
	// TODO: simplify normalize ? (vImpU.x == 0) but normalize should be hard optimized on GPU...
	// But for cross always better doing no calculation.
	// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
	vsData.vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
	// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
	vsData.vImpV = cross( vsData.vImpU, view ) * impSize; // No need to normalize.
	vsData.vImpU *= impSize;

	gl_Position = vec4( viewImpPos, 1.f );
}
