#version 450

//#extension GL_ARB_conservative_depth : enable
// layout(depth_greater) out float gl_FragDepth;
// layout(early_fragment_tests) in;

uniform mat4 uProjMatrix;

smooth in vec3 camImpPos;
flat in vec3   camSpherePos;
flat in vec3   sphereColor;
flat in float  sphereRad;
flat in float  dotCamSpherePos;

out vec4 fragColor;

void main()
{
	// (camImpPos - camSpherePos)^2 - sphereRad^2 = 0;
	const float a = dot( camImpPos, camImpPos );
	// b = -2.f * dot(camImpPos, camSpherePos);
	// but '-' is useless since 'b' is squared for 'delta'
	// so for 't', '-' is also removed
	const float b	  = 2.f * dot( camImpPos, camSpherePos );
	const float c	  = dotCamSpherePos - sphereRad * sphereRad;
	const float delta = b * b - 4.f * a * c;

	if ( delta < 0.f )
	{
		// fragColor = vec4(sphereColor, 0.5f);
		discard;
	}
	else
	{
		// '-' is removed (see 'b')
		const float t	   = ( b - sqrt( delta ) ) / ( 2.f * a );
		const vec3	camHit = camImpPos * t;

		const vec2 clipZW = camHit.z * uProjMatrix[ 2 ].zw + uProjMatrix[ 3 ].zw;

		gl_FragDepth = ( ( clipZW.x / clipZW.y ) + 1.f ) * 0.5f;

		const vec3 normal = normalize( camHit - camSpherePos );
		//		const vec3 lightDir = normalize(-camHit);
		//		const float diffuse = dot(normal, lightDir);
		//		fragColor = vec4(vec3(0.1f, 0.1f, 0.1f) + sphereColor *
		// diffuse, 1.f);
		//

		const vec3	lightDir = normalize( -camHit );
		const float diffuse	 = dot( normal, lightDir );
		const vec3	h		 = normalize( lightDir - camHit );
		// TODO replace spec
		const float spec = pow( max( dot( normal, h ), 0.f ), 60.f );

		const vec3 colorDiff = sphereColor * diffuse;
		const vec3 colorSpec = ( vec3( 1.f ) - sphereColor ) * spec;

		const vec3 color = colorDiff + colorSpec;

		fragColor = vec4( color, 1.f );

		//		const float intensity = dot(normal, lightDir);
		//		vec3 coef;
		//		if (intensity > 0.95)
		//			coef = vec3(0.8, 0.8, 0.8);
		//		else if (intensity > 0.5)
		//			coef = vec3(0.5, 0.5, 0.5);
		//		else if (intensity > 0.25)
		//			coef = vec3(0.4, 0.4, 0.4);
		//		else
		//			coef = vec3(0.2, 0.2, 0.2);
		//
		//		fragColor = vec4(vec3(0.1f, 0.1f, 0.1f) + sphereColor *
		// coef, 1.f); 		fragColor = vec4(sphereColor, 1.f);
	}
}
