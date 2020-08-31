#version 450

layout( vertices = 3 ) out;

uniform vec3 uCamPosition;

in vec3	 vPositionVS[];

out vec3 vPositionTC[];

float getTessLevel(float p_distance0, float p_distance1)
{
    float avgDistance = (p_distance0 + p_distance1) / 2.0;

    if (avgDistance <= 2.0) {
        return 10.0;
    }
    else if (avgDistance <= 5.0) {
        return 7.0;
    }
    else {
        return 3.0;
    }
}

void main()
{
	vPositionTC[ gl_InvocationID ] = vPositionVS[ gl_InvocationID ];

	float camToVertexDistance0 = distance( uCamPosition, vPositionVS[ 0 ] );
	float camToVertexDistance1 = distance( uCamPosition, vPositionVS[ 1 ] );
	float camToVertexDistance2 = distance( uCamPosition, vPositionVS[ 2 ] );

	gl_TessLevelOuter[ 0 ] = getTessLevel( camToVertexDistance1, camToVertexDistance2 );
	gl_TessLevelOuter[ 1 ] = getTessLevel( camToVertexDistance2, camToVertexDistance0 );
	gl_TessLevelOuter[ 2 ] = getTessLevel( camToVertexDistance0, camToVertexDistance1 );
	gl_TessLevelInner[ 0 ] = gl_TessLevelOuter[ 2 ];
}
