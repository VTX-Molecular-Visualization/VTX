#ifndef __VTX_RENDERER_SYSTEM_BUILD__
#define __VTX_RENDERER_SYSTEM_BUILD__

#include "renderer/caches.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/geometry/geometries.hpp"
#include "renderer/layout/layouts.hpp"
#include "renderer/resource_handler.hpp"
#include "renderer/system_build_pipeline.hpp"
#include "renderer/system_data.hpp"
#include <unordered_set>

namespace VTX::Renderer::SystemBuild
{
	/**
	 * @brief Mutable state required by system build phases.
	 */
	struct Context
	{
		VTX::Renderer::Context::ContextWrapper &			   rendererContext;
		ResourceHandler<Cache::System, DescDummy, SystemUID> & systems;
		Layouts &											   layouts;
		Geometries &										   geometries;
		std::unordered_set<Desc::Handle> &					   systemToRefresh;
	};

	struct ClearSystems
	{
		void clear( Context & p_context ) const
		{
			p_context.systems.clear();
			p_context.layouts.clearSystems();
			p_context.geometries.clearSystems();
			p_context.systemToRefresh.clear();
		}
	};

	struct RegisterSystems
	{
		void registerSystem( Context & p_context, const SystemData & p_system ) const
		{
			p_context.systems.emplace( p_system.uid, {}, Cache::System { p_system.transform } );
			const Desc::Handle handle = p_context.systems.handle( p_system.uid );

			// TODO: Build costly geometries lazily when requested by a representation (ribbons, SES).
			p_context.geometries.construct( handle, p_system );

			p_context.layouts.atoms.add( handle, p_context.geometries.spheres.size( handle ) );
			p_context.layouts.residues.add(
				handle, static_cast<uint32_t>( p_context.geometries.ribbons.construction( handle ).residues.size() )
			);
		}
	};

	struct AllocateSystemInputs
	{
		void allocateInputs( Context & p_context ) const
		{
			p_context.layouts.resizeSystems( p_context.rendererContext );
		}
	};

	struct AllocateGeometryOutputs
	{
		void allocateOutputs( Context & p_context ) const
		{
			p_context.geometries.resizeSystems( p_context.rendererContext );
		}
	};

	struct BuildDrawRanges
	{
		void buildDrawRanges( Context & p_context ) const
		{
			p_context.geometries.buildDrawRanges( p_context.rendererContext );
		}
	};

	using DefaultPipeline = SystemBuildPipeline<
		ClearSystems,
		RegisterSystems,
		AllocateSystemInputs,
		AllocateGeometryOutputs,
		BuildDrawRanges>;
} // namespace VTX::Renderer::SystemBuild

#endif
