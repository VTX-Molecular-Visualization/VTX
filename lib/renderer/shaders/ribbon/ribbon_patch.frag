#version 450 core

#include "../struct_data_packed.glsl"

// In.
in
#include "struct_tessellation_evaluation_shader.glsl"
inData;

 // Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;
layout( location = 2 ) out uvec2 outId;

void main()
{
	// TODO: redo it in tessellation shader.
	if ( inData.visibility == 0 )
	{
		discard;
	}

	vec3 normal = inData.normal;
	if ( dot( normal, inData.viewPosition ) > 0.f )
	{
		normal = -normal;
	}

	// Output data.
	packData( inData.viewPosition, normal, inData.selection, outDataPacked );
	outColor			  = inData.color; // w = specular shininess.
	outId				  = uvec2( inData.id, 0 );
}
