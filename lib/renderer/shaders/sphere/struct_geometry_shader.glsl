StructGeometryShader
{
	smooth vec3 viewImpPos;	   // Impostor position in view space.
	flat vec3	viewSpherePos; // Sphere position in view space.
	flat vec4	sphereColor;
	flat float	sphereRadius;
	flat float	dotViewSpherePos;
	flat uint	sphereSelected;
	flat uint	sphereId;
}