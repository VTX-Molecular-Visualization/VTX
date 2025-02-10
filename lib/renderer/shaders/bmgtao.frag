#version 450 core

#include "layout_uniforms_camera.glsl"
#include "struct_data_packed.glsl"

// Uniforms and textures
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureNoise;
layout( binding = 2 ) uniform sampler2D inTextureDepth;
layout( binding = 3 ) uniform sampler2D inTextureColor;
const int noiseTextureSize = 64;

// Output variables
layout( location = 0 ) out float outAmbientOcclusion;
layout( location = 1 ) out float testValue;
layout(location = 2) out vec4 outColor;

// Uniform block for SSAO parameters
layout ( std140, binding =4 ) uniform Uniforms
{
    float intensity;
    float radius; //10 is a pretty good value
} uniforms;

// Constants
const float T = 1.f; // Thickness of samples
const float PI = 3.14159265359f;
const float HALF_PI = 1.57079632679f;
const float PI_INV = 0.31830988618f;

const int number_sectors = 32; //uint is 32 bits
const float NUMSECTORSINV = 1.0f / float(number_sectors);

#define NUM_STEPS 5
const float stepSize = 1.0f *0.2; // 1/ number of samples on a ray

#define NUM_DIRECTIONS 3
const float NUMDIR_inv = 1.0f / (float(NUM_DIRECTIONS));
const vec3 directions[NUM_DIRECTIONS] = vec3[NUM_DIRECTIONS](vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f)); 
vec4 localIllumination;

uint globalOccludedBitfield ;

//get bitmask for a sample
uint getBitMaskSample(const float minHoriz, const float maxHoriz);
// Function to update the occlusion sectors
void updateSectors(const float minHoriz, const float maxHoriz) ;
// Function to calculate visibility along a direction
void getVisibility(vec3 position, vec3 direction, float radius, vec3 normal, vec3 V); ;
vec3 getNormal(vec3 position);
float fastAcos(float x) ;

void main() {
    localIllumination = vec4(0.0f);
    // Get the texture position from the fragment coordinates
    const ivec2 texPos = ivec2(gl_FragCoord.xy);
    globalOccludedBitfield = 0u;
    // Scale the texture position for the noise texture
    const ivec2 smallerTexPos = ivec2(texPos.x * 2, texPos.y * 2);
    
    // Get view position from data
    UnpackedData data;
    unpackData(inTexturePackedData, data, smallerTexPos);
    const vec3 pos = data.viewPosition;
    const vec3 normal = normalize(data.normal);

    // Get a random vector from the noise texture
    const vec3 randomVec = normalize(texture(inTextureNoise, smallerTexPos / float(noiseTextureSize)).xyz);
    
    float integral = 0.f;
    //TBN with gram schmidt
    const vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    const vec3 bitangent = cross(normal, tangent);
    const mat3 TBN = mat3(tangent, bitangent, normal);
    const vec3 V = -normalize(pos);

    //compute radius 
    float radius = 10.f ;
    
    // Loop through the number of directions to calculate occlusion
    for (int i = 0; i < NUM_DIRECTIONS; i++){
        globalOccludedBitfield = 0u;
        // Rotate the direction vector randomly
        vec3 direction = TBN*directions[i];
        direction = normalize(direction);
        
        // Calculate the slice normal and center
        vec3 sliceNormal = normalize(cross(direction, V));
    
        // Get the visibility for the current direction, updates global occluded bitfield
        getVisibility(pos, direction, radius, normal,V);

        // Accumulate the occlusion integral
        integral += float(bitCount(globalOccludedBitfield)) * NUMSECTORSINV;
    }
    
    // Average the occlusion integral over the number of directions
    integral *= NUMDIR_inv;
    // Output the ambient occlusion value
    outAmbientOcclusion = pow(1.0f- integral,uniforms.intensity);
    outColor = 2*localIllumination * NUMDIR_inv;
}

uint getBitMaskSample( const float minHoriz , const float maxHoriz) {
    int startBit = int(floor(minHoriz * number_sectors * PI_INV));
    int horizAngle = int(ceil((maxHoriz - minHoriz) * number_sectors * PI_INV));
    uint angleBit = (1u << horizAngle) - 1u;
    return angleBit << startBit;
}

// Function to update the occlusion sectors
void updateSectors(const float minHoriz, const float maxHoriz) {
  
    //get bitmask for a sample
    uint currentBitfield = getBitMaskSample(minHoriz, maxHoriz);
    //update the global bitfield
    globalOccludedBitfield = uint(globalOccludedBitfield | currentBitfield);
}

// Function to calculate visibility along a direction
void getVisibility(vec3 position, vec3 direction,float radius, vec3 projectedNormal, vec3 V) {
    // Vector pointing from the sample position to the view position
    
    
    // Loop through the samples along the direction
    for (int i = -NUM_STEPS; i < NUM_STEPS; i++) {
        if (i == 0) {

            continue;
        }
        // Calculate the sample position along the direction
        
        vec2 samplePos =( position.xyz + radius* float(i)*stepSize*direction.xyz).xy;
        // Project the sample position to screen space
        vec4 offset = uniformsCamera.matrixProjection * vec4(samplePos, position.z, 1.f);
        offset.xy = offset.xy / offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;

        // Get the depth value at the sample position
        float samplePosFrontDepth = texture(inTextureDepth, offset.xy, 0).x;
        vec3 samplePosFront = vec3(samplePos.xy, samplePosFrontDepth);
        // Calculate the vector from the sample position to position
        // z is a sum because position.z is negative, and samplePosFrontDepth is positive (depth buffer is positive, data is negative)
       
       //TODO WHY IS THIS A NEGATIVE SIGN IN FRONT
        vec3 horizVec = vec3((samplePos - position.xy), max(0.f, -position.z - samplePosFrontDepth));
		        
        // compute occlusion
        vec3 horizVecBack = horizVec - T*V;
        horizVecBack.z = max(horizVecBack.z, 0.0f);
        

        horizVec = normalize(horizVec);
        horizVecBack = normalize(horizVecBack);

        vec2 frontBackHoriz = vec2(dot(horizVec, direction), dot(horizVecBack, direction));
        frontBackHoriz = vec2(acos(frontBackHoriz.x), acos(frontBackHoriz.y));

        //frontBackHoriz = vec2(fastAcos(frontBackHoriz.x), fastAcos(frontBackHoriz.y));
        // Ensure the angles are in the correct order
        if (frontBackHoriz.x > frontBackHoriz.y) {
            frontBackHoriz = vec2(frontBackHoriz.y, frontBackHoriz.x);
        }

        //compute indirect lighting
        vec3 normal_sample = getNormal(samplePosFront);
        vec4 sampleColor = texture(inTextureColor, offset.xy, 0);
        uint bitmask = getBitMaskSample(frontBackHoriz.x, frontBackHoriz.y);
        float unoccluded = float(bitCount(bitmask & (~globalOccludedBitfield)))*NUMSECTORSINV  ;
        localIllumination += sampleColor * unoccluded * abs(dot(horizVec, projectedNormal) * dot(horizVec, - normal_sample));
        
    
        updateSectors(frontBackHoriz.x, frontBackHoriz.y);

    }

    // Return the updated occlusion bitfield
    
}


vec3 getNormal(vec3 position){return normalize(cross(dFdx(position)  ,dFdy(position)));}

float fastAcos(float x) {

  float negate = float(x < 0);
  x = abs(x);
  float ret = -0.0187293;
  ret = ret * x;
  ret = ret + 0.0742610;
  ret = ret * x;
  ret = ret - 0.2121144;
  ret = ret * x;
  ret = ret + 1.5707288;
  ret = ret * sqrt(1.0-x);
  ret = ret - 2 * negate * ret;
  return negate * 3.14159265358979 + ret;
} //TODO i don't think it's worth it