#version 450

in TeOut
{
	vec3				viewPosition;
	vec3				normal;
	vec3				color;
	flat unsigned short selection;
}
teIn;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

void main()
{
	vec3 normal = teIn.normal;
	vec3 color  = teIn.color;
	if ( dot(normal, teIn.viewPosition) > 0.f )
	{
		normal = -normal;
	}

	// Compress color and normal.
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( teIn.viewPosition.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( teIn.viewPosition.z, normal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
	viewPositionNormalCompressed.w = packHalf2x16( vec2( teIn.selection, 0 ) );

	// Output data.
	outViewPositionNormal = viewPositionNormalCompressed;
	outColor			  = vec4( color, 32.f ); // w = specular shininess.
}
