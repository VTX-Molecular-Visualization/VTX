#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4 uProjMatrix;

in VERT_OUT 
{
	flat vec3  impU;
	flat vec3  impV;
	flat vec3  torusPosition;
	flat vec4  radii;
	flat mat3  torusRotationMatrix; 
	flat vec4  osVisibilitySphere; 
} inData[];

out GEOM_OUT 
{
	smooth vec3  viewImpPos;
	flat   vec3  viewTorusPosition;
	flat   vec4  radii;
	flat   mat3  viewTorusRotationMatrix; 
	flat   vec4  osVisibilitySphere; 
} outData;

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	outData.viewImpPos	= v1;
	gl_Position = uProjMatrix * vec4( outData.viewImpPos, 1.f );
	EmitVertex();

	outData.viewImpPos	= v2;
	gl_Position = uProjMatrix * vec4( outData.viewImpPos, 1.f );
	EmitVertex();

	outData.viewImpPos	= v3;
	gl_Position = uProjMatrix * vec4( outData.viewImpPos, 1.f );
	EmitVertex();

	outData.viewImpPos	= v4;
	gl_Position = uProjMatrix * vec4( outData.viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Output data.
	outData.viewTorusPosition		= inData[ 0 ].torusPosition;
	outData.radii					= inData[ 0 ].radii;
	outData.viewTorusRotationMatrix = inData[ 0 ].torusRotationMatrix;
	outData.osVisibilitySphere      = inData[ 0 ].osVisibilitySphere;

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - inData[ 0 ].impU - inData[ 0 ].impV;
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + inData[ 0 ].impU - inData[ 0 ].impV;
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - inData[ 0 ].impU + inData[ 0 ].impV;
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + inData[ 0 ].impU + inData[ 0 ].impV;

	emitQuad( v1, v2, v3, v4 );
}
