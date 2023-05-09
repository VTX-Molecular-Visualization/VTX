#version 450

in
#include "struct_tessellation_evaluation_shader.glsl"
dataIn;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;
layout( location = 2 ) out uvec2 outId;

void main()
{
	// TODO: redo it in tessellation shader.
	if ( dataIn.visibility == 0 )
	{
		discard;
	}

	vec3 normal = dataIn.normal;
	vec4 color	= dataIn.color;
	if ( dot( normal, dataIn.viewPosition ) > 0.f )
	{
		normal = -normal;
	}

	// Compress position and normal.
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( dataIn.viewPosition.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( dataIn.viewPosition.z, normal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
	viewPositionNormalCompressed.w = packHalf2x16( vec2( dataIn.selection, 0 ) );

	// Output data.
	outViewPositionNormal = viewPositionNormalCompressed;
	outColor			  = vec4( color.xyz, 32.f ); // w = specular shininess.
	outId				  = uvec2( dataIn.id, 0 );
}
