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
		void Snapshoter::takeSnapshot( const IO::Path & p_path ) const
		{
			ImGuiIO &		   io	  = ImGui::GetIO();
			uint			   width  = (uint)io.DisplaySize.x;
			uint			   height = (uint)io.DisplaySize.y;
			uint			   size	  = width * height * JPG_CHANNELS * sizeof( char );
			std::vector<uchar> buffer( width * height * JPG_CHANNELS );

			glReadnPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, size, buffer.data() );

			stbi_flip_vertically_on_write( true );
			stbi_write_jpg( p_path.c_str(), width, height, JPG_CHANNELS, buffer.data(), JPG_QUALITY );
		}

	} // namespace Tool
} // namespace VTX
