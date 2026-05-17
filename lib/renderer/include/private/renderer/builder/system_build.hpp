#ifndef __VTX_RENDERER_BUILDER_SYSTEM_BUILD__
#define __VTX_RENDERER_BUILDER_SYSTEM_BUILD__

#include "renderer/builder/system_build_pipeline.hpp"
#include "renderer/caches.hpp"
#include "renderer/color.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/geometry/geometries.hpp"
#include "renderer/layout/layouts.hpp"
#include "renderer/resource_handler.hpp"
#include "renderer/system_data.hpp"
#include <unordered_set>
#include <util/enum.hpp>
#include <util/math.hpp>

namespace VTX::Renderer::Builder
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

	struct UploadSystemInputs
	{
		void uploadInput( Context & p_context, const SystemData & p_system ) const
		{
			using namespace Layout;

			const Desc::Handle handle = p_context.systems.handle( p_system.uid );

			p_context.layouts.atoms.upload<ATOM_ATTR::SYMBOL, Symbol>(
				p_context.rendererContext, handle, p_system.data.atomSymbols
			);
			p_context.layouts.atoms.upload<ATOM_ATTR::ID, PickingUID>(
				p_context.rendererContext, handle, p_system.atomUids
			);
			p_context.layouts.atoms.upload<ATOM_ATTR::POSITION, Vec3f>(
				p_context.rendererContext, handle, p_system.trajectory
			);

			if ( p_context.geometries.ribbons.empty( handle ) )
			{
				return;
			}

			const auto &			construction  = p_context.geometries.ribbons.construction( handle );
			const Index				countResidues = p_context.layouts.residues.size( handle );
			std::vector<PickingUID> residueIds( countResidues );
			std::vector<uint8_t>	residueTypes( countResidues );
			std::vector<ColorIndex> residueColors( countResidues );

			for ( Index i = 0; i < countResidues; ++i )
			{
				const Index residueIndex = construction.residues[ i ].index;
				residueIds[ i ]			 = p_system.residueUids[ residueIndex ];
				auto ss					 = p_system.data.residueSecondaryStructureTypes[ residueIndex ];
				residueTypes[ i ]		 = toUnderlying( ss );
				residueColors[ i ]		 = Color::getColorIndex( ss );
			}

			p_context.layouts.residues.upload<RESIDUE_ATTR::ID, PickingUID>(
				p_context.rendererContext, handle, residueIds
			);
			p_context.layouts.residues.upload<RESIDUE_ATTR::TYPE, uint8_t>(
				p_context.rendererContext, handle, residueTypes
			);
			p_context.layouts.residues.upload<RESIDUE_ATTR::COLOR, ColorIndex>(
				p_context.rendererContext, handle, residueColors
			);
		}
	};

	struct BuildRibbonInputs
	{
		void buildDerived( Context & p_context, std::span<const SystemData> p_systems ) const
		{
			for ( const SystemData & system : p_systems )
			{
				_buildSystem( p_context, system );
			}
		}

	  private:
		static void _buildSystem( Context & p_context, const SystemData & p_system )
		{
			using namespace Layout;

			const Desc::Handle handle		= p_context.systems.handle( p_system.uid );
			const auto &	   construction = p_context.geometries.ribbons.construction( handle );
			if ( construction.isEmpty )
			{
				return;
			}

			const Index		   countResidues = p_context.layouts.residues.size( handle );
			std::vector<Vec4f> ribbonPositions( countResidues );
			std::vector<Vec3f> ribbonDirections( countResidues );

			for ( Index i = 0; i < countResidues; ++i )
			{
				// Compute direction between carbon alpha and oxygen.
				const Vec3f & positionCA   = p_system.trajectory[ construction.residues[ i ].ca ];
				const Vec3f & positionO	   = p_system.trajectory[ construction.residues[ i ].o ];
				const Vec3f	  directionCAO = Util::Math::normalize( positionO - positionCA );

				ribbonPositions[ i ]  = Vec4f( positionCA, i );
				ribbonDirections[ i ] = directionCAO;

				if ( i > 0 )
				{
					const Vec3f & prevDirection = ribbonDirections[ i - 1 ];
					if ( Util::Math::dot( directionCAO, prevDirection ) < 0.f )
					{
						ribbonDirections[ i ] = -directionCAO;
					}
				}
			}

			p_context.layouts.residues.upload<RESIDUE_ATTR::POSITION, Vec4f>(
				p_context.rendererContext, handle, ribbonPositions
			);
			p_context.layouts.residues.upload<RESIDUE_ATTR::DIRECTION, Vec3f>(
				p_context.rendererContext, handle, ribbonDirections
			);
		}
	};

	struct UploadSystemRenderState
	{
		void uploadInput( Context & p_context, const SystemData & p_system ) const
		{
			const Desc::Handle handle = p_context.systems.handle( p_system.uid );

			uploadColors( p_context, handle, p_system.atomColors );
			uploadRepresentations( p_context, handle, p_system.representationRanges, p_system.atomRepresentations );
			uploadVisibility( p_context, handle, p_system.visibility );
			uploadSelection( p_context, handle, p_system.atomFlags );
		}

		static void uploadColors(
			Context &					p_context,
			const Desc::Handle			p_handle,
			std::span<const ColorIndex> p_colors
		)
		{
			using namespace Layout;

			assert( p_colors.size() == p_context.layouts.atoms.size( p_handle ) );

			p_context.layouts.atoms.upload<ATOM_ATTR::COLOR, ColorIndex>(
				p_context.rendererContext, p_handle, p_colors
			);
		}

		static void uploadRepresentations(
			Context &							 p_context,
			const Desc::Handle					 p_handle,
			const MapRepresentationRanges &		 p_representations,
			std::span<const RepresentationIndex> p_atomRepresentations
		)
		{
			using namespace Layout;

			Cache::System & cache = p_context.systems.get( p_handle );
			cache.representations = p_representations;

			const Index countAtoms = p_context.layouts.atoms.size( p_handle );
			assert( p_atomRepresentations.size() == countAtoms );

			p_context.layouts.atoms.upload<ATOM_ATTR::REPRESENTATION, RepresentationIndex>(
				p_context.rendererContext, p_handle, p_atomRepresentations
			);

			_uploadResidueRepresentations( p_context, p_handle, p_atomRepresentations );

			p_context.systemToRefresh.insert( p_handle );
		}

		static void uploadVisibility(
			Context &				   p_context,
			const Desc::Handle		   p_handle,
			const Util::Math::BitSet & p_visibility
		)
		{
			Cache::System & cache = p_context.systems.get( p_handle );
			cache.visibility	  = p_visibility;

			p_context.systemToRefresh.insert( p_handle );
		}

		static void uploadSelection(
			Context &			  p_context,
			const Desc::Handle	  p_handle,
			std::span<const Flag> p_atomFlags
		)
		{
			using namespace Layout;

			assert( p_atomFlags.size() == p_context.layouts.atoms.size( p_handle ) );

			p_context.layouts.atoms.upload<ATOM_ATTR::FLAG, Flag>( p_context.rendererContext, p_handle, p_atomFlags );

			_uploadResidueFlags( p_context, p_handle, p_atomFlags );
		}

	  private:
		static void _uploadResidueRepresentations(
			Context &							 p_context,
			const Desc::Handle					 p_handle,
			std::span<const RepresentationIndex> p_atomRepresentations
		)
		{
			using namespace Layout;

			const auto &					 construction  = p_context.geometries.ribbons.construction( p_handle );
			const Index						 countResidues = p_context.layouts.residues.size( p_handle );
			std::vector<RepresentationIndex> residues( countResidues );

			for ( Index i = 0; i < countResidues; ++i )
			{
				const Index atomIndex = construction.residues[ i ].ca;
				assert( atomIndex < p_atomRepresentations.size() );
				residues[ i ] = p_atomRepresentations[ atomIndex ];
			}

			p_context.layouts.residues.upload<RESIDUE_ATTR::REPRESENTATION, RepresentationIndex>(
				p_context.rendererContext, p_handle, residues
			);
		}

		static void _uploadResidueFlags(
			Context &			  p_context,
			const Desc::Handle	  p_handle,
			std::span<const Flag> p_atomFlags
		)
		{
			using namespace Layout;

			const auto &	  construction = p_context.geometries.ribbons.construction( p_handle );
			std::vector<Flag> residueFlags( p_context.layouts.residues.size( p_handle ), 0 );

			for ( Index i = 0; i < residueFlags.size(); ++i )
			{
				const Index atomIndex = construction.residues[ i ].ca;
				assert( atomIndex < p_atomFlags.size() );
				if ( p_atomFlags[ atomIndex ] & ELEMENT_FLAG_SELECTION )
				{
					residueFlags[ i ] |= ELEMENT_FLAG_SELECTION;
				}
			}

			p_context.layouts.residues.upload<RESIDUE_ATTR::FLAG, Flag>(
				p_context.rendererContext, p_handle, residueFlags
			);
		}
	};

	struct WriteGeometryOutputs
	{
		void writeOutput( Context & p_context, const SystemData & p_system ) const
		{
			const Desc::Handle handle = p_context.systems.handle( p_system.uid );

			p_context.geometries.spheres.uploadIndexes( p_context.rendererContext, handle );
			p_context.geometries.cylinders.uploadIndexes( p_context.rendererContext, handle );

			if ( not p_context.geometries.ribbons.empty( handle ) )
			{
				p_context.geometries.ribbons.uploadIndexes( p_context.rendererContext, handle );
			}
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
		UploadSystemInputs,
		UploadSystemRenderState,
		BuildRibbonInputs,
		WriteGeometryOutputs,
		BuildDrawRanges>;
} // namespace VTX::Renderer::Builder

#endif
