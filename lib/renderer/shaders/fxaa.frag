#version 450 core

// FXAA : Fast Approximated Anti-Aliasing
// adapted from: http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
// and Timothy Lottes' code (NVidia)

// In.
layout( binding = 0 ) uniform sampler2D inTexture;

// Out.
layout( location = 0 ) out vec4 outFragColor;

// the minimum amount of local contrast required to apply algorithm.
const float EDGE_THRESHOLD
	//= 1.f / 3.f; // too little
	// = 1.f / 4.f; // low quality
	= 1.f / 8.f; // high quality
				 //= 1.f / 16.f; // overkill

// trims the algorithm from processing darks.
const float EDGE_THRESHOLD_MIN
	//= 1.f / 32.f;			   // visible limit
	= 1.f / 16.f; // high quality
				  //= 1.f / 12.f; // upper limit (start of visible unfiltered edges)

// sub-pixel aliasing removal
const float QUALITY_SUBPIXEL
	//= 1.f; // upper limit (softer)
	= 0.75f; // default amount of filtering
			 //= 0.5f; // lower limit (sharper, less sub-pixel aliasing removal)
			 //= 0.25f; // almost off
			 //= 0.f;					 // completely off

const int NB_SEARCH_STEPS = 12; // !!! MAX 12, see array:
// setup like "PC EXTREME QUALITY" in Lottes' code
const float AA_QUALITY[ 12 ] = { 1.f, 1.f, 1.f, 1.f, 1.f, 1.5f, 2.f, 2.f, 2.f, 2.f, 4.f, 8.f };

// FXAA requires non-linear color input
const vec3 luma = vec3( 0.299f, 0.587f, 0.114f );
float	   rgb2luma( const vec3 rgb ) { return dot( rgb, luma ); }

void main()
{
	vec2 invTexSize = 1.f / textureSize( inTexture, 0 );
	vec2 texCoord	= gl_FragCoord.xy * invTexSize;

	// =====================================================================================
	// local contrast check -> where to apply antialiasing ? (edge detection)
	// =====================================================================================
	// get current pixel and compute its luma
	vec3  rgbC	= texture( inTexture, texCoord ).xyz;
	float lumaC = rgb2luma( rgbC );
	// compute luma in north, south, east, west directions
	float lumaN = rgb2luma( textureOffset( inTexture, texCoord, ivec2( 0, -1 ) ).xyz );
	float lumaS = rgb2luma( textureOffset( inTexture, texCoord, ivec2( 0, 1 ) ).xyz );
	float lumaE = rgb2luma( textureOffset( inTexture, texCoord, ivec2( 1, 0 ) ).xyz );
	float lumaW = rgb2luma( textureOffset( inTexture, texCoord, ivec2( -1, 0 ) ).xyz );

	// determine min and max luma around the pixel
	float lumaMax = max( lumaC, max( max( lumaN, lumaS ), max( lumaE, lumaW ) ) );
	float lumaMin = min( lumaC, min( min( lumaN, lumaS ), min( lumaE, lumaW ) ) );
	// compute local contrast
	float lumaRange = lumaMax - lumaMin;
	// early exit if local contrast is lower than EDGE_THRESHOLD (ie. not on an edge)
	// threshold is clamped to EDGE_THRESHOLD_MIN to avoid AA in really dark areas
	if ( lumaRange < max( EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD ) )
	{
		outFragColor = texture( inTexture, texCoord );
		return;
	}
	// =====================================================================================

	// =====================================================================================
	// vertical/horizontal edge test
	// =====================================================================================
	// compute luma in the corners
	float lumaNW = rgb2luma( textureOffset( inTexture, texCoord, ivec2( -1, -1 ) ).xyz );
	float lumaSE = rgb2luma( textureOffset( inTexture, texCoord, ivec2( 1, 1 ) ).xyz );
	float lumaNE = rgb2luma( textureOffset( inTexture, texCoord, ivec2( 1, -1 ) ).xyz );
	float lumaSW = rgb2luma( textureOffset( inTexture, texCoord, ivec2( -1, 1 ) ).xyz );

	// combine lumas
	float lumaNS   = lumaN + lumaS;
	float lumaWE   = lumaW + lumaE;
	float lumaNESE = lumaNE + lumaSE;
	float lumaNWNE = lumaNW + lumaNE;
	float lumaNWSW = lumaNW + lumaSW;
	float lumaSWSE = lumaSW + lumaSE;
	// compute gradient in horizontal and vertical direction of the 3x3 neighborhood
	float edgeHor
		= abs( -2.f * lumaW + lumaNWSW ) + abs( -2.f * lumaC + lumaNS ) * 2.f + abs( -2.f * lumaE + lumaNESE );
	float edgeVert
		= abs( -2.f * lumaS + lumaSWSE ) + abs( -2.f * lumaC + lumaWE ) * 2.f + abs( -2.f * lumaN + lumaNWNE );

	// is the edge horizontal ?
	bool isHorEdge = edgeHor >= edgeVert;
	// =====================================================================================

	// =====================================================================================
	// edge orientation choice
	// =====================================================================================
	// get edge neighbor lumas
	float luma1, luma2;
	if ( isHorEdge )
	{
		luma1 = lumaN;
		luma2 = lumaS;
	}
	else
	{
		luma1 = lumaW;
		luma2 = lumaE;
	}
	float lengthSign = isHorEdge ? invTexSize.y : invTexSize.x;

	// compute gradiants for the chosen direction
	float gradient1 = abs( luma1 - lumaC );
	float gradient2 = abs( luma2 - lumaC );

	// average luma in the chosen direction
	float lumaAvg;
	if ( gradient1 >= gradient2 )
	{
		lumaAvg	   = ( luma1 + lumaC ) * 0.5f;
		lengthSign = -lengthSign; // flip direction !
	}
	else
	{
		lumaAvg = ( luma2 + lumaC ) * 0.5f;
	}
	// compute scaled gradient in the chosen direction
	float gradientScaled = max( gradient1, gradient2 ) * 0.25f;

	// shift current texCoord by hal a pixel and compute offset in the chosen direction
	vec2 currentTexCoord = texCoord;
	vec2 offset; // initiliazing offset x/y with = vec2(...) results in some missed pixels... why ?
	if ( isHorEdge )
	{
		currentTexCoord.y += lengthSign * 0.5f;
		offset.x = invTexSize.y;
		offset.y = 0.f;
	}
	else
	{
		currentTexCoord.x += lengthSign * 0.5f;
		offset.x = 0.f;
		offset.y = invTexSize.x;
	}
	// =====================================================================================

	// =====================================================================================
	// end-of-edge search
	// =====================================================================================
	// AA_QUALITY speeds up research when distance from current pixel grows up
	vec2 texCoord1 = currentTexCoord - offset * AA_QUALITY[ 0 ];
	vec2 texCoord2 = currentTexCoord + offset * AA_QUALITY[ 0 ];
	// compute luma variation
	float lumaVar1 = rgb2luma( texture( inTexture, texCoord1 ).xyz ) - lumaAvg;
	float lumaVar2 = rgb2luma( texture( inTexture, texCoord2 ).xyz ) - lumaAvg;
	// stop when variation is higher than gradient
	bool isDone1 = abs( lumaVar1 ) >= gradientScaled;
	bool isDone2 = abs( lumaVar2 ) >= gradientScaled;
	if ( !isDone1 )
	{
		texCoord1 -= offset * AA_QUALITY[ 1 ];
	}
	if ( !isDone2 )
	{
		texCoord2 += offset * AA_QUALITY[ 1 ];
	}
	bool isDone = isDone1 && isDone2;
	// and iterate...
	for ( int i = 2; i < NB_SEARCH_STEPS && !isDone; ++i )
	{
		// compute luma variation with previous step
		if ( !isDone1 )
		{
			lumaVar1 = rgb2luma( texture( inTexture, texCoord1.xy ).xyz ) - lumaAvg;
		}
		if ( !isDone2 )
		{
			lumaVar2 = rgb2luma( texture( inTexture, texCoord2.xy ).xyz ) - lumaAvg;
		}
		isDone1 = abs( lumaVar1 ) >= gradientScaled;
		isDone2 = abs( lumaVar2 ) >= gradientScaled;
		// shift extremities if not done
		if ( !isDone1 )
		{
			texCoord1 -= offset * AA_QUALITY[ i ];
		}
		if ( !isDone2 )
		{
			texCoord2 += offset * AA_QUALITY[ i ];
		}
		isDone = isDone1 && isDone2;
	}

	// compute distance between center and each reached extremity
	float dist1, dist2;
	if ( isHorEdge )
	{
		dist1 = texCoord.x - texCoord1.x;
		dist2 = texCoord2.x - texCoord.x;
	}
	else
	{
		dist1 = texCoord.y - texCoord1.y;
		dist2 = texCoord2.y - texCoord.y;
	}
	// determine closest extremity direction and distance
	bool  isDir1 = dist1 < dist2;
	float dist	 = min( dist1, dist2 );

	// check if luma variation is coherent (maybe we went too far)
	bool isLumaVarCoherent = ( ( isDir1 ? lumaVar1 : lumaVar2 ) < 0.f ) != lumaC < lumaAvg;

	// compute offset if coherent (else no offset)
	float edgeLength  = ( dist1 + dist2 );
	float pixelOffset = isLumaVarCoherent ? ( -dist / edgeLength ) + 0.5f : 0.f;
	// =====================================================================================

	// =====================================================================================
	// sub-pixel aliasing test
	// =====================================================================================
	// compute average weighted luma in the 3x3 neighborhood
	float lumaAvgN = ( ( lumaNS + lumaWE ) * 2.f + ( lumaNWSW + lumaNESE ) ) / 12.f;
	// compute sub-pixel offset and refined it
	float subpixelOffsetTmp1 = clamp( abs( lumaAvgN - lumaC ) / lumaRange, 0.f, 1.f );
	float subpixelOffsetTmp2 = ( -2.f * subpixelOffsetTmp1 + 3.f ) * subpixelOffsetTmp1 * subpixelOffsetTmp1;
	float subpixelOffset	 = subpixelOffsetTmp2 * subpixelOffsetTmp2 * QUALITY_SUBPIXEL;

	// keep farthest offset
	float aaOffset	 = max( pixelOffset, subpixelOffset );
	vec2  aaTexCoord = texCoord;
	if ( isHorEdge )
	{
		aaTexCoord.y += aaOffset * lengthSign;
	}
	else
	{
		aaTexCoord.x += aaOffset * lengthSign;
	}
	// =====================================================================================

	outFragColor = vec4( texture( inTexture, aaTexCoord ) );
}
