#include "snapshoter.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "define.hpp"
#include "io/path.hpp"
#include "ui/imgui/imgui.h"
#include "util/time.hpp"
#include <gl/gl3w.h>
#include <stb/stb_image_write.h>
#include <vector>

namespace VTX
{
	namespace Tool
	{
		bool Snapshoter::takeSnapshot( const IO::Path & p_path ) const
		{
			ImGuiIO &		   io	  = ImGui::GetIO();
			uint			   width  = (uint)io.DisplaySize.x;
			uint			   height = (uint)io.DisplaySize.y;
			uint			   size	  = width * height * 3 * sizeof( char );
			std::vector<uchar> buffer( width * height * 3 );

			glReadnPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, size, buffer.data() );

			stbi_flip_vertically_on_write( true );
			stbi_write_png_compression_level = 0;
			return stbi_write_png( p_path.c_str(), width, height, 3, buffer.data(), 0 );
			// stbi_write_jpg( p_path.c_str(), width, height, 3, buffer.data(), 100 );
		}

	} // namespace Tool
} // namespace VTX
