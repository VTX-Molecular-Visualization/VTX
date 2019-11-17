#include <iostream>
#include <random>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "defines.hpp"
#include <stb/stb_image_write.h>

int main( int argc, char ** argv )
{
	const uint width  = atoi( argv[ 1 ] );
	const uint height = atoi( argv[ 2 ] );

	std::vector<uchar> buffer( width * height * JPG_CHANNELS );

	std::random_device										 rd;
	std::mt19937											 rng( rd() );
	std::uniform_int_distribution<std::mt19937::result_type> rand( 0, 255 );

	for ( uint h = 0; h < height; ++h )
	{
		for ( uint w = 0; w < width; ++w )
		{
			uint pixelIndex = ( ( h * width ) + w ) * JPG_CHANNELS;

			// Generate random color for each pixel channels.
			for ( uint c = 0; c < JPG_CHANNELS; ++c )
			{
				buffer[ pixelIndex + c ] = rand( rng );
			}
		}
	}

	// Write image buffer in file.
	stbi_write_jpg( "image.jpg", width, height, JPG_CHANNELS, buffer.data(), JPG_QUALITY );

	buffer.clear();

	return 0;
}
