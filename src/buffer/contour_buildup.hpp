#ifndef __VTX_BUFFER_CONTOUR_BUILDUP__
#define __VTX_BUFFER_CONTOUR_BUILDUP__

#include "base_buffer_opengl.hpp"
#include "renderer/gl/buffer_storage.hpp"

namespace VTX
{
	namespace Buffer
	{
		class ContourBuildup : public BaseBufferOpenGL
		{
		  public:
			ContourBuildup()  = default;
			~ContourBuildup() = default;

			void generate() override;

		  private:
			Renderer::GL::BufferStorage _ssboTorusPatches
				= Renderer::GL::BufferStorage( Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
			Renderer::GL::BufferStorage _ssboSphericalTrianglePatches
				= Renderer::GL::BufferStorage( Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
			Renderer::GL::BufferStorage _ssboProbeIntersection
				= Renderer::GL::BufferStorage( Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
		};
	} // namespace Buffer
} // namespace VTX
#endif
