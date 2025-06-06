#version 450

layout(std140, binding=0) uniform SesdfSettings
{
	mat4 uMVPMatrix;
};

layout(std140, binding = 1) readonly buffer SortedAtoms {
	vec4 atoms[];
};

layout(std140, binding = 3) readonly buffer Probes {
	vec4 probes[];
};

layout(std140, binding = 4) readonly buffer ProbesAtomIndices {
	ivec4 probesAtomIndices[];
};

flat out vec3 vPoint1;
flat out vec3 vPoint2;
flat out vec3 vPoint3;
flat out vec3 vPoint4;
flat out vec3 vColor;

// Reference: https://www.shadertoy.com/view/Xt3cDn
uint baseHash(uint p)
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint h32 = p + PRIME32_5;
	h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17))); //Initial testing suggests this line could be omitted for extra perf
    h32 = PRIME32_2*(h32^(h32 >> 15));
    h32 = PRIME32_3*(h32^(h32 >> 13));
    return h32^(h32 >> 16);
}

vec3 hash31(uint x)
{
    uint n = baseHash(x);
    uvec3 rz = uvec3(n, n*16807U, n*48271U); //see: http://random.mat.sbg.ac.at/results/karl/server/node4.html
    return vec3((rz >> 1) & uvec3(0x7fffffffU))/float(0x7fffffff);
}

void main()
{
	const ivec4 intersectionIndices = probesAtomIndices[gl_VertexID];
	
	vec4 intersectionCenter = probes[gl_VertexID];
	intersectionCenter.xyz  = ( vec4( intersectionCenter.xyz, 1.f ) ).xyz;

	vec4 atom1 = atoms[intersectionIndices.x];
	vec4 atom2 = atoms[intersectionIndices.y];
	vec4 atom3 = atoms[intersectionIndices.z];
	
	vColor = vec3(hash31(intersectionIndices.x) + hash31(intersectionIndices.y) + hash31(intersectionIndices.z)) / 3.;
	vPoint1 = atom1.xyz;
	vPoint2 = atom2.xyz;
	vPoint3 = atom3.xyz;
	vPoint4 = intersectionCenter.xyz;

	gl_Position = uMVPMatrix * vec4((vPoint1 + vPoint2 + vPoint3 + vPoint4) * .25, 1.);
}