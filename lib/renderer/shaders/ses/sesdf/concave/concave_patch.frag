#version 460 core

#include "../../../layout_uniforms_camera.glsl"
#include "../../../layout_uniforms_model.glsl"
#include "../../../layout_uniforms_representation.glsl"
#include "../../../struct_data_packed.glsl"
#include "struct_geometry_shader.glsl"
#include "struct_plane.glsl"
#include "struct_tetrahedron.glsl"

// #define SHOW_IMPOSTORS

layout( depth_greater ) out float gl_FragDepth;

// In.
smooth in StructGeometryShader gsData;
flat in StructTetrahedron	   gsTetrahedron;

// Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;

layout( std140, binding = 5 ) restrict readonly buffer ProbeNeighborsBuffer { vec4 probeNeighbors[]; };

const float Epsilon = 1e-8;
const float Pi		= 3.1415926;
const float TwoPi	= 6.2831853;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth
		= ( v.z * uniformsCamera.matrixProjection[ 2 ].z + uniformsCamera.matrixProjection[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

float computeDepthOrtho( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * uniformsCamera.matrixProjection[ 2 ].z + uniformsCamera.matrixProjection[ 3 ].z );
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

float computeFragmentDepth( const vec3 v )
{
	return uniformsCamera.isCameraPerspective == 1 ? computeDepth( v ) : computeDepthOrtho( v );
}

void handleImpostor()
{
#ifdef SHOW_IMPOSTORS
	// Show impostors for debugging purpose.
	uvec4 colorNormal = uvec4( 0 );
	// fill G-buffers.
	vec3 normal = normalize( -gsData.viewImpPos );

	// Output data.
	packData( gsData.viewImpPos, normal, gsTetrahedron.selection, outDataPacked );
	outColor = vec4( gsTetrahedron.color.rgb, 32.f ); // w = specular shininess.

	gl_FragDepth = computeFragmentDepth( gsData.viewImpPos );
#else
	discard;
#endif
}

float sdPlane( in vec3 pos, in Plane plane ) { return dot( pos, plane.n ) - plane.d; }

bool isInTriangleFrustrum(
	const in vec3  pos,
	const in Plane p1,
	const in Plane p2,
	const in Plane p3,
	const in float sensibility
)
{
	return sdPlane( pos, p1 ) > 1e-4 && sdPlane( pos, p2 ) > 1e-4 && sdPlane( pos, p3 ) > 1e-4;
}

bool fConcavePatchSingularities( const in vec3 point, const in StructTetrahedron p_tetrahedron )
{
	uint baseId = p_tetrahedron.startNeighborId;
	uint maxId	= baseId + p_tetrahedron.neighborNb;

	const vec3 vPoint = ( uniformsModel[ p_tetrahedron.model ].matrixModelViewInv * vec4( point, 1. ) ).xyz;

	bool valid = true;
	for ( uint otherId = baseId; otherId < maxId; otherId++ )
	{
		const vec3 otherIntersection = probeNeighbors[ otherId ].xyz;
		valid						 = valid
				&& distance( vPoint, otherIntersection )
						   - uniformsRepresentation[ p_tetrahedron.representation ].SESProbeRadius
					   > -1e-3;
	}

	return valid;
}

bool isInConcavePatch( const in vec3 point, const in float sensibility )
{
	return isInTriangleFrustrum( point, gsTetrahedron.plane1, gsTetrahedron.plane2, gsTetrahedron.plane3, sensibility )
		   && fConcavePatchSingularities( point, gsTetrahedron );
}

void main()
{
	const float sensibility = length( gsData.viewImpPos );
	const vec3	ro			= gsData.viewImpPos;
	const vec3	rd			= uniformsCamera.isCameraPerspective == 1 ? normalize( gsData.viewImpPos ) : vec3( 0.f, 0.f, -1.f );

	const vec3	oc			= ro - gsTetrahedron.point.xyz;
	const float b			= dot( oc, rd );
	const float probeRadius = uniformsRepresentation[ gsTetrahedron.representation ].SESProbeRadius;
	const float c			= dot( oc, oc ) - probeRadius * probeRadius;
	const float h			= b * b - c;

	if ( h < 0. )
	{
		handleImpostor();
		return;
	}

	vec3  hit;
	float t = -1.;

	const float d = sqrt( h );

	const float tS1	 = -b + d;
	vec3		temp = ro + rd * tS1;
	if ( isInConcavePatch( temp, sensibility ) )
	{
		hit = temp;
		t	= tS1;
	}

	const float tS2 = -b - d;
	temp			= ro + rd * tS2;
	if ( ( t < 0. || tS2 <= t ) && isInConcavePatch( temp, sensibility ) )
	{
		hit = temp;
		t	= tS2;
	}

	if ( t < 0.f )
	{
		handleImpostor();
	}
	else
	{
		vec3 normal = normalize( hit - gsTetrahedron.point.xyz );

		// Both sides of the patch can be seen
		// so we make sure that its normal face the camera
		normal *= -sign( dot( normal, rd ) );

		// Fill depth buffer.
		gl_FragDepth = computeFragmentDepth( hit );

		// Output data.
		packData( hit, normal, gsTetrahedron.selection, outDataPacked );
		outColor = vec4( gsTetrahedron.color.rgb, 32.f ); // w = specular shininess.
	}
}
