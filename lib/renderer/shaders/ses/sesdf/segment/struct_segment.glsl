struct StructSegment
{
	uint model;
	uint representation;
	uint selection;
	vec4 color;
	vec3 v1;
	vec3 v2;
	vec4 startAtom;
	vec4 circle; // circle center + radius
	vec4 normal; // normal + segment angle
	vec3 bbPos;
	vec3 bbDim;
	vec4 rot;
	vec4 vSphere;
};
