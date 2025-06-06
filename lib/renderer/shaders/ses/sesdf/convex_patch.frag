#version 450

// #define SHOW_IMPOSTORS

layout (depth_greater) out float gl_FragDepth;

struct DisplayConvexPatch
{
    uint atomId;
    vec4 wsAtomData;	 // world space ith pos + ith radius
    vec4 vAtomData;		 // View space ith pos + ith Extended radius
    uvec2 elementsId;
};

smooth in vec3				 viewImpPos;  // Impostor position in view space.
flat   in DisplayConvexPatch patchData;

layout(std140, binding=0) uniform SesdfSettings
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

layout(std140, binding = 1) readonly buffer SortedAtoms {
    vec4 atoms[];
};

layout(std140, binding = 2) restrict readonly buffer Segments 
{
    uvec4 segments[];
};

layout(std140, binding = 3) readonly buffer Probes {
    vec4 probes[];
};

layout(std140, binding = 6) readonly buffer Sectors {
    vec4 sectors[];
};

const float Epsilon = 1e-6;
const float Pi		= 3.1415926;
const float TwoPi   = 6.2831853;

// https://github.com/quanchaoyu/MolSurfComp/blob/master/Core/data_I_Cir.m#L255
float length2(vec3 v){return dot(v,v);}
vec3 computeCircleCenter( vec4 c1, vec4 c2 )
{
    c1.w += uProbeRadius;
    c2.w += uProbeRadius;
    const float d = length2( c1.xyz - c2.xyz );
    const float t = ( c1.w * c1.w - c2.w * c2.w + d ) / ( 2.f * d );
    return c1.xyz + ( c2.xyz - c1.xyz ) * t;
}

float computeDepth( const vec3 v )
{
    // Computes 'v' NDC depth ([-1,1])
    const float ndcDepth = ( v.z * uProjMatrix[ 2 ].z + uProjMatrix[ 3 ].z ) / -v.z;
    // Return depth according to depth range
    return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

bool isInSector(vec3 p, vec3 o, float r) 
{
    // Reference: https://www.shadertoy.com/view/wsyyRh
    return acos( dot(p, o) ) - r < 0.f;
}

bool isInPatch(const vec3 viewP)
{
    //if(patchData.elementsId.y - patchData.elementsId.x != 16)
    const vec3 centerToPoint = (uInvMVMatrix * vec4((viewP - patchData.vAtomData.xyz) / patchData.wsAtomData.w, 0.)).xyz;
    bool isIn = true;
    for( uint i = patchData.elementsId.x; i < patchData.elementsId.y && isIn; i++) 
    {
        const vec4 sector = sectors[i];
        // if(!isnan(sector.w))
        isIn = isIn && !isInSector(centerToPoint, sector.xyz, sector.w);
    }

    return isIn;
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
    outColor			  = vec4( 1., 0., 0., 32.f ); // w = specular shininess.

    gl_FragDepth = computeDepth( viewImpPos );
#else
    discard;
#endif
}

void main()
{
    const vec3 ro = viewImpPos;
    const vec3 rd = normalize(viewImpPos);

    const vec3 oc = ro - patchData.vAtomData.xyz;
    const float b = dot( oc, rd );
    const float c = dot( oc, oc ) - patchData.wsAtomData.w * patchData.wsAtomData.w;
    const float h = b*b - c;
    const float dist = h >= 0. ? -b - sqrt( h ) : -1.;

    if ( dist < 0.f )
    {
        handleImpostor();
    }
    else
    {
        vec3 hit = ro + rd * dist;
        bool hitSes = isInPatch(hit);
        if(!hitSes)
        {
            hit = ro + rd * (-b + sqrt( h ));
            hitSes = isInPatch(hit);
        }

        if(!hitSes) 
        {
            handleImpostor();
        }
        else
        {
            vec3 normal = -normalize(hit - patchData.vAtomData.xyz);

            // Both sides of the patch can be seen
            // so we make sure that its normal face the camera
            normal = mix( -normal, normal, float(dot(normal, rd) < 0.) );

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
            // outColor = vec4(hash31(uint(res)), 32.f); // w = specular shininess.
            outColor = vec4(vec3(1., 1., 1.), 32.f); // w = specular shininess.
        }
    }
}