#ifndef __VTX_BUFFER_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_BUFFER_SOLVENT_EXCLUDED_SURFACE__

#include "base_buffer_opengl.hpp"
#include "renderer/gl/buffer_storage.hpp"

namespace VTX
{
	namespace Buffer
	{
		class SolventExcludedSurface : public BaseBufferOpenGL
		{
		  public:
			SolventExcludedSurface()  = default;
			~SolventExcludedSurface() = default;

			void generate() override;

		  private:
		};
	} // namespace Buffer
} // namespace VTX
#endif
