#ifndef __VTX_RENDERER_BUILDER_MODEL_BUILD__
#define __VTX_RENDERER_BUILDER_MODEL_BUILD__

#include "renderer/binary_buffer.hpp"
#include "renderer/caches.hpp"
#include "renderer/context/context_wrapper.hpp"
#include <util/math.hpp>

namespace VTX::Renderer::Builder
{
	struct Models
	{
		template<typename... Resources>
		static void upload(
			Context::ContextWrapper & p_context,
			const Cache::Camera &	  p_camera,
			Resources &... p_resources
		)
		{
			if ( ( p_resources.empty() && ... ) )
			{
				return;
			}

			BinaryBuffer430 buffer;
			uint32_t		modelIndex = 0;
			( _append( buffer, p_resources, p_camera, modelIndex ), ... );

			buffer.close();
			p_context.setBuffer( { "Models" }, buffer );
		}

	  private:
		template<typename Resources>
		static void _append(
			BinaryBuffer430 &	  p_buffer,
			Resources &			  p_resources,
			const Cache::Camera & p_camera,
			uint32_t &			  p_modelIndex
		)
		{
			for ( auto entry : p_resources.entries() )
			{
				auto & resource		= entry.resource;
				resource.modelIndex = p_modelIndex++;

				const Mat4f matrixModelView	   = p_camera.matView * resource.transform;
				const Mat4f matrixModelViewInv = Util::Math::inverse( matrixModelView );
				const Mat4f matrixNormal	   = Util::Math::transpose( matrixModelViewInv );

				p_buffer.write( matrixModelView );
				p_buffer.write( matrixModelViewInv );
				p_buffer.write( matrixNormal );
			}
		}
	};
} // namespace VTX::Renderer::Builder

#endif // !__VTX_RENDERER_BUILDER_MODEL_BUILD__
