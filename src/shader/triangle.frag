#version 450

uniform mat4 uProjMatrix;

smooth in vec4 position;
smooth in vec3 normal;
smooth in vec3 color;

// 3 16 bits for color
// 3 16 bits for normal
// 1 32 bits for padding
layout( location = 0 ) out uvec4 outColorNormal;
// 3 32 bits for position in cam space
// 1 32 bits for specular
layout( location = 1 ) out vec4 outCamPosition;

void main()
{
	// Retrouver positon espace caméra en multipliant par l'inverse de la proj (inverse())

	vec4 positionCameraSpace = inverse( uProjMatrix ) * position;

	uvec4 colorNormal = uvec4( 0 );
	vec4  camPosition = vec4( 0 );

	colorNormal.x = packHalf2x16( color.xy );
	colorNormal.y = packHalf2x16( vec2( color.z, normal.x ) );
	colorNormal.z = packHalf2x16( normal.yz );
	colorNormal.w = 0; // padding

	camPosition.xyz = vec3( positionCameraSpace );
	camPosition.w	= 60.f; // specular

	outColorNormal = colorNormal;
	outCamPosition = camPosition;
}
