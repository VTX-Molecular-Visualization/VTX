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
    //float radius;
} uniforms;

// Out.
layout( location = 0 ) out float outAmbientOcclusion;

const int noiseTextureSize = 64;

//échantillons
const vec2[16] aoKernel = vec2[16](
	   vec2( 0.32445664, -0.46387565),
       vec2( 0.81621681, -0.14990926),
       vec2(-0.1438048 ,  0.76429324),
       vec2(-0.72922218, -0.09745927),
       vec2( 0.4227899 ,  0.59662847),
       vec2( 0.10553416, -0.76803416),
       vec2( 0.770926  ,  0.25649906),
       vec2( 0.37228354,  0.1666603 ),
       vec2( 0.13110044,  0.29125743),
       vec2( 0.15852943,  0.71677565),
       vec2(-0.33342448, -0.23021528),
       vec2( 0.07961414, -0.00439711),
       vec2( 0.15623727, -0.53913641),
       vec2(-0.3244394 ,  0.17778365),
       vec2(-0.78266549,  0.37232542),
       vec2( 0.92002305, -0.18765742)

);


//longueur des "tubes"
/* const float[16] sampleWidths = float[16](
 // 2*sqrt(1-(x^2+y^2))	
 	1.6486931439597876,
 	1.1159163688839602,
 	1.2572605471262712,
 	1.3546020771898228,
 	1.3642333629288579,
 	1.2633068840203099,
 	1.165986849296459 ,
 	1.8260660543112157,
 	1.8952380141495444,
 	1.3580884895806002,
 	1.8284737220848768,
 	1.9936321167784465,
 	1.6552001040467705,
 	1.8580980071741682,
 	0.9976141902169144,
 	0.6880182573298848
); */

const float[16] sampleWidthsInv = float[16](
	// 1/(2*sqrt(1-(x^2+y^2)))
	0.6065410071386762,
	0.8961245016955085,
	0.7953800843315306,
	0.7382241743453847,
	0.7330124208757883,
	0.7915733007150487,
	0.8576426060065658,
	0.54762531598409,
	0.5276382135299945,
	0.7363290445888516,
	0.5469042228617713,
	0.501597055737606,
	0.6041565594124342,
	0.5381847438288896,
	1.002391515484124,
	1.4534498021620508
);
//volume des "tubes"
const float[16] sphereWeights = float[16](

 0.08199816,
 0.04796015,
 0.06610754,
 0.07372611,
 0.04887164,
 0.06656264,
 0.04354735,
 0.06565534,
 0.05898782,
 0.04020759,
 0.11841025,
 0.04410694,
 0.05093445, 
 0.08576653,
 0.05302658,
 0.01034299
);

//volume du tube central
const float centerWeight = 0.04378792 ;


void main(){
    //Get current pixel, screen space coordinates
    ivec2 texPos= ivec2(gl_FragCoord.xy);

	//downsample
	texPos *=2;
	
    //get depth current pixel
    float depth = texelFetch( inTextureDepth, texPos, 0 ).x;

	// Adapt radius wrt depth: the deeper the fragment is, the larger the radius is.
	const float radius = 500.f/depth;
    
	//on considère que le centre de la sphère est à 50% cachée
	float sphereOcclusion= 0.5f*centerWeight;
	

    //calculate random rotation matrix based on noise
    float angle = texture( inTextureNoise, texPos / float( noiseTextureSize )).x;
    mat2 rot = mat2(cos(angle), -sin(angle),
					sin(angle), cos(angle));

     for (int i = 0 ; i < 16; i++){
	
        //get sample position in screen space
        ivec2 samplePos_frag =  ivec2(radius * rot *aoKernel[i] + texPos);

        //get depth (normalisée et pas linéaire)
        float sampleDepth = texelFetch( inTextureDepth, samplePos_frag, 0 ).x;
		
        //measure ao on the line, 
		//width not scaled because comparing normalized values
		float diff = (depth-sampleDepth)*sampleWidthsInv[i];

        float tubeOcclusion = 0.5f+ diff; 
		//length of line in sphere that is occluded, 
		//if behind, reduces occlusion percentage

		//if outside sphere do not include the value
		tubeOcclusion = clamp(tubeOcclusion,0.f,1.f);
		//weighted mean
		tubeOcclusion *=  sphereWeights[i]; 

		//add to sphere occlusion, value is between 0 and 1
		sphereOcclusion+=tubeOcclusion;

    }

	//remove anything under 0.5 and normalize between 1 and 0.
	sphereOcclusion = (sphereOcclusion-0.5f)*2;
	sphereOcclusion = clamp(sphereOcclusion,0.f,1.f);

	outAmbientOcclusion = pow(1.0f-sphereOcclusion,uniforms.intensity);

}