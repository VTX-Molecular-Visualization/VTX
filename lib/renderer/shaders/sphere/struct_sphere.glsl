struct StructSphere
{
    vec4  color;
	float radius;
	uint  isVisible;
	uint  isSelected;
	uint  id;
    vec3  viewPos; // Sphere position in view space.
    float dotViewPos;
};