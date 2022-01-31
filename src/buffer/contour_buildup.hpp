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

			inline const Renderer::GL::BufferStorage & getSsboTorusPatches() const { return _ssboTorusPatches; }
			inline const Renderer::GL::BufferStorage & getSsboSphericalTrianglePatches() const
			{
				return _ssboSphericalTrianglePatches;
			}
			inline const Renderer::GL::BufferStorage & getSsboProbeIntersection() const
			{
				return _ssboProbeIntersection;
			}

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
