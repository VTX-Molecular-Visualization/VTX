#version 450

struct DisplayFullCircle
{
	vec4  firstAtom;  // ith pos + ith radius
	vec4  secondAtom; // jth pos + jth radius
	vec4  center;	  // Circle center + circle radius
	vec4  normal;	  // normal + isExterior
	vec3  bbPos;
	vec3  bbDim;
	vec4  rot;
	vec4  vSphere;
};

layout(location = 0) in uvec2 atomsId;

flat out vec3			   vImpU; // Impostor vectors.
flat out vec3			   vImpV;
flat out DisplayFullCircle vCircle;

layout(std140, binding=0) uniform SesdfSettings
{
	mat4  uMVMatrix;
	mat4  uProjMatrix;
	mat4  uInvMVMatrix;
	float uProbeRadius;
	uint  uMaxProbeNeighborNb;
};

layout(std140, binding = 1) readonly buffer SortedAtoms {
	vec4 atoms[];
};

float length2(vec3 v){return dot(v,v);}

vec3 computeCircleCenter( vec4 c1, vec4 c2 )
{
	c1.w += uProbeRadius;
	c2.w += uProbeRadius;
	const float d = length2( c1.xyz - c2.xyz );
	const float t = ( c1.w * c1.w - c2.w * c2.w + d ) / ( 2.f * d );
	return c1.xyz + ( c2.xyz - c1.xyz ) * t;
}

// Based on GLM implementation
vec3 quatMult(vec4 q, vec3 v)
{
	const vec3 qAxis = q.xyz;
	return 2. * dot( qAxis, v ) * qAxis + ( q.w * q.w - dot( qAxis, qAxis ) ) * v
			+ 2. * q.w * cross( qAxis, v );
}

vec4 toLocalSpaceTransform( const vec3 n )
{
    const vec3 ref = vec3( 0.f, 0.f, 1.f );
    if ( dot( n, ref ) < -1.f + 1e-4f )
        return vec4( 1.f, 0.f, 0.f, 0.f );

    float angle = 1.f + dot( n, ref ); 
    vec3 axis   = cross( n, ref );
    return normalize( vec4( axis, angle ) );
}

vec4 computeSmallCircle(vec4 atom, vec3 n, vec3 x)
{
    vec3 sCircle = atom.xyz + n * dot(n, normalize(x - atom.xyz) * atom.w);
    float sqCircleDistance = length2(atom.xyz - sCircle);
	float r = sqrt( atom.w * atom.w - sqCircleDistance );
    return vec4(sCircle, r);
}

vec3 orthogonalVector(vec3 normal) 
{
	vec3 ref = vec3(1., 0., 0.);
	if (abs(dot(normal, ref)) < 1e-4)
		ref = vec3(0., 1., 0.);
    return normalize(cross(normal, ref));
}

struct Bound
{
    vec3 pMin;
    vec3 pMax;
};
Bound getCircleBoundingBox(vec3 n)
{
	// https://iquilezles.org/articles/diskbbox/
	vec3 v = sqrt(1. - n*n );
	return Bound(-v, v);
}

void main()
{
	const vec4 atom1 = atoms[atomsId.x];
	const vec4 atom2 = atoms[atomsId.y];
	vCircle.firstAtom  = vec4(( uMVMatrix * vec4( atom1.xyz, 1.f ) ).xyz, atom1.w);
	const float ithExtendedRadius = vCircle.firstAtom.w + uProbeRadius;
	vCircle.secondAtom = vec4(( uMVMatrix * vec4( atom2.xyz, 1.f ) ).xyz, atom2.w);

	const vec3  circleCenter = computeCircleCenter(vCircle.firstAtom, vCircle.secondAtom);
	const vec3  circleToI    = vCircle.firstAtom.xyz - circleCenter;
	const float squaredCircleDistance = dot( circleToI, circleToI );
	const float radius = sqrt( ithExtendedRadius * ithExtendedRadius - squaredCircleDistance );

	vCircle.center = vec4( circleCenter, radius );
	vCircle.normal = vec4( normalize( vCircle.secondAtom.xyz - vCircle.firstAtom.xyz ), 0. );

	vCircle.rot = toLocalSpaceTransform(vCircle.normal.xyz);
	const Bound bound = Bound(-vec3(1., 1., 0.), vec3(1., 1., 0.)); // getCircleBoundingBox(vec3(0., 0., 1.));

	const vec3 x1p = circleCenter + orthogonalVector(vCircle.normal.xyz) * radius;
	const vec4 sCircle  = computeSmallCircle(vCircle.firstAtom,  vCircle.normal.xyz, x1p);
    const vec4 sCircle2 = computeSmallCircle(vCircle.secondAtom, vCircle.normal.xyz, x1p);
	
    const float rad = max(sCircle.w, sCircle2.w);
    vec3 sMin = min(bound.pMin * rad, bound.pMin * max(0., radius - uProbeRadius));
    vec3 sMax = max(bound.pMax * rad, bound.pMax * max(0., radius - uProbeRadius));

    const vec2 dim = abs((sMax - sMin).xy * .5);
    vCircle.bbDim = vec3(dim, length(sCircle.xyz - sCircle2.xyz) * .5);

    sMin = quatMult(vec4(-vCircle.rot.xyz, vCircle.rot.w), sMin);
    sMax = quatMult(vec4(-vCircle.rot.xyz, vCircle.rot.w), sMax);

	vCircle.bbPos = (sCircle.xyz + sCircle2.xyz) * .5 + (sMax + sMin) * .5;

	// Compute normalized view vector.
	const float dotViewSpherePos  = dot( circleCenter, circleCenter );
	const float dSphereCenter	  = sqrt( dotViewSpherePos );
	const vec3	view			  = circleCenter / dSphereCenter;

	vec3 p = x1p;
	vec3 x = normalize(p - vCircle.firstAtom.xyz) * vCircle.firstAtom.w;
	vec3 c = (length(p - vCircle.firstAtom.xyz) / (length(p - vCircle.secondAtom.xyz) + length(p - vCircle.firstAtom.xyz))) * (vCircle.secondAtom.xyz - vCircle.firstAtom.xyz);
	float d = length(x - c);
	c = c + vCircle.firstAtom.xyz;
	vCircle.vSphere = vec4(c, d);

	// Impostor in front of the sphere.
	const vec3 viewImpPos = vCircle.vSphere.xyz - vCircle.vSphere.w * view;

	// Compute impostor size.
	const float sinAngle = vCircle.vSphere.w / dSphereCenter;
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

	gl_Position = vec4( viewImpPos, 1.f );}
