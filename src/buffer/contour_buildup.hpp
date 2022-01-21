#ifndef __VTX_BUFFER_CONTOUR_BUILDUP__
#define __VTX_BUFFER_CONTOUR_BUILDUP__

#include "base_buffer_opengl.hpp"

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
		};
	} // namespace Buffer
} // namespace VTX
#endif
