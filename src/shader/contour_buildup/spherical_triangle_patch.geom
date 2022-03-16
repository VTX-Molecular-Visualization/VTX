#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

layout(std140) uniform SphericalTrianglePatchSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	uint uProbeNb;
	uint uMaxProbeNeighborNb;
};

in VERT_OUT {
	flat vec3  impU;
	flat vec3  impV;
	flat float dotViewSpherePos;
	flat vec3  wsProbePos;
	flat vec4  probePos;
	flat int   probeId;
	flat float probeIntersectionNumber;
	flat vec3  vert1;
	flat vec3  vert2;
	flat vec3  vert3;
} inData[];

out GEOM_OUT 
{
	smooth vec3  viewImpPos;	  // Impostor position in view space.
	flat   float dotViewSpherePos;
	flat   vec3  wsProbePos;
	flat   vec4  probePos;
	flat   int	 probeId;
	flat   float probeIntersectionNumber;
	flat   vec3  vert1;
	flat   vec3  vert2;
	flat   vec3  vert3;
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
	outData.dotViewSpherePos		= inData[ 0 ].dotViewSpherePos;
	outData.wsProbePos				= inData[ 0 ].wsProbePos;
	outData.probePos				= inData[ 0 ].probePos;
	outData.probeId				    = inData[ 0 ].probeId;
	outData.probeIntersectionNumber = inData[ 0 ].probeIntersectionNumber;
	outData.vert1					= inData[ 0 ].vert1;
	outData.vert2					= inData[ 0 ].vert2;
	outData.vert3					= inData[ 0 ].vert3;

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - inData[ 0 ].impU - inData[ 0 ].impV;
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + inData[ 0 ].impU - inData[ 0 ].impV;
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - inData[ 0 ].impU + inData[ 0 ].impV;
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + inData[ 0 ].impU + inData[ 0 ].impV;

	emitQuad( v1, v2, v3, v4 );
}
