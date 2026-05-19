struct StructConvexPatch
{
	uint model;
	uint atomId;
	vec4 wsAtomData;	 // world space ith pos + ith radius
	vec4 vAtomData;		 // View space ith pos + ith Extended radius
	uvec2 elementsId;
};
