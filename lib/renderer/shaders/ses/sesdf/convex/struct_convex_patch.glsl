struct StructConvexPatch
{
	uint  model;
	uint  representation;
	uint  atomId;
	uint  rendererAtomId;
	uint  selection;
	vec4  color;
	vec4  wsAtomData; // world space ith pos + ith radius
	vec4  vAtomData;  // View space ith pos + ith Extended radius
	uvec2 elementsId;
};
