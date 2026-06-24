void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackData( inTexturePackedData, data, texCoord );
	if ( shadeBackground( data ) )
	{
		return;
	}

	const vec3 lightDirection
		= uniformsCamera.isCameraPerspective == 1 ? normalize( -data.viewPosition ) : vec3( 0.f, 0.f, 1.f );
	const float lighting		 = computeLighting( data, lightDirection );
	const float ambientOcclusion = sampleAmbientOcclusion( texCoord, data.viewPosition.z );
	const float fogFactor = computeFogFactor( -data.viewPosition.z );
	const vec3	color
		= texelFetch( inTextureColor, texCoord, 0 ).rgb * ambientOcclusion * lighting * uniforms.colorLight.rgb;

	outFragColor = vec4( mix( color, uniforms.colorFog.rgb, fogFactor ), 1.f );
}
