struct StructCircle
{
	vec4  firstAtom;  // ith pos + ith radius
	vec4  secondAtom; // jth pos + jth radius
	vec4  center;	  // Circle center + circle radius
	vec4  normal;	  // normal + isExterior
	vec3  bbPos;
	vec3  bbDim;
	vec4  rot;
	vec4  vSphere;
};