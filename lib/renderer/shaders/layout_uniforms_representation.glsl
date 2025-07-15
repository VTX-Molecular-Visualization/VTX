struct Representation
{
	float radiusSphereFixed;
	float radiusSphereAdd;
	uint  isRadiusSphereFixed;
	float radiusCylinder;
	uint cylinderColorBlendingMode;
	uint ribbonColorBlendingMode;
	float SESProbeRadius;
	uint  SESMaxProbeNeighborNb;
};

layout( std430, binding = 12 ) readonly buffer UniformsRepresentation
{
   Representation uniformsRepresentation[];
};