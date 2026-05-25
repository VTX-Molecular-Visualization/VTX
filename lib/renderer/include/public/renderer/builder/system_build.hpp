#ifndef __VTX_RENDERER_BUILDER_SYSTEM_BUILD__
#define __VTX_RENDERER_BUILDER_SYSTEM_BUILD__

#include "renderer/binary_buffer.hpp"
#include "renderer/caches.hpp"
#include "renderer/color.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/geometry/geometries.hpp"
#include "renderer/layout/layouts.hpp"
#include "renderer/representation.hpp"
#include "renderer/resource_handler.hpp"
#include <algorithm>
#include <unordered_set>
#include <util/enum.hpp>
#include <util/math.hpp>

namespace VTX::Renderer::Builder
{
	struct SystemRegistry
	{
		template<typename Systems>
		static void clear(
			Systems &						   p_systems,
			Layouts &						   p_layouts,
			Geometries &					   p_geometries,
			std::unordered_set<Desc::Handle> & p_dirtyGeometrySystems
		)
		{
			p_systems.clear();
			p_layouts.clearSystems();
			p_geometries.clearSystems();
			p_dirtyGeometrySystems.clear();
		}

		template<typename Systems>
		static Desc::Handle registerSystem(
			Systems &		 p_systems,
			Geometries &	 p_geometries,
			Layouts &		 p_layouts,
			Cache::System && p_system
		)
		{
			Desc::Handle key = 0;
			for ( const Desc::Handle existingKey : p_systems.keys() )
			{
				key = std::max<Desc::Handle>( key, existingKey + 1 );
			}

			const Desc::Handle	  handle = p_systems.emplace( key, std::move( p_system ) );
			const Cache::System & system = p_systems.get( handle );

			p_geometries.construct( handle, system );
			p_layouts.atoms.add( handle, p_geometries.spheres.size( handle ) );

			return handle;
		}
	};

	struct AtomLayout
	{
		static void allocateInputs( Context::ContextWrapper & p_context, Layouts & p_layouts )
		{
			p_layouts.atoms.resize( p_context );
		}

		template<typename Systems>
		static void uploadInput(
			Context::ContextWrapper &		   p_context,
			Systems &						   p_systems,
			Layouts &						   p_layouts,
			const Desc::Handle				   p_handle,
			std::unordered_set<Desc::Handle> & p_dirtyGeometrySystems
		)
		{
			const Cache::System & system		  = p_systems.get( p_handle );
			const auto			  colors		  = buildAtomColors( system );
			const auto			  representations = buildAtomRepresentations( system );
			const auto			  flags			  = buildAtomFlags( system );
			const auto			  atomIds		  = system.atomUids.toStdVector();

			uploadSymbols( p_context, p_layouts, p_handle, system.data.atomSymbols );
			uploadIds( p_context, p_layouts, p_handle, atomIds );
			uploadPositions( p_context, p_systems, p_layouts, p_handle, system.trajectory );
			uploadColors( p_context, p_layouts, p_handle, colors );
			uploadRepresentations( p_context, p_layouts, p_handle, representations, p_dirtyGeometrySystems );
			uploadSelection( p_context, p_layouts, p_handle, flags );
		}

		static std::vector<ColorIndex> buildAtomColors( const Cache::System & p_system )
		{
			const Core::Struct::Topology & data = p_system.data;
			std::vector<ColorIndex>		   atoms( data.getAtomCount() );
			size_t						   count = 0;

			for ( const auto & [ scheme, ranges ] : p_system.colorSchemeAtoms )
			{
				if ( scheme == E_COLOR_SCHEME::ATOM )
				{
					for ( const Index atom : ranges )
					{
						atoms[ atom ] = Color::getColorIndex( data.getAtomSymbol( atom ) );
					}
				}
				else if ( scheme == E_COLOR_SCHEME::RESIDUE )
				{
					for ( const Index atom : ranges )
					{
						const Index residue = data.atomResidueIndexes[ atom ];
						atoms[ atom ]		= Color::getColorIndex( data.getResidueSymbol( residue ) );
					}
				}
				else if ( scheme == E_COLOR_SCHEME::CHAIN )
				{
					for ( const Index atom : ranges )
					{
						const Index chain = data.getAtomChainIndex( atom );
						atoms[ atom ]	  = Color::getColorIndex( data.getChainName( chain ) );
					}
				}
				else
				{
					assert( false && "Unsupported renderer atom color scheme." );
				}

				count += ranges.count();
			}
			for ( const auto & [ colorIndex, ranges ] : p_system.customColorAtoms )
			{
				for ( const Index atom : ranges )
				{
					atoms[ atom ] = colorIndex;
				}
				count += ranges.count();
			}

			assert( count == data.getAtomCount() );

			return atoms;
		}

		static std::vector<RepresentationIndex> buildAtomRepresentations( const Cache::System & p_system )
		{
			std::vector<RepresentationIndex> atoms( p_system.data.getAtomCount(), RepresentationIndex( 0 ) );

			// TODO: resolve App preset Entity keys to renderer representation handles.
			size_t count = 0;
			for ( const auto & [ preset, ranges ] : p_system.presetAtoms )
			{
				for ( auto it = ranges.rangeBegin(); it != ranges.rangeEnd(); ++it )
				{
					std::fill_n( atoms.begin() + it->getFirst(), it->getCount(), RepresentationIndex( 0 ) );
				}
				count += ranges.count();
			}

			assert( count == 0 || count == p_system.data.getAtomCount() );

			return atoms;
		}

		static std::vector<Flag> buildAtomFlags( const Cache::System & p_system )
		{
			std::vector<Flag> atomFlags( p_system.data.getAtomCount(), 0 );
			p_system.selection.forEachSetBit( [ & ]( const size_t p_atom )
											  { atomFlags[ p_atom ] |= toUnderlying( E_ELEMENT_FLAGS::SELECTION ); } );

			return atomFlags;
		}

		template<typename Systems>
		static void refreshColors(
			Context::ContextWrapper & p_context,
			Systems &				  p_systems,
			Layouts &				  p_layouts,
			const Desc::Handle		  p_handle
		)
		{
			uploadColors( p_context, p_layouts, p_handle, buildAtomColors( p_systems.get( p_handle ) ) );
		}

		static void uploadSymbols(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			const Desc::Handle		  p_handle,
			std::span<const Symbol>	  p_symbols
		)
		{
			using namespace Layout;

			assert( p_symbols.size() == p_layouts.atoms.size( p_handle ) );
			p_layouts.atoms.upload<ATOM_ATTR::SYMBOL, Symbol>( p_context, p_handle, p_symbols );
		}

		static void uploadIds(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			const Desc::Handle		  p_handle,
			std::span<const UID32>	  p_ids
		)
		{
			using namespace Layout;

			assert( p_ids.size() == p_layouts.atoms.size( p_handle ) );
			p_layouts.atoms.upload<ATOM_ATTR::ID, UID32>( p_context, p_handle, p_ids );
		}

		template<typename Systems>
		static void uploadPositions(
			Context::ContextWrapper & p_context,
			Systems &				  p_systems,
			Layouts &				  p_layouts,
			const Desc::Handle		  p_handle,
			std::span<const Vec3f>	  p_positions
		)
		{
			using namespace Layout;

			assert( p_positions.size() == p_layouts.atoms.size( p_handle ) );
			p_systems.get( p_handle ).trajectory = p_positions;
			p_layouts.atoms.upload<ATOM_ATTR::POSITION, Vec3f>( p_context, p_handle, p_positions );
		}

		static void uploadColors(
			Context::ContextWrapper &	p_context,
			Layouts &					p_layouts,
			const Desc::Handle			p_handle,
			std::span<const ColorIndex> p_colors
		)
		{
			using namespace Layout;

			assert( p_colors.size() == p_layouts.atoms.size( p_handle ) );
			p_layouts.atoms.upload<ATOM_ATTR::COLOR, ColorIndex>( p_context, p_handle, p_colors );
		}

		static void uploadRepresentations(
			Context::ContextWrapper &			 p_context,
			Layouts &							 p_layouts,
			const Desc::Handle					 p_handle,
			std::span<const RepresentationIndex> p_atomRepresentations,
			std::unordered_set<Desc::Handle> &	 p_dirtyGeometrySystems
		)
		{
			using namespace Layout;

			assert( p_atomRepresentations.size() == p_layouts.atoms.size( p_handle ) );
			p_layouts.atoms.upload<ATOM_ATTR::REPRESENTATION, RepresentationIndex>(
				p_context, p_handle, p_atomRepresentations
			);
			p_dirtyGeometrySystems.insert( p_handle );
		}

		static void uploadSelection(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			const Desc::Handle		  p_handle,
			std::span<const Flag>	  p_atomFlags
		)
		{
			using namespace Layout;

			assert( p_atomFlags.size() == p_layouts.atoms.size( p_handle ) );
			p_layouts.atoms.upload<ATOM_ATTR::FLAG, Flag>( p_context, p_handle, p_atomFlags );
		}
	};

	struct SystemVisibility
	{
		template<typename Systems>
		static void uploadInput(
			Systems &						   p_systems,
			const Desc::Handle				   p_handle,
			std::unordered_set<Desc::Handle> & p_dirtyGeometrySystems
		)
		{
			uploadVisibility( p_systems, p_dirtyGeometrySystems, p_handle );
		}

		template<typename Systems>
		static void uploadVisibility(
			Systems &						   p_systems,
			std::unordered_set<Desc::Handle> & p_dirtyGeometrySystems,
			const Desc::Handle				   p_handle
		)
		{
			assert( p_systems.contains( p_handle ) );
			p_dirtyGeometrySystems.insert( p_handle );
		}

		template<typename Systems, typename Representations>
		static void refreshGeometryVisibility(
			Context::ContextWrapper & p_context,
			Systems &				  p_systems,
			Representations &		  p_representations,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle
		)
		{
			const Cache::System & systemCache = p_systems.get( p_handle );

			auto visibleSpheres	  = systemCache.visibility;
			auto visibleCylinders = systemCache.visibility;
			auto visibleRibbons	  = systemCache.visibility;
			bool requestedRibbon  = false;
			bool visibleSes		  = false;

			for ( const auto & [ preset, ranges ] : systemCache.presetAtoms )
			{
				const RepresentationIndex representationIndex = 0;
				assert( p_representations.contains( representationIndex ) );

				const auto & representation = p_representations.get( representationIndex );
				if ( not representation.showSphere )
				{
					visibleSpheres.subtractInPlace( ranges );
				}
				if ( not representation.showCylinder )
				{
					visibleCylinders.subtractInPlace( ranges );
				}
				if ( not representation.showRibbon )
				{
					visibleRibbons.subtractInPlace( ranges );
				}
				else if ( systemCache.visibility.any( ranges ) )
				{
					requestedRibbon = true;
				}
				if ( representation.showSes && systemCache.visibility.any( ranges ) )
				{
					visibleSes = true;
				}
			}

			if ( requestedRibbon && not p_geometries.ribbons.built( p_handle ) )
			{
				constructRibbon( p_context, p_systems, p_layouts, p_geometries, p_handle );
			}
			if ( visibleSes && not p_geometries.ses.built( p_handle ) )
			{
				constructSES( p_context, p_systems, p_layouts, p_geometries, p_handle );
			}

			p_geometries.spheres.setVisibility( p_handle, visibleSpheres );
			p_geometries.cylinders.setVisibility( p_handle, visibleCylinders );
			p_geometries.ribbons.setVisibility( p_handle, visibleRibbons );
			p_geometries.ses.setVisibility( p_handle, visibleSes );
		}

		template<typename Systems>
		static void constructRibbon(
			Context::ContextWrapper & p_context,
			Systems &				  p_systems,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle
		)
		{
			const Cache::System & system = p_systems.get( p_handle );

			p_geometries.ribbons.construct( p_handle, system );
			p_geometries.ribbons.resize( p_context );

			const auto & construction = p_geometries.ribbons.construction( p_handle );
			if ( construction.isEmpty )
			{
				return;
			}

			p_layouts.residues.add( p_handle, static_cast<uint32_t>( construction.residues.size() ) );
			p_layouts.residues.resize( p_context );

			uploadRibbonResidues( p_context, p_layouts, p_geometries, p_handle, system );
			uploadRibbonPositions( p_context, p_layouts, p_geometries, p_handle, system.trajectory );
		}

		template<typename Systems>
		static void constructSES(
			Context::ContextWrapper & p_context,
			Systems &				  p_systems,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle
		)
		{
			const Cache::System & system = p_systems.get( p_handle );
			p_geometries.constructSES( p_context, p_handle, system, p_layouts.atoms.offset( p_handle ) );
			p_geometries.ses.resize( p_context );
		}

		static void uploadRibbonResidues(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle,
			const Cache::System &	  p_system
		)
		{
			using namespace Layout;

			const auto &					 construction		 = p_geometries.ribbons.construction( p_handle );
			const Index						 countResidues		 = p_layouts.residues.size( p_handle );
			const auto						 atomRepresentations = AtomLayout::buildAtomRepresentations( p_system );
			const auto						 atomFlags			 = AtomLayout::buildAtomFlags( p_system );
			std::vector<UID32>				 residueIds( countResidues );
			std::vector<uint8_t>			 residueTypes( countResidues );
			std::vector<ColorIndex>			 residueColors( countResidues );
			std::vector<RepresentationIndex> residueRepresentations( countResidues );
			std::vector<Flag>				 residueFlags( countResidues, 0 );

			for ( Index i = 0; i < countResidues; ++i )
			{
				const Index residueIndex = construction.residues[ i ].index;
				const Index atomIndex	 = construction.residues[ i ].ca;
				const auto	ss			 = p_system.data.residueSecondaryStructureTypes[ residueIndex ];

				residueIds[ i ]				= p_system.residueUids.first + residueIndex;
				residueTypes[ i ]			= toUnderlying( ss );
				residueColors[ i ]			= Color::getColorIndex( ss );
				residueRepresentations[ i ] = atomRepresentations[ atomIndex ];

				if ( atomFlags[ atomIndex ] & toUnderlying( E_ELEMENT_FLAGS::SELECTION ) )
				{
					residueFlags[ i ] |= toUnderlying( E_ELEMENT_FLAGS::SELECTION );
				}
			}

			p_layouts.residues.upload<RESIDUE_ATTR::ID, UID32>( p_context, p_handle, residueIds );
			p_layouts.residues.upload<RESIDUE_ATTR::TYPE, uint8_t>( p_context, p_handle, residueTypes );
			p_layouts.residues.upload<RESIDUE_ATTR::COLOR, ColorIndex>( p_context, p_handle, residueColors );
			p_layouts.residues.upload<RESIDUE_ATTR::REPRESENTATION, RepresentationIndex>(
				p_context, p_handle, residueRepresentations
			);
			p_layouts.residues.upload<RESIDUE_ATTR::FLAG, Flag>( p_context, p_handle, residueFlags );
		}

		static void uploadRibbonPositions(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle,
			std::span<const Vec3f>	  p_positions
		)
		{
			using namespace Layout;

			const auto & construction = p_geometries.ribbons.construction( p_handle );
			if ( construction.isEmpty )
			{
				return;
			}

			const Index		   countResidues = p_layouts.residues.size( p_handle );
			std::vector<Vec4f> ribbonPositions( countResidues );
			std::vector<Vec3f> ribbonDirections( countResidues );

			for ( Index i = 0; i < countResidues; ++i )
			{
				const Vec3f & positionCA   = p_positions[ construction.residues[ i ].ca ];
				const Vec3f & positionO	   = p_positions[ construction.residues[ i ].o ];
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

			p_layouts.residues.upload<RESIDUE_ATTR::POSITION, Vec4f>( p_context, p_handle, ribbonPositions );
			p_layouts.residues.upload<RESIDUE_ATTR::DIRECTION, Vec3f>( p_context, p_handle, ribbonDirections );
		}
	};

	struct RepresentationState
	{
		template<typename Systems, typename Representations>
		static void upload(
			Context::ContextWrapper &					p_context,
			Systems &									p_systems,
			Representations &							p_representations,
			std::unordered_set<Desc::Handle> &			p_dirtyGeometrySystems,
			const std::vector<const Representation *> & p_representationsData
		)
		{
			BinaryBuffer<E_LAYOUT_TYPE::Std140> buffer;
			RepresentationIndex					index = 0;

			for ( const auto * representation : p_representationsData )
			{
				bool showSphere	  = representation->hasSphere;
				bool showCylinder = representation->hasCylinder;
				bool showRibbon	  = representation->hasRibbon;
				bool showSes	  = representation->hasSes;

				bool  isSphereRadiusFixed = representation->isRadiusSphereFixed;
				float cylinderRadius	  = representation->radiusCylinder;
				float sphereRadiusFixed	  = representation->radiusSphereFixed;

				if ( showSes )
				{
					showSphere	 = false;
					showCylinder = false;
					showRibbon	 = false;
				}
				else if ( showSphere && not isSphereRadiusFixed )
				{
					showCylinder = false;
					showRibbon	 = false;
				}
				else if ( showSphere && showCylinder )
				{
					if ( isSphereRadiusFixed && sphereRadiusFixed < cylinderRadius )
					{
						sphereRadiusFixed = cylinderRadius;
					}
				}
				else if ( not showSphere && showCylinder )
				{
					showSphere			= true;
					isSphereRadiusFixed = true;
					sphereRadiusFixed	= cylinderRadius;
				}

				buffer.write( sphereRadiusFixed );
				buffer.write( representation->radiusSphereAdd );
				buffer.write( uint( isSphereRadiusFixed ) );
				buffer.write( representation->radiusCylinder );
				buffer.write( uint( representation->cylinderColorBlending ) );
				buffer.write( uint( representation->ribbonColorBlending ) );
				buffer.write( representation->sesProbeRadius );
				buffer.write( Geometry::SES::MAX_PROBE_NEIGHBOR_NB );

				p_representations.emplace(
					index, Cache::Representation { showSphere, showCylinder, showRibbon, showSes }
				);

				index++;
			}

			buffer.close();
			p_context.setBuffer( { "Representations" }, buffer );

			auto handles = p_systems.handles();
			p_dirtyGeometrySystems.insert( handles.begin(), handles.end() );
		}
	};

	struct ResidueLayout
	{
		static void allocateInputs( Context::ContextWrapper & p_context, Layouts & p_layouts )
		{
			p_layouts.residues.resize( p_context );
		}

		static void uploadInput(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle,
			const Cache::System &	  p_system
		)
		{
			if ( p_geometries.ribbons.empty( p_handle ) )
			{
				return;
			}

			uploadIds( p_context, p_layouts, p_geometries, p_handle, p_system );
			uploadTypesAndColors( p_context, p_layouts, p_geometries, p_handle, p_system );
			uploadRepresentations(
				p_context, p_layouts, p_geometries, p_handle, AtomLayout::buildAtomRepresentations( p_system )
			);
			uploadSelection( p_context, p_layouts, p_geometries, p_handle, AtomLayout::buildAtomFlags( p_system ) );
		}

		static void uploadRepresentations(
			Context::ContextWrapper &			 p_context,
			Layouts &							 p_layouts,
			Geometries &						 p_geometries,
			const Desc::Handle					 p_handle,
			std::span<const RepresentationIndex> p_atomRepresentations
		)
		{
			using namespace Layout;

			if ( p_geometries.ribbons.empty( p_handle ) )
			{
				return;
			}

			const auto &					 construction  = p_geometries.ribbons.construction( p_handle );
			const Index						 countResidues = p_layouts.residues.size( p_handle );
			std::vector<RepresentationIndex> residues( countResidues );

			for ( Index i = 0; i < countResidues; ++i )
			{
				const Index atomIndex = construction.residues[ i ].ca;
				assert( atomIndex < p_atomRepresentations.size() );
				residues[ i ] = p_atomRepresentations[ atomIndex ];
			}

			p_layouts.residues.upload<RESIDUE_ATTR::REPRESENTATION, RepresentationIndex>(
				p_context, p_handle, residues
			);
		}

		static void uploadSelection(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle,
			std::span<const Flag>	  p_atomFlags
		)
		{
			using namespace Layout;

			if ( p_geometries.ribbons.empty( p_handle ) )
			{
				return;
			}

			const auto &	  construction = p_geometries.ribbons.construction( p_handle );
			std::vector<Flag> residueFlags( p_layouts.residues.size( p_handle ), 0 );

			for ( Index i = 0; i < residueFlags.size(); ++i )
			{
				const Index atomIndex = construction.residues[ i ].ca;
				assert( atomIndex < p_atomFlags.size() );
				if ( p_atomFlags[ atomIndex ] & toUnderlying( E_ELEMENT_FLAGS::SELECTION ) )
				{
					residueFlags[ i ] |= toUnderlying( E_ELEMENT_FLAGS::SELECTION );
				}
			}

			p_layouts.residues.upload<RESIDUE_ATTR::FLAG, Flag>( p_context, p_handle, residueFlags );
		}

	  private:
		static void uploadIds(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle,
			const Cache::System &	  p_system
		)
		{
			using namespace Layout;

			const auto &	   construction	 = p_geometries.ribbons.construction( p_handle );
			const Index		   countResidues = p_layouts.residues.size( p_handle );
			std::vector<UID32> residueIds( countResidues );

			for ( Index i = 0; i < countResidues; ++i )
			{
				const Index residueIndex = construction.residues[ i ].index;
				residueIds[ i ]			 = p_system.residueUids.first + residueIndex;
			}

			p_layouts.residues.upload<RESIDUE_ATTR::ID, UID32>( p_context, p_handle, residueIds );
		}

		static void uploadTypesAndColors(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle,
			const Cache::System &	  p_system
		)
		{
			using namespace Layout;

			const auto &			construction  = p_geometries.ribbons.construction( p_handle );
			const Index				countResidues = p_layouts.residues.size( p_handle );
			std::vector<uint8_t>	residueTypes( countResidues );
			std::vector<ColorIndex> residueColors( countResidues );

			for ( Index i = 0; i < countResidues; ++i )
			{
				const Index residueIndex = construction.residues[ i ].index;
				auto		ss			 = p_system.data.residueSecondaryStructureTypes[ residueIndex ];
				residueTypes[ i ]		 = toUnderlying( ss );
				residueColors[ i ]		 = Color::getColorIndex( ss );
			}

			p_layouts.residues.upload<RESIDUE_ATTR::TYPE, uint8_t>( p_context, p_handle, residueTypes );
			p_layouts.residues.upload<RESIDUE_ATTR::COLOR, ColorIndex>( p_context, p_handle, residueColors );
		}
	};

	struct RibbonGeometry
	{
		static void uploadPositions(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle,
			std::span<const Vec3f>	  p_positions
		)
		{
			using namespace Layout;

			if ( not p_geometries.ribbons.built( p_handle ) )
			{
				return;
			}

			SystemVisibility::uploadRibbonPositions( p_context, p_layouts, p_geometries, p_handle, p_positions );
		}

		static void writeOutput(
			Context::ContextWrapper & p_context,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle
		)
		{
			if ( not p_geometries.ribbons.empty( p_handle ) )
			{
				p_geometries.ribbons.uploadIndexes( p_context, p_handle );
			}
		}
	};

	struct GeometryBuffers
	{
		static void allocateOutputs( Context::ContextWrapper & p_context, Geometries & p_geometries )
		{
			p_geometries.resizeSystems( p_context );
		}

		static void writeOutput(
			Context::ContextWrapper & p_context,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle
		)
		{
			p_geometries.spheres.uploadIndexes( p_context, p_handle );
			p_geometries.cylinders.uploadIndexes( p_context, p_handle );
		}
	};

	struct SystemModels
	{
		template<typename Systems>
		static void upload( Context::ContextWrapper & p_context, Systems & p_systems, const Cache::Camera & p_camera )
		{
			if ( p_systems.empty() )
			{
				return;
			}

			BinaryBuffer430 buffer;
			for ( const Cache::System & system : p_systems )
			{
				const Mat4f matrixModelView	   = p_camera.matView * system.transform;
				const Mat4f matrixModelViewInv = Util::Math::inverse( matrixModelView );
				const Mat4f matrixNormal	   = Util::Math::transpose( matrixModelViewInv );

				buffer.write( matrixModelView );
				buffer.write( matrixModelViewInv );
				buffer.write( matrixNormal );
			}

			buffer.close();
			p_context.setBuffer( { "Models" }, buffer );
		}
	};

	struct DrawRanges
	{
		static void buildDrawRanges( Context::ContextWrapper & p_context, Geometries & p_geometries )
		{
			p_geometries.buildDrawRanges( p_context );
		}
	};
} // namespace VTX::Renderer::Builder

#endif
