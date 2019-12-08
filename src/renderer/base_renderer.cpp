#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		void BaseRenderer::setSize( const int p_width, const int p_height )
		{
			_width	= p_width;
			_height = p_height;
		}
	} // namespace Renderer
} // namespace VTX
