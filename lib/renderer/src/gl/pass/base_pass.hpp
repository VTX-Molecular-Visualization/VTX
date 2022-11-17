#ifndef __VTX_BASE_PASS__
#define __VTX_BASE_PASS__

#include "define.hpp"

namespace VTX::Renderer::GL
{
	class GL;
	namespace Pass
	{
		class BasePass
		{
		  public:
			BasePass()			= default;
			virtual ~BasePass() = default;

			virtual void init( const size_t, const size_t, const GL & )	  = 0;
			virtual void resize( const size_t, const size_t, const GL & ) = 0;
			virtual void render( const GL & )							  = 0;
		};
	} // namespace Pass
} // namespace VTX::Renderer::GL

#endif
