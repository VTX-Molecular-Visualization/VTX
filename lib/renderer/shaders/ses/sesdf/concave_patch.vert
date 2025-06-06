#version 450

struct Plane 
{
	vec3 n;
	float d;
};

struct DisplayTetrahedron
{
	vec4 point;			  // (x, y, z): coordinate of the point
	Plane plane1;
	Plane plane2;
	Plane plane3;
	int  startNeighborId;
	int  neighborNb;
	vec3 color;
};

flat out vec3 vImpU; // Impostor vectors.
flat out vec3 vImpV;
flat out DisplayTetrahedron vTetrahedron;

layout(std140, binding=0) uniform SesdfSettings
{
	mat4  uMVMatrix;
	mat4  uProjMatrix;
	mat4  uInvMVMatrix;
	float uProbeRadius;
	uint  uMaxProbeNeighborNb;
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

//https://www.shadertoy.com/view/Xt3cDn
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

float length2(vec3 v) { return dot(v,v); }

void main()
{
	const vec4 intersectionCenter   = probes[gl_VertexID];
	const ivec4 intersectionIndices = probesAtomIndices[gl_VertexID];
	
	vTetrahedron.point.w   = gl_VertexID;
	vTetrahedron.point.xyz = ( uMVMatrix * vec4( intersectionCenter.xyz, 1.f ) ).xyz;

	vTetrahedron.startNeighborId = int(gl_VertexID * uMaxProbeNeighborNb);
	vTetrahedron.neighborNb		 = int(intersectionIndices.w);
	
	vec4 atom1 = atoms[intersectionIndices.x];
	atom1 = vec4(( uMVMatrix * vec4( atom1.xyz, 1.f ) ).xyz, atom1.w);
	vec4 atom2 = atoms[intersectionIndices.y];
	atom2 = vec4(( uMVMatrix * vec4( atom2.xyz, 1.f ) ).xyz, atom2.w);
	vec4 atom3 = atoms[intersectionIndices.z];
	atom3 = vec4(( uMVMatrix * vec4( atom3.xyz, 1.f ) ).xyz, atom3.w);
	
	vTetrahedron.color = vec3(1.);//vec3(hash31(intersectionIndices.x) + hash31(intersectionIndices.y) + + hash31(intersectionIndices.z)) / 3.;

	vec3 p1 = vTetrahedron.point.xyz;
	vec3 p2 = atom1.xyz;
	vec3 p3 = atom2.xyz;
	vec3 p4 = atom3.xyz;
	
	vec3 center = (p2 + p3 + p4) / 3.;
	vec3 dirToCenter = normalize(center - p1);

	vec3 p2p1 = normalize(p2 - p1);
	vec3 p3p1 = normalize(p3 - p1);
	vec3 p4p1 = normalize(p4 - p1);

	vec3 n1 = cross( p2p1, p3p1 );
	vec3 n2 = cross( p2p1, p4p1 );
	vec3 n3 = cross( p3p1, p4p1 );

	n1 *= sign(dot(dirToCenter, n1)) ;
	n2 *= sign(dot(dirToCenter, n2)) ;
	n3 *= sign(dot(dirToCenter, n3)) ;

	vTetrahedron.plane1 = Plane(n1, dot(p1, n1));
	vTetrahedron.plane2 = Plane(n2, dot(p1, n2)); 
	vTetrahedron.plane3 = Plane(n3, dot(p1, n3));

	// Impostor in front of the sphere.
	vec3 vViewCenter			 = p1;
	const float dotViewSpherePos = dot( vViewCenter, vViewCenter );
	const float dSphereCenter	 = sqrt( dotViewSpherePos );
	const vec3	view			 = vViewCenter / dSphereCenter;

	const float sphereRad = uProbeRadius;
	const vec3 viewImpPos = vViewCenter - sphereRad * view;

	// Compute impostor size.
	const float sinAngle = sphereRad / dSphereCenter;
	const float tanAngle = tan( asin( sinAngle ) );
	const float impSize	 = tanAngle * length( viewImpPos );

	// Compute impostor vectors.
	// TODO: simplify normalize ? (vImpU.x == 0) but normalize should be hard optimized on GPU...
	// But for cross always better doing no calculation.
	// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
	vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
	// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
	vImpV = cross( vImpU, view ) * impSize; // No need to normalize.
	vImpU *= impSize;

	gl_Position = vec4( viewImpPos, 1.f );
}
