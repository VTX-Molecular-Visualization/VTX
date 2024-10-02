#version 450 core

#include "layout_uniforms_camera.glsl"
#include "struct_data_packed.glsl"

// line integration like SSAO

// In.
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureNoise;
layout( binding = 2 ) uniform sampler2D inTextureDepth;


layout ( std140, binding = 3 ) uniform Uniforms
{	
	float intensity;
} uniforms;

// Out.
layout( location = 0 ) out float outAmbientOcclusion;
layout (location = 1) out float outTestingValues;
const int noiseTextureSize = 64;

const float BIAS = 0.025f;
const int NUM_DIR = 4;
const int NUM_SAMPLE = 4;

//Directions to integrate in
const vec2[4] dXs = vec2[4](
	vec2(1.f,0.f),
	vec2(-1.f,0.f),
	vec2(0.f,1.f),
	vec2(0.f,-1.f)
) ;

//different radii to sample
const float[4] radii = float[4](
	5.0f,10.0f,15.0f,20.0f
);

void main(){
    //Get current pixel, screen space coordinates
    ivec2 texPos= ivec2(gl_FragCoord.xy);

	UnpackedData data;
	unpackData( inTexturePackedData, data, texPos );
	const vec3 pos = data.viewPosition;
	
    //get depth current pixel
    float depth = texelFetch( inTextureDepth, texPos, 0 ).x;

    vec3 posVec3 = vec3(float(texPos.x), float(texPos.y), depth);

	//TODO :  Adapt radius wrt depth: the deeper the fragment is, the larger the radius is.
	const float radius = 1.f;
    
	//initialise la valeur d'occlusion
	float sphereOcclusion= 0.f;
	

    //calculate random rotation matrix based on noise
    float angle = texture( inTextureNoise, texPos / float( noiseTextureSize )).x;
    mat2 rot = mat2(cos(angle), -sin(angle),
					sin(angle), cos(angle));

	
   

     for (int i = 0 ; i < 4; i++){
		//Calculate pixel tangent

		//get sample position in screen space
        ivec2 dX =  ivec2(rot *dXs[i] + texPos);
		float depthdX = texelFetch( inTextureDepth, texPos+dX, 0 ).x;

		vec3 dXPosVec3 = vec3(float(dX.x), float(dX.y), depthdX);

		float angle = atan(depthdX/length(dX)); 
		float sinTan = sin(angle);
    	//float sinHoriz = abs(depth - depthdX)/distance(dXPosVec3, posVec3);
		
		//TODO normalize le pixel pour calculer ta tangente avant de faire quoi que ce soit d'autre
		for (int j = 0 ; j< 4; j++){
	
        //get sample position in screen space
        ivec2 samplePos_frag =  ivec2(radius*radii[j]*dX + texPos);
		//float depthSample = texelFetch( inTextureDepth, samplePos_frag, 0 ).x;
		float depthSample = texture( inTextureDepth, samplePos_frag).x;

		vec3 samplePosVec3 = vec3(float(samplePos_frag.x), float(samplePos_frag.y), depthSample);


		float horizAngle = atan(depthSample/length(samplePos_frag)); 
		float sinHoriz = sin(horizAngle);

    	//float sinSample = abs(depth - depthSample)/distance(samplePosVec3, posVec3);
	
		sphereOcclusion += (sinHoriz-sinTan)/float(16);

    } }

	//remove anything under 0.5 and normalize between 1 and 0.
	
	sphereOcclusion = clamp(sphereOcclusion,0.f,1.f);

	outAmbientOcclusion = pow(1.f-sphereOcclusion,uniforms.intensity);
	outAmbientOcclusion = 1.f-sphereOcclusion;

}