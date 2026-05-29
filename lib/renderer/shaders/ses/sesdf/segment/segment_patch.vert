#version 460

#include "../../../constant.glsl"
#include "../../../layout_uniforms_camera.glsl"
#include "../../../layout_uniforms_color.glsl"
#include "../../../layout_uniforms_model.glsl"
#include "../../../layout_uniforms_representation.glsl"
#include "../../../struct/draw_indexed_indirect.glsl"
#include "struct_segment.glsl"
#include "struct_vertex_shader.glsl"

layout( location = 0 ) in uvec4 segmentIds;

// In.
layout( std140, binding = 1 ) readonly buffer SortedAtoms { vec4 atoms[]; };

layout( std430, binding = 7 ) readonly buffer SESAtomIds { uint rendererAtomIds[]; };

layout( std430, binding = 8 ) readonly buffer AtomColors { uint atomColorWords[]; };

layout( std430, binding = 9 ) readonly buffer AtomFlags { uint atomFlagWords[]; };

layout( std140, binding = 3 ) readonly buffer Probes { vec4 probes[]; };

// Out.
flat out StructVertexShader vsData;
flat out StructSegment		vsSegment;

layout( std430, binding = 26 ) readonly buffer SegmentPatchIndirectDraws
{
	uint					  segmentPatchDrawCount;
	uint					  segmentPatchDrawPadding0;
	uint					  segmentPatchDrawPadding1;
	uint					  segmentPatchDrawPadding2;
	DrawIndexedIndirectRecord segmentPatchDraws[];
};

uint readPackedAtomColor( const uint p_index )
{
	const uint word = atomColorWords[ p_index >> 2 ];
	return ( word >> ( ( p_index & 3u ) * 8u ) ) & 0xFFu;
}

uint readPackedAtomFlag( const uint p_index )
{
	const uint word = atomFlagWords[ p_index >> 2 ];
	return ( word >> ( ( p_index & 3u ) * 8u ) ) & 0xFFu;
}

vec4 sesColor( const vec4 p_atomColor ) { return vec4( p_atomColor.rgb, 1.f ); }

const float TwoPi = 6.2831853;

// Based on GLM implementation
vec3 quatMult( vec4 q, vec3 v )
{
	const vec3 qAxis = q.xyz;
	return 2. * dot( qAxis, v ) * qAxis + ( q.w * q.w - dot( qAxis, qAxis ) ) * v + 2. * q.w * cross( qAxis, v );
}

vec4 toLocalSpaceTransform( const vec3 n )
{
	const vec3 front = vec3( 0.f, 0.f, 1.f );
	if ( dot( n, front ) < -1.f + 1e-4f )
	{
		return vec4( 1.f, 0.f, 0.f, 0.f );
	}

	float angle = 1.f + dot( n, front );
	vec3  axis	= cross( n, front );
	return normalize( vec4( axis, angle ) );
}

// https://www.shadertoy.com/view/Xt3cDn
uint baseHash( uint p )
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint	   h32 = p + PRIME32_5;
	h32			   = PRIME32_4
		  * ( ( h32 << 17 )
			  | ( h32 >> ( 32 - 17 ) ) ); // Initial testing suggests this line could be omitted for extra perf
	h32 = PRIME32_2 * ( h32 ^ ( h32 >> 15 ) );
	h32 = PRIME32_3 * ( h32 ^ ( h32 >> 13 ) );
	return h32 ^ ( h32 >> 16 );
}

vec3 hash31( uint x )
{
	uint  n	 = baseHash( x );
	uvec3 rz = uvec3( n, n * 16807U, n * 48271U ); // see: http://random.mat.sbg.ac.at/results/karl/server/node4.html
	return vec3( ( rz >> 1 ) & uvec3( 0x7fffffffU ) ) / float( 0x7fffffff );
}

float length2( vec3 v ) { return dot( v, v ); }

vec4 computeMidCircle( vec4 c1, vec4 c2, const uint representation )
{
	const float probeRadius = uniformsRepresentation[ representation ].SESProbeRadius;
	c1.w += probeRadius;
	c2.w += probeRadius;
	float d = length2( c1.xyz - c2.xyz );
	float t = ( c1.w * c1.w - c2.w * c2.w + d ) / ( 2.f * d );
	vec3  p = c1.xyz + ( c2.xyz - c1.xyz ) * t;

	vec3  circleToI				= c1.xyz - p;
	float squaredCircleDistance = dot( circleToI, circleToI );
	return vec4( p, sqrt( c1.w * c1.w - squaredCircleDistance ) );
}

vec4 computeSmallCircle( vec4 atom, vec3 n, vec3 x )
{
	vec3  sCircle		   = atom.xyz + n * dot( n, normalize( x - atom.xyz ) * atom.w );
	float sqCircleDistance = length2( atom.xyz - sCircle );
	float r				   = sqrt( atom.w * atom.w - sqCircleDistance );
	return vec4( sCircle, r );
}

// Based on https://github.com/quanchaoyu/MolSurfComp/blob/master/Dist_Computation/fsas.m#L204
float angleBetweenEdges( vec3 u, vec3 v, vec3 n )
{
	float theta = acos( clamp( dot( u, v ) / ( length( u ) * length( v ) ), -1.f, 1.f ) );

	// Use triple product to compute which way of the circle we're currently looking at.
	return sign( determinant( mat3( u, v, n ) ) ) > 0. ? theta : TwoPi - theta;
}

struct Bound
{
	vec3 pMin;
	vec3 pMax;
};

Bound getArcBoundingBox( vec3 v1, vec3 v2, vec3 n, float maxAngle )
{
	// Based on: https://stackoverflow.com/a/2618772
	vec3 right = normalize( cross( v1, n ) );
	// vec3 sMin = min(v1, min(vec3(0.), v2));
	// vec3 sMax = max(v1, max(vec3(0.), v2));
	vec3 sMin = min( v1, v2 );
	vec3 sMax = max( v1, v2 );
	for ( int i = 0; i < 3; i++ )
	{
		float angle = atan( right[ i ], v1[ i ] );
		vec3  v		= normalize( cos( angle ) * v1 + sin( angle ) * right );
		if ( angleBetweenEdges( v1, v, n ) < maxAngle )
		{
			sMax = max( sMax, v );
		}

		v = -v;
		if ( angleBetweenEdges( v1, v, n ) < maxAngle )
		{
			sMin = min( sMin, v );
		}
	}
	return Bound( sMin, sMax );
}

vec3 orthogonalVector( vec3 normal )
{
	vec3 ref = vec3( 1., 0., 0. );
	if ( abs( dot( normal, ref ) ) < 1e-4 )
	{
		ref = vec3( 0., 1., 0. );
	}
	return normalize( cross( normal, ref ) );
}

void main()
{
	const DrawIndexedIndirectRecord draw		   = segmentPatchDraws[ gl_DrawID ];
	const uint						idModel		   = draw.idModel;
	const uint						representation = draw.padding1;
	const float						probeRadius	   = uniformsRepresentation[ representation ].SESProbeRadius;

	uint	   startAtomId	   = segmentIds.x;
	const uint rendererAtomId1 = rendererAtomIds[ startAtomId ];
	const uint rendererAtomId2 = rendererAtomIds[ segmentIds.y ];
	vsSegment.model			   = idModel;
	vsSegment.representation   = representation;
	vsSegment.selection
		= ( readPackedAtomFlag( rendererAtomId1 ) | readPackedAtomFlag( rendererAtomId2 ) ) & ( 1u << FLAG_SELECTION );
	vsSegment.color = sesColor(
		( uniformsColor[ readPackedAtomColor( rendererAtomId1 ) ]
		  + uniformsColor[ readPackedAtomColor( rendererAtomId2 ) ] )
		* 0.5f
	);
	vsSegment.startAtom		= atoms[ startAtomId ];
	vsSegment.startAtom.xyz = ( uniformsModel[ idModel ].matrixModelView * vec4( vsSegment.startAtom.xyz, 1.f ) ).xyz;

	vec4 endAtom = atoms[ segmentIds.y ];
	endAtom.xyz	 = ( uniformsModel[ idModel ].matrixModelView * vec4( endAtom.xyz, 1.f ) ).xyz;

	const vec4 startIntersection = probes[ segmentIds.z ];
	const vec4 endIntersection	 = probes[ segmentIds.w ];

	vsSegment.circle = computeMidCircle( vsSegment.startAtom, endAtom, representation );

	vec3 x1p	 = ( uniformsModel[ idModel ].matrixModelView * vec4( startIntersection.xyz, 1. ) ).xyz;
	vsSegment.v1 = ( x1p - vsSegment.circle.xyz ) / vsSegment.circle.w;
	vec3 x2p	 = ( uniformsModel[ idModel ].matrixModelView * vec4( endIntersection.xyz, 1. ) ).xyz;
	vsSegment.v2 = ( x2p - vsSegment.circle.xyz ) / vsSegment.circle.w;

	vsSegment.normal.xyz = normalize( endAtom.xyz - vsSegment.startAtom.xyz );
	float maxAngle		 = angleBetweenEdges( vsSegment.v1, vsSegment.v2, vsSegment.normal.xyz );
	;
	vsSegment.normal.w = maxAngle;

	vsData.vColor = vsSegment.color;

	vec4 sCircle  = computeSmallCircle( vsSegment.startAtom, vsSegment.normal.xyz, x1p );
	vec4 sCircle2 = computeSmallCircle( endAtom, vsSegment.normal.xyz, x1p );

	vsSegment.rot = toLocalSpaceTransform( vsSegment.normal.xyz );
	Bound bound	  = getArcBoundingBox(
		  quatMult( vsSegment.rot, vsSegment.v1 ), quatMult( vsSegment.rot, vsSegment.v2 ), vec3( 0., 0., 1. ), maxAngle
	  );

	float rad  = max( sCircle.w, sCircle2.w );
	vec3  sMin = min( bound.pMin * rad, bound.pMin * max( 0., vsSegment.circle.w - probeRadius ) );
	vec3  sMax = max( bound.pMax * rad, bound.pMax * max( 0., vsSegment.circle.w - probeRadius ) );

	vec2 dim		= abs( ( sMax - sMin ).xy * .5 );
	vsSegment.bbDim = vec3( dim, length( sCircle.xyz - sCircle2.xyz ) * .5 );

	sMin = quatMult( vec4( -vsSegment.rot.xyz, vsSegment.rot.w ), sMin );
	sMax = quatMult( vec4( -vsSegment.rot.xyz, vsSegment.rot.w ), sMax );

	vsSegment.bbPos = ( sCircle.xyz + sCircle2.xyz ) * .5 + ( sMax + sMin ) * .5;

	vec3 p = x1p;
	vec3 x = normalize( p - vsSegment.startAtom.xyz ) * vsSegment.startAtom.w;
	vec3 c = ( length( p - vsSegment.startAtom.xyz )
			   / ( length( p - endAtom.xyz ) + length( p - vsSegment.startAtom.xyz ) ) )
			 * ( endAtom.xyz - vsSegment.startAtom.xyz );
	float d			  = length( x - c );
	c				  = c + vsSegment.startAtom.xyz;
	vsSegment.vSphere = vec4( c, d );

	vec4 boundingSphere;
	vec3 pMax		   = ( sCircle2.xyz + sMax );
	boundingSphere.xyz = vec3( pMax + ( sCircle.xyz + sMin ) ) * .5;
	boundingSphere.w   = distance( boundingSphere.xyz, pMax );

	if ( uniformsCamera.isCameraPerspective == 1 )
	{
		const float dotViewSpherePos = dot( boundingSphere.xyz, boundingSphere.xyz );
		const float dSphereCenter	 = sqrt( dotViewSpherePos );
		const vec3	view			 = boundingSphere.xyz / dSphereCenter;

		const vec3 viewImpPos = boundingSphere.xyz - boundingSphere.w * view;

		// Compute impostor size.
		const float sinAngle = boundingSphere.w / dSphereCenter;
		const float tanAngle = tan( asin( sinAngle ) );
		const float impSize	 = tanAngle * length( viewImpPos );

		// Compute impostor vectors.
		// TODO: simplify normalize ? (vsData.vImpU.x == 0) but normalize should be hard optimized on GPU...
		// But for cross always better doing no calculation.
		// vsData.vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
		vsData.vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
		// TODO: simplify cross ? (vsData.vImpU.x == 0) but cross should be hard optimized on GPU...
		vsData.vImpV = cross( vsData.vImpU, view ) * impSize; // No need to normalize.
		vsData.vImpU *= impSize;

		gl_Position = vec4( viewImpPos, 1.f );
	}
	else
	{
		vsData.vImpU = vec3( -1.f, 0.f, 0.f ) * boundingSphere.w;
		vsData.vImpV = vec3( 0.f, -1.f, 0.f ) * boundingSphere.w;
		gl_Position  = vec4( boundingSphere.xyz + vec3( 0.f, 0.f, boundingSphere.w ), 1.f );
	}
}
