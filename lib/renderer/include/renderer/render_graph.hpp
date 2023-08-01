#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept_context.hpp"
#include "scheduler/concept_scheduler.hpp"

namespace VTX::Renderer
{
	enum struct E_FORMAT
	{
		RGBA16F,
		RGBA32UI,
	};

	enum struct E_ACCESS
	{
		READ,
		WRITE,
		READ_WRITE,
	};

	// Descriptors.
	struct StructDescTexture
	{
		E_FORMAT format;
	};

	// Render graph.
	template<Context::Concept C, Scheduler::Concept S>
	class RenderGraph
	{
	  public:
		RenderGraph()
		{
			_context   = std::make_unique<C>();
			_scheduler = std::make_unique<S>();
		}

		void addPass( const StructRenderPass & p_pass ) {}

		bool connect( const StructResource & p_from, const StructResource & p_to ) { return true; }

		// void addAttachment() {}
		// void addStorage() {}

		void setup() { _commandList = _scheduler->schedule( _passes ); }
		void render() {}

	  private:
		std::unique_ptr<C> _context;
		std::unique_ptr<S> _scheduler;
		ListRessources	   _resources;
		ListPasses		   _passes;

		Scheduler::ListCommand _commandList;
	};

} // namespace VTX::Renderer
#endif
