StructVertexShader
{
    flat vec3 vImpU; // Impostor vectors.
    flat vec3 vImpV;
    flat vec4 firstAtom;  // ith pos + ith radius
	flat vec4 secondAtom; // jth pos + jth radius
	flat vec4 center;	  // Circle center + circle radius
	flat vec4 normal;	  // normal + isExterior
	flat vec3 bbPos;
	flat vec3 bbDim;
	flat vec4 rot;
	flat vec4 vSphere;
}