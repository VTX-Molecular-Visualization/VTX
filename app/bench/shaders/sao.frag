// based on mcguire 2012 alchemyAO SAO

#version 450 core

#include "layout_uniforms_camera.glsl"
#include "struct_data_packed.glsl"



// In.
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureNoise;
layout( binding = 2 ) uniform sampler2D inTextureDepth;

layout ( std140, binding = 3 ) uniform Uniforms
{	
	float intensity;
	float radius;
} uniforms;

// Out.
layout( location = 0 ) out float outAmbientOcclusion;

const int kernelSize = 9;

const int noiseTextureSize = 64;



const float projScale = 500.f;
// how many pixels a 1m size 1m away from the camera is.  Or 500 

const float BIAS = 0.04f;
//	avoid self occlusion
const int NUM_SPIRAL_TURNS = 7;

float radius; //good value 4.0f
vec4 projInfo;

void defProjInfo(){
	const vec2 wh = vec2(uniformsCamera.resolution);
    const mat4 P = uniformsCamera.matrixProjection;
    projInfo = vec4(-2.0f/(wh.x*P[0][0]), 
                    -2.0f/(wh.y*P[1][1]),
                    ( 1.0f - P[0][2]) / P[0][0], 
                    ( 1.0f + P[1][2]) / P[1][1]); 
}

vec3 reconstructCSFaceNormal(vec3 C){ return normalize(cross(dFdy(C), dFdx(C)));}
//Reconstruct screen space unitary face normal


float reconstructZDepth (float z){
	const vec4 nf = uniformsCamera.cameraClipInfos ;
	//  _near * _far, _far, _far - _near, _near
	//  (near*far)/ ((near-far)*z + far)
    return ( nf.x ) / ( (-nf.z) * z + nf.y );
} // compute clipped depth

vec3 reconstructCSPosition(vec2 S, float z){
    return vec3((S.xy*projInfo.xy+projInfo.zw)*z, z);
} // compute clip space position using projection matrix-based coefficients

vec2 tapLocation(int index, float angleSpin){
    float alpha = float(index)+0.5f*(1.0/float(kernelSize));
    float angle = alpha * float((NUM_SPIRAL_TURNS)*6.283f)+angleSpin;

    return vec2(cos(angle),sin(angle));
} // get rotation unitary vector

vec3 positionP(ivec2 fragCoords){
    vec3 pos;
    float z = texelFetch(inTextureDepth, fragCoords,0).x;
   
	float x = float(fragCoords.x) +0.5f;
	float y = float(fragCoords.y)+ 0.5f; 
    
    pos = reconstructCSPosition(vec2(x,y),z);
    return pos;
} // get clip space position of vector on given fragment

vec3 offset(ivec2 ssC, vec2 offsetVect, float ssR){

	ivec2 ssP = ivec2(ssR*offsetVect)+ssC;
	vec3 offsetPos;
	
    offsetPos.z = texelFetch(inTextureDepth, ssP,0).x;
    
    offsetPos = reconstructCSPosition(vec2(ssP)+vec2(0.5f,0.5f), offsetPos.z);

    return offsetPos;
} // given a unitary vector, a radius and the current point, calculate screen space sample

float computeAO(ivec2 ssC, vec3 C, vec3 n_C, float ssDiskRadius, int tapIndex, float randomRotation){
    
    vec2 offset_unit = tapLocation(tapIndex,randomRotation);
    
    float alpha = float(tapIndex+0.5f)*(1.0/kernelSize);
    float ssR = ssDiskRadius* alpha;
	
    vec3 Q = offset(ssC, offset_unit, ssR*float(tapIndex+1)/float(kernelSize));
    vec3 v = Q- C;

    float vv = dot(v,v);
    float vn = dot(v, n_C);

    const float epsilon = 0.01f; // avoid division by zero
    float f = max(radius*radius - vv, 0.0f);
    
	
    return f*f*f*max((vn-BIAS)/(epsilon+vv),0.f);

} //compute ao coefficient contributed by sample k= tapIndex

void main()
{
	//define constants
	radius = uniforms.radius;
	defProjInfo();
	

	const ivec2 texPos = ivec2(2*gl_FragCoord.xy ); // multiplie par largeur et longueur

    vec3 pos = positionP(texPos);
	UnpackedData data;
	unpackData( inTexturePackedData, data, texPos ).x;
	//const vec3 pos = data.viewPosition;
    
    
    const vec3 normal_pos = reconstructCSFaceNormal(pos);
	
	vec2 randomAngle = normalize(texture(inTextureNoise, texPos / float( noiseTextureSize ) ).xy);
    const float rotation_angle = 2.0*3.1415 * randomAngle.x + randomAngle.y*texPos.x;

    float projScale = 400.f;
    float radiusSS =projScale*radius*randomAngle.y / pos.z;
   
    float ao = 0.f;

	for ( int i = 0; i < kernelSize; ++i)
	{
		ao+=computeAO(texPos, pos, normal_pos, radiusSS, i , rotation_angle);

	}
    float temp = radius*radius * radius;

    ao /= temp*temp;
    
	ao = clamp(400.f*ao, 0.f,1.f);

    // sigma = scale intensity
    const float SIGMA = 5.0f;
	outAmbientOcclusion = max(0.0, 1.f-ao*uniforms.intensity*SIGMA/kernelSize);
	
}