struct StructTetrahedron
{
	uint model;
	vec4 point;			  // (x, y, z): coordinate of the point
	Plane plane1;
	Plane plane2;
	Plane plane3;
	int  startNeighborId;
	int  neighborNb;
	uint selection;
	vec4 color;
};
