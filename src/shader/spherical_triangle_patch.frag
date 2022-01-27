#version 450

//#define SHOW_IMPOSTORS

layout (depth_greater) out float gl_FragDepth;

layout(std140, binding = 1) readonly buffer IntersectionBuffer {
	vec4 neighborsBuffer[];
};

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;
uniform uint uMaxProbeNeighborNb;

in GEOM_OUT 
{
	smooth vec3  viewImpPos;	  // Impostor position in view space.
	flat   float dotViewSpherePos;
	flat   vec4  probePos;
	flat   int	 probeHash;
	flat   float probeIntersectionNumber;
	flat   vec3  vert1;
	flat   vec3  vert2;
	flat   vec3  vert3;
} inData;
	   

layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

const float EPSILON = 1e-4;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * uProjMatrix[ 2 ].z + uProjMatrix[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

float dot1( vec3 v1)
{
    return dot( v1, v1);
}

bool planeIntersect(vec3 v1, vec3 v2, vec3 v3, vec3 position, vec3 intersection ) 
{
    vec3 planeNormal = normalize( cross( v1.xyz, v2.xyz));
    float d = dot( position.xyz, planeNormal);
    float dist1 = dot( intersection, planeNormal ) - d;
    float dist2 = dot( v3.xyz + position.xyz, planeNormal ) - d;
    return ( dist2 < -EPSILON && dist1 > EPSILON ) || ( dist2 > EPSILON && dist1 < -EPSILON );
}

void main()
{
    const float squaredRadius = inData.probePos.w * inData.probePos.w;
	const float a = dot( inData.viewImpPos, inData.viewImpPos );

	// b = -dot(viewImpPos, viewSpherePos);
	// But '-' is useless since 'b' is squared for 'delta'.
	// So for 't', '-' is also removed.
	const float b	  = dot( inData.viewImpPos, inData.probePos.xyz );
	const float c	  = inData.dotViewSpherePos - inData.probePos.w * inData.probePos.w;
	const float delta = b * b - a * c;

    if (delta < -EPSILON) 
    { 
#ifdef SHOW_IMPOSTORS
		// Show impostors for debugging purpose.
		// fill G-buffers.
		uvec4 viewPositionNormalCompressed;
		viewPositionNormalCompressed.x = packHalf2x16( viewImpPos.xy );
		viewPositionNormalCompressed.y = packHalf2x16( vec2( viewImpPos.z, -probePos.x ) );
		viewPositionNormalCompressed.z = packHalf2x16( -probePos.yz );
		viewPositionNormalCompressed.w = 0; // Padding.

		// Output data.
		outViewPositionNormal = viewPositionNormalCompressed;
		outColor			  = vec4( 1.f, 0.f, 0.f, 32.f ); // w = specular shininess.

		gl_FragDepth = computeDepth( viewImpPos );
#else
		discard;
#endif
    }
    else 
    {
	    const float t = ( b + sqrt( delta ) ) / a;
	    const vec3 hit = inData.viewImpPos * t;

        // cut with plane 1
        if(planeIntersect(inData.vert2, inData.vert3, inData.vert1, inData.probePos.xyz, hit)) { discard; } 
    
        // cut with plane 2
        if(planeIntersect(inData.vert1, inData.vert3, inData.vert2, inData.probePos.xyz, hit)) { discard; } 
        
        // cut with plane 3
        if(planeIntersect(inData.vert1, inData.vert2, inData.vert3, inData.probePos.xyz, hit)) { discard; } 

        if( inData.probeIntersectionNumber > 0. ) {
            const uint baseCoordinates = inData.probeHash * uMaxProbeNeighborNb;
            for(int i = 0; i < inData.probeIntersectionNumber; i++ ) {
                vec3 otherProbe = neighborsBuffer[ baseCoordinates + i ].xyz;

                otherProbe = (uMVMatrix * vec4(otherProbe, 1.)).xyz;
                if( dot1( otherProbe - inData.probePos.xyz ) > EPSILON && 
                    dot1( hit - otherProbe ) - squaredRadius < -EPSILON) { discard; }
            }
        }

        // compute normal at intersection point
	    const vec3 normal = -normalize( hit - inData.probePos.xyz );
    
        gl_FragDepth = computeDepth(hit);
    
        // Compress position and normal.
	    uvec4 viewPositionNormalCompressed;
	    viewPositionNormalCompressed.x = packHalf2x16( hit.xy);
	    viewPositionNormalCompressed.y = packHalf2x16( vec2( hit.z, normal.x ) );
	    viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
	    viewPositionNormalCompressed.w = 0; // Padding.

	    // Output data.
	    outViewPositionNormal = viewPositionNormalCompressed;
        outColor			  = vec4(0., 0., 1., 32.f ); // w = specular shininess.
    }
}
