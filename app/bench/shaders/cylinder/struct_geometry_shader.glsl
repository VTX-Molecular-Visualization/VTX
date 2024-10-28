StructGeometryShader
{
	smooth vec3 viewImpostorPosition; // Impostor position in view space.
	flat vec3	viewVertices[ 2 ];	  // Cylinder vertices position in view space.
	flat vec4	colors[ 2 ];
	flat uint	vertexSelected[ 2 ];
	flat uint	vertexId[ 2 ];
	flat uint	vertexIdRepresentation[ 2 ];
}