#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include <functional>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer
{
	enum HandleType
	{
		Attachment,
		Storage,
		// ...?
	};

	enum PassType
	{
		Graphic,
		Compute,
		// ...?
	};

	struct Handle
	{
		size_t	   id;
		HandleType type;
	};

	struct Resource
	{
		const Handle handle;
	};

	struct ResourceMutable
	{
		Handle handle;
	};

	struct Builder
	{
	};

	using CallbakcPassSetup	 = std::function<void>( Builder &, Resource, ResourceMutable );
	using CallbakcPassRender = std::function<void>();

	template<class Ctx>
	struct RenderGraph
	{
		template<class Data>
		void addPass( std::string & p_name, CallbakcPassSetup, CallbakcPassRender )
		{
		}

		void addAttachment() {}
		void addStorage() {}

		void setup() {}
		void render() {}

		std::unique_ptr<Ctx> context;
		std::vector<Handle>	 resources;
	};
} // namespace VTX::Renderer
#endif
