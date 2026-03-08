struct StructSphere
{
    vec4  color;
	float radius;
	uint  isSelected;
	uint  id;
    vec3  viewPos; // Sphere position in view space.
    float dotViewPos;
};