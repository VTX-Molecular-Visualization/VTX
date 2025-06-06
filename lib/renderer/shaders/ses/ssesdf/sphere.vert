#version 450

struct Sphere
{
    vec3  position;
    float radius;
};

layout(std140, binding = 0) uniform SsesdfSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	float uProbeRadius;
};

layout(std140, binding = 1) readonly buffer Spheres {
	Sphere spheres[];
};

flat out vec3  vViewSpherePos; // Sphere position in view space.
flat out vec3  vSphereColor;
flat out float vSphereRad;
flat out uint  vVisible;
flat out vec3  vImpU; // Impostor vectors.
flat out vec3  vImpV;
flat out float vDotViewSpherePos;

void main()
{
	Sphere sphere  = spheres[gl_VertexID];
	vViewSpherePos = vec3( uMVMatrix * vec4( sphere.position, 1. ) );
	vSphereColor   = vec3(1.);
	vSphereRad	   = sphere.radius;
	vVisible	   = 1u;

	// Compute normalized view vector.
	vDotViewSpherePos		  = dot( vViewSpherePos, vViewSpherePos );
	const float dSphereCenter = sqrt( vDotViewSpherePos );
	const vec3	view		  = vViewSpherePos / dSphereCenter;

	// Impostor in front of the sphere.
	vec3 viewImpPos = vViewSpherePos - vSphereRad * view;

	// Compute impostor size.
	const float sinAngle = vSphereRad / dSphereCenter;
	const float tanAngle = tan( asin( sinAngle ) );
	const float impSize	 = tanAngle * length( viewImpPos );

	// Compute impostor vectors.
	// TODO: simplify normalize ? (vImpU.x == 0) but normalize should be hard optimized on GPU...
	// But for cross always better doing no calculation.
	// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
	vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
	// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
	vImpV = cross( vImpU, view ) * impSize; // No need to normalize.
	vImpU *= impSize;

	gl_Position = vec4( viewImpPos, 1.f );
}
