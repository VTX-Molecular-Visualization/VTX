#include "snapshoter.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../defines.hpp"
#include "../io/path.hpp"
#include "../ui/imgui/imgui.h"
#include "../util/time.hpp"
#include <random>
#include <stb/stb_image_write.h>
#include <vector>

namespace VTX
{
	namespace Tool
	{
		void Snapshoter::takeSnapshot() const
		{
			ImGuiIO & io	 = ImGui::GetIO();
			uint	  width	 = (uint)io.DisplaySize.x;
			uint	  height = (uint)io.DisplaySize.y;

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
			VTX_INFO( std::string( Util::Time::getTimestamp() ) );
			std::string filename = std::string( Util::Time::getTimestamp() );
			IO::Path	path( SNAPSHOT_DIR + filename + ".jpg" );
			std::cout << path.c_str() << std::endl;
			stbi_write_jpg( path.c_str(), width, height, JPG_CHANNELS, buffer.data(), JPG_QUALITY );

			buffer.clear();
		}

	} // namespace Tool
} // namespace VTX
