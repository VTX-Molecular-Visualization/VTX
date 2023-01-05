#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4 u_projMatrix;

in VsOut
{
	flat vec3  viewSpherePos; // Sphere position in view space.
	flat vec4  sphereColor;
	flat float sphereRadius;
	flat uint  sphereVisible;
	flat uint  sphereSelected;
	flat uint  sphereId;
	flat vec3  vImpU; // Impostor vectors.
	flat vec3  vImpV;
	flat float dotViewSpherePos;
}
vsIn[];

out GsOut
{
	smooth vec3 viewImpPos;	   // Impostor position in view space.
	flat vec3	viewSpherePos; // Sphere position in view space.
	flat vec4	sphereColor;
	flat float	sphereRadius;
	flat float	dotViewSpherePos;
	flat uint	sphereSelected;
	flat uint	sphereId;
}
gsOut;

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	gsOut.viewImpPos = v1;
	gl_Position		 = u_projMatrix * vec4( gsOut.viewImpPos, 1.f );
	EmitVertex();

	gsOut.viewImpPos = v2;
	gl_Position		 = u_projMatrix * vec4( gsOut.viewImpPos, 1.f );
	EmitVertex();

	gsOut.viewImpPos = v3;
	gl_Position		 = u_projMatrix * vec4( gsOut.viewImpPos, 1.f );
	EmitVertex();

	gsOut.viewImpPos = v4;
	gl_Position		 = u_projMatrix * vec4( gsOut.viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Do not emit primitive if sphere is not visible.
	if ( vsIn[ 0 ].sphereVisible == 0 )
	{
		return;
	}

	// Output data.
	gsOut.viewSpherePos	   = vsIn[ 0 ].viewSpherePos;
	gsOut.sphereColor	   = vsIn[ 0 ].sphereColor;
	gsOut.sphereRadius	   = vsIn[ 0 ].sphereRadius;
	gsOut.dotViewSpherePos = vsIn[ 0 ].dotViewSpherePos;
	gsOut.sphereSelected   = vsIn[ 0 ].sphereSelected;
	gsOut.sphereId		   = vsIn[ 0 ].sphereId;

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vsIn[ 0 ].vImpU - vsIn[ 0 ].vImpV;
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vsIn[ 0 ].vImpU - vsIn[ 0 ].vImpV;
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vsIn[ 0 ].vImpU + vsIn[ 0 ].vImpV;
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vsIn[ 0 ].vImpU + vsIn[ 0 ].vImpV;

	emitQuad( v1, v2, v3, v4 );
}
