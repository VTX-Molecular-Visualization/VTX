struct StructTetrahedron
{
	vec4 point;			  // (x, y, z): coordinate of the point
	Plane plane1;
	Plane plane2;
	Plane plane3;
	int  startNeighborId;
	int  neighborNb;
	vec3 color;
};