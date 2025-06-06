#version 450

//#define SHOW_IMPOSTORS

layout (depth_greater) out float gl_FragDepth;

layout(std140, binding = 0) uniform SsesdfSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	float uProbeRadius;
};

smooth in vec3  viewImpPos;
flat   in vec3  viewTorusPosition;
flat   in vec4  radii;
flat   in mat3  viewTorusRotationMatrix; 
flat   in vec4  osVisibilitySphere; 

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * uProjMatrix[ 2 ].z + uProjMatrix[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

// global constants
float doubtol = 0.00000001;      // min
float doubmin = 0.0;             // zero
float doubmax = 1000000000.0;    // max (inf)

// From https://github.com/UniStuttgart-VISUS/megamol/blob/master/plugins/protein_cuda/shaders/protein_cuda.btf
/**
*    solve the quadratic equation
*        x**2+b*x+c = 0
*
*    input:
*        b, c - coefficients of equation.
*    output:
*        [return nquad] - number of real roots.
*        rts - array of root values.+
*        
*    called by:
*        ferrari
*        neumark
*/
int quadratic( float b, float c, out vec4 rts, float dis)
{
    int nquad;
    float rtdis;
    if( dis >= 0.0 )
    {
        nquad = 2;
        rtdis = sqrt( dis) ;
        if( b > 0.0 )
            rts.x = ( -b - rtdis) * 0.5;
        else
            rts.x = ( -b + rtdis) * 0.5;
        if( rts.x == 0.0 )
            rts.y = -b;
        else
            rts.y = c/rts.x;
    }
    else
    {
        nquad = 0;
        rts.x = 0.0;
        rts.y = 0.0;
    }
    return nquad;
} // ===== quadratic =====

/** 
*    find cube root of x.
*
*    called by:
*        cubic 
*/
float curoot( float x)
{
    float value;
    float absx;
    int neg;
    neg = 0;
    absx = x;
    if( x < 0.0 )
    {
        absx = -x;
        neg = 1;
    }
    value = exp( log( absx)*(1.0/3.0));
    if( neg == 1 )
        value = -value;
    return value;
} // ===== curoot =====

/** 
* find cos(acos(x)/3) 
*
* called by:
*    cubic 
*/
float acos3( float x)
{
    return cos( acos( x)*(1.0/3.0));
} // ===== acos3 =====


/**
*    find the lowest real root of the cubic equation
*        x**3 + p*x**2 + q*x + r = 0 
*
*    input parameters:
*        p, q, r - coefficients of cubic equation. 
*    output:
*        cubic - a real root.
*
*    calls:
*        acos3
*        curoot
*    called by:
*        ferrari
*        neumark
*/
float cubic( float p, float q, float r)
{    
    int nrts;
    float po3, po3sq, qo3;
    float uo3, u2o3, uo3sq4, uo3cu4;
    float v, vsq, wsq;
    float m, mcube, n;
    float muo3, s, scube, t, cosk, sinsqk;
    float root;
    m = 0.0;
    nrts = 0;
    if( (p > doubmax) || (p <  -doubmax) )
        root = -p;
    else
    {
        if( (q > doubmax) || (q <  -doubmax) )
        {
            if (q > 0.0)
                root = -r/q;
            else
                root = -sqrt( -q);
        }
        else
        {
            if( (r > doubmax) || (r <  -doubmax) )
                root = -curoot( r);
            else
            {
                po3 = p * (1.0/3.0);
                po3sq = po3*po3 ;
                if( po3sq > doubmax )
                    root = -p;
                else
                {
                    v = r + po3*(po3sq + po3sq - q) ;
                    if( (v > doubmax) || (v < -doubmax) )
                        root = -p;
                    else
                    {
                        vsq = v*v ;
                        qo3 = q * (1.0/3.0);
                        uo3 = qo3 - po3sq ;
                        u2o3 = uo3 + uo3 ;
                        if( (u2o3 > doubmax) || (u2o3 < -doubmax) )
                        {
                            if (p == 0.0)
                            {
                                if (q > 0.0)
                                    root = -r/q;
                                else
                                    root = -sqrt( -q);
                            }
                            else
                                root = -q/p;
                        }
                        uo3sq4 = u2o3 * u2o3 ;
                        if( uo3sq4 > doubmax)
                        {
                            if (p == 0.0)
                            {
                                if( q > 0.0 )
                                    root = -r/q;
                                else
                                    root = -sqrt( abs( q));
                            }
                            else
                                root = -q/p;
                        }
                        uo3cu4 = uo3sq4 * uo3;
                        wsq = uo3cu4 + vsq;
                        if( wsq >= 0.0 )
                        {
                            // cubic has one real root
                            nrts = 1;
                            if( v <= 0.0 )
                                mcube = ( -v + sqrt( wsq))*0.5;
                            if( v  > 0.0 )
                                mcube = ( -v - sqrt( wsq))*0.5;
                            m = curoot( mcube);
                            if( m != 0.0 )
                                n = -uo3/m;
                            else
                                n = 0.0;
                            root = m + n - po3;
                        }
                        else
                        {
                            nrts = 3;
                            // cubic has three real roots
                            if( uo3 < 0.0 )
                            {
                                muo3 = -uo3;
                                s = sqrt( muo3);
                                scube = s*muo3;
                                t =  -v/(scube+scube);
                                cosk = acos3( t);
                                if( po3 < 0.0 )
                                    root = (s+s)*cosk - po3;
                                else
                                {
                                    sinsqk = 1.0 - cosk*cosk;
                                    if( sinsqk < 0.0 )
                                        sinsqk = 0.0;
                                    root = s*( -cosk - sqrt( 3.0)*sqrt( sinsqk)) - po3;
                                }
                            }
                            else
                                // cubic has multiple root
                                root = curoot( v) - po3;
                        }
                    }
                }
            }
        }
    }
    return root;
} // ===== cubic =====

/**
*    solve the quartic equation
*        x**4 + a*x**3 + b*x**2 + c*x + d = 0
*
*    input:
*        a, b, c, d - coefficients of equation.
*    output:
*        [return nquar] - number of real roots.
*        rts - array of root values.
*
*    calls:
*        cubic
*        quadratic
*/
int ferrari( float a, float b, float c, float d, out vec4 rts)
{
    rts = vec4( 0.0, 0.0, 0.0, 0.0);
    
    int nquar, n1, n2;
    float asq, ainv2;
    vec4 v1, v2;
    float p, q, r;
    float y;
    float e, f, esq, fsq, ef;
    float g, gg, h, hh;
    asq = a*a;
    p = b;
    q = a * c - 4.0 * d;
    r = ( asq - 4.0 * b) * d + c*c;
    y = cubic( p, q, r);
    esq = 0.25 * asq - b - y;
    if( esq < 0.0 )
        return(0);
    else
    {
        fsq = 0.25*y*y - d;
        if( fsq < 0.0 )
            return 0;
        else
        {
            ef = -(0.25*a*y + 0.5*c);
            if( ((a > 0.0) && (y > 0.0) && (c > 0.0))
                || ((a > 0.0) && (y < 0.0) && (c < 0.0))
                || ((a < 0.0) && (y > 0.0) && (c < 0.0))
                || ((a < 0.0) && (y < 0.0) && (c > 0.0))
                ||  (a == 0.0) || (y == 0.0) || (c == 0.0) )
            // use ef
            {
                if( (b < 0.0) && (y < 0.0) && (esq > 0.0) )
                {
                    e = sqrt( esq);
                    f = ef/e;
                }
                else if( (d < 0.0) && (fsq > 0.0) )
                {
                    f = sqrt( fsq);
                    e = ef/f;
                }
                else
                {
                    e = sqrt( esq);
                    f = sqrt( fsq);
                    if( ef < 0.0 ) f = -f;
                }
            }
            else
            {
                e = sqrt( esq);
                f = sqrt( fsq);
                if( ef < 0.0 ) f = -f;
            }
            // note that e >= 0.0
            ainv2 = a*0.5;
            g = ainv2 - e;
            gg = ainv2 + e;
            if( ((b > 0.0) && (y > 0.0))
                || ((b < 0.0) && (y < 0.0)) )
            {
                if( ( a > 0.0) && (e != 0.0) )
                    g = (b + y)/gg;
                else if( e != 0.0 )
                    gg = (b + y)/g;
            }
            if( (y == 0.0) && (f == 0.0) )
            {
                h = 0.0;
                hh = 0.0;
            }
            else if( ((f > 0.0) && (y < 0.0))
                || ((f < 0.0) && (y > 0.0)) )
            {
                hh = -0.5*y + f;
                h = d/hh;
            }
            else
            {
                h = -0.5*y - f;
                hh = d/h;
            }
            n1 = quadratic( gg, hh, v1, gg*gg - 4.0 * hh);
            n2 = quadratic( g, h, v2, g*g - 4.0 * h);
            nquar = n1 + n2;
            rts.x = v1.x;
            rts.y = v1.y;
            if( n1 == 0 )
            {
                rts.x = v2.x;
                rts.y = v2.y;
            }
            else
            {
                rts.z = v2.x;
                rts.w = v2.y;
            }
            return nquar;
        }
    }
} // ===== ferrari =====

void main()
{
    // Strongly based on the Megamol project implementation
    // https://github.com/UniStuttgart-VISUS/megamol/blob/master/plugins/protein_cuda/shaders/protein_cuda.btf#
    #define r radii.x
    #define r2 radii.y
    #define R radii.z
    #define R2 radii.w
    
    vec3 rayDirection = normalize(viewImpPos);

    // Ray base point
    // Get close to the torus to limit approximation errors;
    vec3 rayOrigin = viewImpPos.xyz - viewTorusPosition - (R + r) * rayDirection;

    rayOrigin = viewTorusRotationMatrix * rayOrigin;
    rayDirection = viewTorusRotationMatrix * rayDirection;

    const float K = dot( rayOrigin, rayOrigin ) - ( R2 + r2 );
    const float A = 4.0 * dot( rayOrigin, rayDirection );
    const float B = 2.0 * ( 2.0 * dot( rayOrigin, rayDirection ) * dot( rayOrigin, rayDirection ) + K + 2.0 * R2 * rayDirection.z * rayDirection.z );
    const float C = 4.0 * ( K * dot( rayOrigin, rayDirection ) + 2.0 * R2 * rayOrigin.z * rayDirection.z );
    const float D = K*K + 4.0 * R2 * ( rayOrigin.z*rayOrigin.z - r2 );
    
    vec4 lambdas = vec4( 0.0, 0.0, 0.0, 0.0 );
    const int numRoots = ferrari( A, B, C, D, lambdas );
    
    if( numRoots < 2 ) { discard; }
    
    float second = lambdas.x;
    vec3 intersection = rayOrigin + rayDirection * second;
    
    if( r > R )
    {
        if( dot( intersection, intersection ) < r2 - R2 )
        {
            second = lambdas.x;
            if( lambdas.y > second && numRoots > 1 ) { second = lambdas.y; }
            if( lambdas.z > second && numRoots > 2 ) { second = lambdas.z; }
            if( lambdas.w > second && numRoots > 3 ) { second = lambdas.w; }
            intersection = rayOrigin + rayDirection * second;
        }
    }

    const float intersectionVSphereDistance = length( intersection - osVisibilitySphere.xyz );
    if( intersectionVSphereDistance > osVisibilitySphere.w  )
    {
#ifdef SHOW_IMPOSTORS
        // Show impostors for debugging purpose.
        // fill G-buffers.
        uvec4 viewPositionNormalCompressed;
        viewPositionNormalCompressed.x = packHalf2x16( viewImpPos.xy );
        viewPositionNormalCompressed.y = packHalf2x16( vec2( viewImpPos.z, -viewImpPos.x ) );
        viewPositionNormalCompressed.z = packHalf2x16( -viewImpPos.yz );
        viewPositionNormalCompressed.w = 0; // Padding.

        // Output data.
        outViewPositionNormal = viewPositionNormalCompressed;
        outColor              = vec4(intersectionVSphereDistance, 0., 0., 32.f ); // w = specular shininess.

        gl_FragDepth = computeDepth( viewImpPos );
#else
        discard;
#endif
    }
    else 
    {
        const mat3 transposedVTorusRotMatrix = transpose(viewTorusRotationMatrix);

        // https://www.shadertoy.com/view/4sBGDy
        vec3 normal = normalize(intersection * (dot(intersection, intersection) - r2 - R2 * vec3( 1.0, 1.0, -1.0 )));

        // Transfer back normal to view space
        normal = transposedVTorusRotMatrix * normal;
        // invert normal since we want to render the inside of the torus
        normal = -normalize(normal);

        // Transfer back to view space
        intersection = transposedVTorusRotMatrix * intersection;
        intersection += viewTorusPosition.xyz;

        gl_FragDepth = computeDepth( intersection );

        // Compress position and normal.
        uvec4 viewPositionNormalCompressed;
        viewPositionNormalCompressed.x = packHalf2x16( intersection.xy);
        viewPositionNormalCompressed.y = packHalf2x16( vec2( intersection.z, normal.x ) );
        viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
        viewPositionNormalCompressed.w = 0; // Padding.

        // Output data.
        outViewPositionNormal = viewPositionNormalCompressed;
        outColor			  = vec4( 1., 1., 1. , 32. ); // w = specular shininess.
    }
}
