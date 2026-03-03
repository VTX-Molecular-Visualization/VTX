#version 460 core

#include "../../../layout_uniforms_camera.glsl"
#include "../../../layout_uniforms_model.glsl"
#include "../../../layout_uniforms_representation.glsl"
#include "../../../struct_data_packed.glsl"
#include "struct_convex_patch.glsl"
#include "struct_geometry_shader.glsl"

// #define SHOW_IMPOSTORS

layout (depth_greater) out float gl_FragDepth;

// In.
smooth in StructGeometryShader gsData;
flat in StructConvexPatch gsPatchData;

 // Out.
layout( location = 0 ) out PackedData outDataPacked;
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
    c1.w += uniformsRepresentation[ 0 ].SESProbeRadius;
    c2.w += uniformsRepresentation[ 0 ].SESProbeRadius;
    const float d = length2( c1.xyz - c2.xyz );
    const float t = ( c1.w * c1.w - c2.w * c2.w + d ) / ( 2.f * d );
    return c1.xyz + ( c2.xyz - c1.xyz ) * t;
}

float computeDepth( const vec3 v )
{
    // Computes 'v' NDC depth ([-1,1])
    const float ndcDepth = ( v.z * uniformsCamera.matrixProjection[ 2 ].z + uniformsCamera.matrixProjection[ 3 ].z ) / -v.z;
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
    //if(gsPatchData.elementsId.y - gsPatchData.elementsId.x != 16)
    const vec3 centerToPoint = (uniformsModel[ 0 ].matrixModelViewInv * vec4((viewP - gsPatchData.vAtomData.xyz) / gsPatchData.wsAtomData.w, 0.)).xyz;
    bool isIn = true;
    for( uint i = gsPatchData.elementsId.x; i < gsPatchData.elementsId.y && isIn; i++) 
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
    vec3 normal = normalize(-gsData.viewImpPos);

    // Output data.
    packData( gsData.viewImpPos, normal, 0, outDataPacked );
    outColor			  = vec4( 1., 0., 0., 32.f ); // w = specular shininess.

    gl_FragDepth = computeDepth( gsData.viewImpPos );
#else
    discard;
#endif
}

void main()
{
    const vec3 ro = gsData.viewImpPos;
    const vec3 rd = normalize(gsData.viewImpPos);

    const vec3 oc = ro - gsPatchData.vAtomData.xyz;
    const float b = dot( oc, rd );
    const float c = dot( oc, oc ) - gsPatchData.wsAtomData.w * gsPatchData.wsAtomData.w;
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
            vec3 normal = -normalize(hit - gsPatchData.vAtomData.xyz);

            // Both sides of the patch can be seen
            // so we make sure that its normal face the camera
            normal = mix( -normal, normal, float(dot(normal, rd) < 0.) );

            // Fill depth buffer.
            gl_FragDepth = computeDepth( hit );
            
            // Output data.
            packData( hit, normal, 0, outDataPacked );
            // outColor = vec4(hash31(uint(res)), 32.f); // w = specular shininess.
            outColor = vec4(vec3(1., 1., 1.), 32.f); // w = specular shininess.
        }
    }
}