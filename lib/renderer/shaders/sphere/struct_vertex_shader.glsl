StructVertexShader
{
	flat vec3  viewSpherePos; // Sphere position in view space.
	flat vec4  sphereColor;
	flat float sphereRadius;
	flat bool  sphereVisible;
	flat bool  sphereSelected;
	flat uint  sphereId;
	flat vec3  vImpU; // Impostor vectors.
	flat vec3  vImpV;
	flat float dotViewSpherePos;
}