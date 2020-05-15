#version 450

uniform mat4  uProjMatrix;
uniform float uCylRad;

smooth in vec3 impPos;
flat in vec3   cylCenter;
flat in vec3   cylVert[2];
flat in vec3   colors[ 2 ];

// 3 16 bits for color
// 3 16 bits for normal
// 1 32 bits for padding
layout( location = 0 ) out uvec4 outColorNormal;
// 3 32 bits for position in cam space
// 1 32 bits for specular
layout( location = 1 ) out vec4 outCamPosition;

// only consider cylinder body
void main()
{
	const vec3 v1v0	  = cylVert[1] - cylVert[0];
	const vec3 v0	  = -cylVert[0];
	const vec3 rayDir = normalize( impPos );

	const float d0 = dot( v1v0, v1v0 );
	const float d1 = dot( v1v0, rayDir );
	const float d2 = dot( v1v0, v0 );

	const float a = d0 - d1 * d1;
	const float b = d0 * dot( v0, rayDir ) - d2 * d1;
	const float c = d0 * dot( v0, v0 ) - d2 * d2 - uCylRad * uCylRad * d0;

	const float h = b * b - a * c;

	if ( h < 0.f ) { discard; }
	else
	{
		// consider only the first intersection
		const float t = ( -b - sqrt( h ) ) / a;

		const float y = d2 + t * d1;

		if ( y < 0.f || y > d0 ) { discard; }
		else
		{
			const vec3 camHit = rayDir * t;
			const vec2 clipZW = camHit.z * uProjMatrix[ 2 ].zw + uProjMatrix[ 3 ].zw;

			gl_FragDepth
				= ( ( gl_DepthRange.diff * ( clipZW.x / clipZW.y ) ) + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;

			const vec3 normal = normalize( ( v0 + camHit - v1v0 * y / d0 ) / uCylRad );

			const vec3 color = colors[ int( y > d0 * 0.5f ) ];

			uvec4 colorNormal;
			vec4  camPosition;
			// fill G-buffers
			colorNormal.x = packHalf2x16( color.xy );
			colorNormal.y = packHalf2x16( vec2( color.z, normal.x ) );
			colorNormal.z = packHalf2x16( normal.yz );
			colorNormal.w = 0; // padding

			camPosition.xyz = camHit;
			camPosition.w	= 60.f; // specular // TODO materials...

			outColorNormal = colorNormal;
			outCamPosition = camPosition;
		}
	}
}
