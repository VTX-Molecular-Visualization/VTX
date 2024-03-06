struct Representation
{
	float radiusSphereFixed;
	float radiusSphereAdd;
	uint  isRadiusSphereFixed;
	float radiusCylinder;

	uint cylinderColorBlendingMode;
	uint ribbonColorBlendingMode;
	float padding[ 2 ];
};

layout( std140, binding = 12 ) uniform UniformsRepresentation
{
   Representation uniformsRepresentation[ 256 ];
};