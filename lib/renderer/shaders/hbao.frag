

#version 450 core

#include "layout_uniforms_camera.glsl"
#include "struct_data_packed.glsl"

// hbao SSAO

// In.
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureNoise;
layout( binding = 2 ) uniform sampler2D inTextureDepth;


layout ( std140, binding = 3 ) uniform Uniforms
{	
	float intensity;
	float bias;

} uniforms;

// Out.
layout( location = 0 ) out float outAmbientOcclusion;

const int noiseTextureSize = 64;

float BIAS;
const int NUM_DIR = 4;
const int NUM_SAMPLE = 4;
const float threshold = 100.f;

//Directions to integrate in
const vec2[4] dXs = vec2[4](
	vec2( 1.f, 0.f),
	vec2(-1.f, 0.f),
	vec2( 0.f, 1.f ),
	vec2( 0.f,-1.f)
) ;

//different radii to sample
const float[4] radii = float[4](
	5.f, 10.f, 15.f, 20.f
);

void main(){
	
	BIAS = uniforms.bias *3.1415/180.f;
    //Get current pixel, screen space coordinates
    ivec2 texPos= ivec2(2*gl_FragCoord.xy);
	
	
    //get depth current pixel
    float depth = -texelFetch( inTextureDepth, texPos, 0 ).x;
	
	const vec3 position = vec3(float(texPos.x),float(texPos.y),depth);



	//TODO :  Adapt radius wrt depth: the deeper the fragment is, the smaller the radius is.
	const float radius =4.f*threshold/depth;
    
	//initialise la valeur d'occlusion
	float sphereOcclusion= 0.f;
	
	float falloff  = 1.f;

    //calculate random rotation matrix based on noise
	vec2 jitter = texture( inTextureNoise, texPos / float( noiseTextureSize )).xy;
    float angle = jitter.x;
    mat2 rot = mat2(cos(angle), -sin(angle),
					sin(angle), cos(angle));

	
 

     for (int i = 0 ; i < 4; i++){
		//Calculate pixel tangent

		//get very close sample in screen space
       	vec2 dX =  rot *dXs[i];

		float depthdX = -texelFetch( inTextureDepth, ivec2(position.xy+dX), 0 ).x;
		//tangent vector
		vec3 dXPosVec3 = vec3(dX.x, dX.y, depthdX - depth);

		float angleTan = atan(dXPosVec3.z/length(dXPosVec3.xy)); 
		float sinTan = sin(angleTan+BIAS);
		

		float maxAngle = 0.f;
		
		//TODO normalize le pixel pour calculer ta tangente avant de faire quoi que ce soit d'autre
		for (int j = 0 ; j< 4; j++){
			
        	//get sample position in screen space
        	vec2 samplePos = radius*radii[j]*(dX)+ position.xy ;

		
			float depthSample = -texelFetch( inTextureDepth, ivec2(samplePos), 0 ).x;

			vec3 samplePosVec3 = vec3((samplePos.x), (samplePos.y), depthSample);

			samplePosVec3 = samplePosVec3 - position;
			float horizAngle = atan(samplePosVec3.z/length(samplePosVec3)); 

			if (horizAngle >= maxAngle && length(samplePosVec3)<threshold) maxAngle = horizAngle;

			//falloff = 0.03f /(max(0.03f, length(samplePosVec3/radius*20.f)));
			//sphereOcclusion += clamp((sin(horizAngle) - sinTan),0.f,1.f)/float(16.f);

    	} 

		sphereOcclusion += (sin(maxAngle)-sinTan)/float(4);
	}
	
	sphereOcclusion = clamp(sphereOcclusion,0.f,1.f);

	outAmbientOcclusion = pow(1.f-sphereOcclusion,uniforms.intensity);

	
}