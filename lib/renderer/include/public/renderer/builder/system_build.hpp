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
#include <cmath>
#include <cstdint>
#include <limits>
#include <unordered_set>
#include <util/chrono.hpp>
#include <util/constants.hpp>
#include <util/enum.hpp>
#include <util/math.hpp>

namespace VTX::Renderer::Builder
{
	struct SystemRegistry
	{
		static void clear(
			Layouts &						   p_layouts,
			Geometries &					   p_geometries,
			std::unordered_set<Desc::Handle> & p_geometryRefreshSystems
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] SystemRegistry::clear" );

			p_layouts.clearSystems();
			p_geometries.clearSystems();
			p_geometryRefreshSystems.clear();
		}

		static void clearSystemRanges(
			Layouts &						   p_layouts,
			Geometries &					   p_geometries,
			std::unordered_set<Desc::Handle> & p_geometryRefreshSystems
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] SystemRegistry::clearSystemRanges" );

			p_layouts.clearSystems();
			p_geometries.clearSystemRanges();
			p_geometryRefreshSystems.clear();
		}

		static void removeSystemConstruction(
			Context::ContextWrapper & p_context,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] SystemRegistry::removeSystemConstruction" );

			p_geometries.removeSystemConstruction( p_context, p_handle );
		}

		template<typename Systems>
		static void registerSystem(
			Systems &	 p_systems,
			Geometries & p_geometries,
			Layouts &	 p_layouts,
			Desc::Handle p_handle
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] SystemRegistry::registerSystem" );

			const Cache::System & system = p_systems.get( p_handle );

			p_geometries.registerSystem( p_handle, system );
			p_layouts.atoms.add( p_handle, p_geometries.spheres.size( p_handle ) );
		}
	};

	struct AtomLayout
	{
		static void allocateInputs( Context::ContextWrapper & p_context, Layouts & p_layouts )
		{
			// Util::ScopedChrono timer( "[BUILDER] AtomLayout::allocateInputs" );

			p_layouts.atoms.resize( p_context );
		}

		template<typename Systems>
		static void uploadInput(
			Context::ContextWrapper &		   p_context,
			Systems &						   p_systems,
			Layouts &						   p_layouts,
			const Desc::Handle				   p_handle,
			std::unordered_set<Desc::Handle> & p_geometryRefreshSystems
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] AtomLayout::uploadInput" );

			const Cache::System & system		  = p_systems.get( p_handle );
			const auto			  colors		  = buildAtomColors( system );
			const auto			  representations = buildAtomRepresentations( system );
			const auto			  flags			  = buildAtomFlags( system );
			const auto			  atomIds		  = system.data.atomUids->toStdVector();

			uploadSymbols( p_context, p_layouts, p_handle, system.data.topology->atomSymbols );
			uploadIds( p_context, p_layouts, p_handle, atomIds );
			uploadPositions( p_context, p_systems, p_layouts, p_handle, system.data.trajectory );
			uploadColors( p_context, p_layouts, p_handle, colors );
			uploadRepresentations( p_context, p_layouts, p_handle, representations, p_geometryRefreshSystems );
			uploadSelection( p_context, p_layouts, p_handle, flags );
		}

		static std::vector<ColorIndex> buildAtomColors( const Cache::System & p_system )
		{
			const Core::Struct::Topology & data = *p_system.data.topology;
			std::vector<ColorIndex>		   atoms( data.getAtomCount() );
			size_t						   count = 0;

			for ( const auto & [ scheme, ranges ] : *p_system.data.colorSchemeAtoms )
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
			for ( const auto & [ colorIndex, ranges ] : *p_system.data.customColorAtoms )
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
			constexpr RepresentationIndex invalidRepresentation = std::numeric_limits<RepresentationIndex>::max();

			std::vector<RepresentationIndex> atoms( p_system.data.topology->getAtomCount(), invalidRepresentation );
			size_t							 count = 0;
			for ( const auto & [ preset, ranges ] : *p_system.data.presetAtoms )
			{
				const auto representationIt = p_system.data.representationHandles->find( preset );
				assert( representationIt != p_system.data.representationHandles->end() );
				assert( representationIt->second < invalidRepresentation );

				const auto representationIndex = static_cast<RepresentationIndex>( representationIt->second );

				for ( auto it = ranges.rangeBegin(); it != ranges.rangeEnd(); ++it )
				{
					assert( it->getLast() <= atoms.size() );
					for ( Index atom = it->getFirst(); atom < it->getLast(); ++atom )
					{
						assert( atoms[ atom ] == invalidRepresentation );
						atoms[ atom ] = representationIndex;
					}
				}
				count += ranges.count();
			}

			assert( count == p_system.data.topology->getAtomCount() );

			return atoms;
		}

		static std::vector<Flag> buildAtomFlags( const Cache::System & p_system )
		{
			std::vector<Flag> atomFlags( p_system.data.topology->getAtomCount(), 0 );
			p_system.data.selection->forEachSetBit(
				[ & ]( const size_t p_atom ) { atomFlags[ p_atom ] |= toUnderlying( E_ELEMENT_FLAGS::SELECTION ); }
			);

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
			// Util::ScopedChrono timer( "[BUILDER] AtomLayout::refreshColors" );

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
			// Util::ScopedChrono timer( "[BUILDER] AtomLayout::uploadPositions" );

			using namespace Layout;

			assert( p_positions.size() == p_layouts.atoms.size( p_handle ) );
			p_systems.get( p_handle ).data.trajectory = p_positions;
			p_layouts.atoms.upload<ATOM_ATTR::POSITION, Vec3f>( p_context, p_handle, p_positions );
		}

		static void uploadColors(
			Context::ContextWrapper &	p_context,
			Layouts &					p_layouts,
			const Desc::Handle			p_handle,
			std::span<const ColorIndex> p_colors
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] AtomLayout::uploadColors" );

			using namespace Layout;

			assert( p_colors.size() == p_layouts.atoms.size( p_handle ) );
			p_layouts.atoms.upload<ATOM_ATTR::COLOR, ColorIndex>( p_context, p_handle, p_colors );
		}

		static void uploadRepresentations(
			Context::ContextWrapper &			 p_context,
			Layouts &							 p_layouts,
			const Desc::Handle					 p_handle,
			std::span<const RepresentationIndex> p_atomRepresentations,
			std::unordered_set<Desc::Handle> &	 p_geometryRefreshSystems
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] AtomLayout::uploadRepresentations" );

			using namespace Layout;

			assert( p_atomRepresentations.size() == p_layouts.atoms.size( p_handle ) );
			p_layouts.atoms.upload<ATOM_ATTR::REPRESENTATION, RepresentationIndex>(
				p_context, p_handle, p_atomRepresentations
			);
			p_geometryRefreshSystems.insert( p_handle );
		}

		static void uploadSelection(
			Context::ContextWrapper & p_context,
			Layouts &				  p_layouts,
			const Desc::Handle		  p_handle,
			std::span<const Flag>	  p_atomFlags
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] AtomLayout::uploadSelection" );

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
			std::unordered_set<Desc::Handle> & p_geometryRefreshSystems
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] SystemVisibility::uploadInput" );

			uploadVisibility( p_systems, p_geometryRefreshSystems, p_handle );
		}

		template<typename Systems>
		static void uploadVisibility(
			Systems &						   p_systems,
			std::unordered_set<Desc::Handle> & p_geometryRefreshSystems,
			const Desc::Handle				   p_handle
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] SystemVisibility::uploadVisibility" );

			assert( p_systems.contains( p_handle ) );
			p_geometryRefreshSystems.insert( p_handle );
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
			// Util::ScopedChrono timer( "[BUILDER] SystemVisibility::refreshGeometryVisibility" );

			const Cache::System & systemCache = p_systems.get( p_handle );

			auto				visibleSpheres	  = *systemCache.data.visibility;
			auto				visibleCylinders  = *systemCache.data.visibility;
			auto				visibleRibbons	  = *systemCache.data.visibility;
			auto				visibleSesAtoms	  = *systemCache.data.visibility;
			bool				requestedRibbon	  = false;
			bool				requestedSes	  = false;
			bool				hasSesProbeRadius = false;
			bool				hasSesComputeMode = false;
			float				sesProbeRadius	  = SES_PROBE_RADIUS_DEFAULT;
			E_SES_COMPUTE_MODE	sesComputeMode	  = SES_COMPUTE_MODE_DEFAULT;
			RepresentationIndex sesRepresentation = 0;

			for ( const auto & [ preset, ranges ] : *systemCache.data.presetAtoms )
			{
				const auto representationIt = systemCache.data.representationHandles->find( preset );
				assert( representationIt != systemCache.data.representationHandles->end() );
				const auto representationIndex = static_cast<RepresentationIndex>( representationIt->second );
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
				else if ( systemCache.data.visibility->any( ranges ) )
				{
					requestedRibbon = true;
				}
				const E_SES_COMPUTE_MODE representationComputeMode = representation.data.rep->sesComputeMode;
				const E_SES_COMPUTE_MODE representationCategories
					= representationComputeMode & ~E_SES_COMPUTE_MODE::MIXED;
				if ( representation.showSes && representationCategories != E_SES_COMPUTE_MODE::NONE )
				{
					requestedSes = true;

					const float representationProbeRadius = representation.data.rep->sesProbeRadius;
					if ( not hasSesProbeRadius )
					{
						sesProbeRadius	  = representationProbeRadius;
						sesRepresentation = representationIndex;
						hasSesProbeRadius = true;
					}
					else if ( std::abs( sesProbeRadius - representationProbeRadius ) > EPSILON )
					{
						VTX_WARNING(
							"Multiple visible SES probe radii on the same system. Using {} and ignoring {}.",
							sesProbeRadius,
							representationProbeRadius
						);
					}
					if ( not hasSesComputeMode )
					{
						sesComputeMode	  = representationComputeMode;
						hasSesComputeMode = true;
					}
					else if ( sesComputeMode != representationComputeMode )
					{
						VTX_WARNING( "Multiple visible SES modes on the same system. Using first one." );
					}
				}
				else
				{
					visibleSesAtoms.subtractInPlace( ranges );
				}
			}

			if ( requestedRibbon && not p_geometries.ribbons.built( p_handle ) )
			{
				constructRibbon( p_systems, p_layouts, p_geometries, p_handle );
			}
			if ( not requestedSes )
			{
				p_geometries.ses.remove( p_context, p_handle );
			}
			else if ( p_geometries.ses.built( p_handle )
				 && ( std::abs( p_geometries.ses.probeRadius( p_handle ) - sesProbeRadius ) > EPSILON
					  || p_geometries.ses.computeMode( p_handle ) != sesComputeMode ) )
			{
				p_geometries.ses.invalidateForRecompute( p_context, p_handle );
				constructSES(
					p_context,
					p_systems,
					p_layouts,
					p_geometries,
					p_handle,
					sesProbeRadius,
					sesComputeMode,
					sesRepresentation
				);
			}
			else if ( not p_geometries.ses.built( p_handle ) )
			{
				constructSES(
					p_context,
					p_systems,
					p_layouts,
					p_geometries,
					p_handle,
					sesProbeRadius,
					sesComputeMode,
					sesRepresentation
				);
			}

			p_geometries.spheres.setVisibility( p_handle, visibleSpheres );
			p_geometries.cylinders.setVisibility( p_handle, visibleCylinders );
			p_geometries.ribbons.setVisibility( p_handle, visibleRibbons );
			p_geometries.ses.setVisibility( p_handle, visibleSesAtoms );
		}

		template<typename Systems>
		static void constructRibbon(
			Systems &		   p_systems,
			Layouts &		   p_layouts,
			Geometries &	   p_geometries,
			const Desc::Handle p_handle
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] SystemVisibility::constructRibbon" );

			const Cache::System & system = p_systems.get( p_handle );

			p_geometries.ribbons.registerSystem( p_handle, system );

			const auto & construction = p_geometries.ribbons.construction( p_handle );
			if ( not construction.isEmpty )
			{
				p_layouts.residues.add( p_handle, static_cast<uint32_t>( construction.residues.size() ) );
			}
		}

		template<typename Systems>
		static void constructSES(
			Context::ContextWrapper & p_context,
			Systems &				  p_systems,
			Layouts &				  p_layouts,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle,
			const float				  p_probeRadius,
			const E_SES_COMPUTE_MODE  p_computeMode,
			const RepresentationIndex p_representation
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] SystemVisibility::constructSES" );

			const Cache::System & system = p_systems.get( p_handle );
			p_geometries.constructSES(
				p_context,
				p_handle,
				system,
				p_layouts.atoms.offset( p_handle ),
				p_probeRadius,
				p_computeMode,
				p_representation
			);
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
				const auto	ss			 = p_system.data.topology->residueSecondaryStructureTypes[ residueIndex ];

				residueIds[ i ]				= p_system.data.residueUids->first + residueIndex;
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
		static Cache::Representation buildCache( const Representation & p_representation )
		{
			bool showSphere	  = p_representation.hasSphere;
			bool showCylinder = p_representation.hasCylinder;
			bool showRibbon	  = p_representation.hasRibbon;
			bool showSes	  = p_representation.hasSes;

			const bool	isSphereRadiusFixed = p_representation.isRadiusSphereFixed;
			const float cylinderRadius		= p_representation.radiusCylinder;
			float		sphereRadiusFixed	= p_representation.radiusSphereFixed;

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
				showSphere = true;
			}

			return Cache::Representation {
				Cache::Representation::Data { &p_representation }, showSphere, showCylinder, showRibbon, showSes
			};
		}

		static void refreshCache( Cache::Representation & p_cache )
		{
			const Cache::Representation refreshed = buildCache( *p_cache.data.rep );

			p_cache.showSphere	 = refreshed.showSphere;
			p_cache.showCylinder = refreshed.showCylinder;
			p_cache.showRibbon	 = refreshed.showRibbon;
			p_cache.showSes		 = refreshed.showSes;
		}

		template<typename RepresentationPool>
		static void upload( Context::ContextWrapper & p_context, const RepresentationPool & p_representations )
		{
			// Util::ScopedChrono timer( "[BUILDER] RepresentationState::upload" );

			BinaryBuffer<E_LAYOUT_TYPE::Std140> buffer;
			if ( p_representations.empty() )
			{
				_write( buffer, VTX::Renderer::Representations::STICKS_AND_RIBBONS );
			}
			else
			{
				Desc::Handle lastHandle = 0;
				for ( const auto entry : p_representations.entries() )
				{
					lastHandle = std::max( lastHandle, entry.handle );
				}

				for ( Desc::Handle handle = 0; handle <= lastHandle; ++handle )
				{
					const Representation & representation = p_representations.contains( handle )
																? *p_representations.get( handle ).data.rep
																: VTX::Renderer::Representations::STICKS_AND_RIBBONS;
					_write( buffer, representation );
				}
			}

			buffer.close();
			p_context.setBuffer( { "Representations" }, buffer );
		}

	  private:
		static void _write( BinaryBuffer<E_LAYOUT_TYPE::Std140> & p_buffer, const Representation & p_representation )
		{
			bool  isSphereRadiusFixed = p_representation.isRadiusSphereFixed;
			float cylinderRadius	  = p_representation.radiusCylinder;
			float sphereRadiusFixed	  = p_representation.radiusSphereFixed;

			if ( p_representation.hasSphere && p_representation.hasCylinder )
			{
				if ( isSphereRadiusFixed && sphereRadiusFixed < cylinderRadius )
				{
					sphereRadiusFixed = cylinderRadius;
				}
			}
			else if ( not p_representation.hasSphere && p_representation.hasCylinder )
			{
				isSphereRadiusFixed = true;
				sphereRadiusFixed	= cylinderRadius;
			}

			p_buffer.write( sphereRadiusFixed );
			p_buffer.write( p_representation.radiusSphereAdd );
			p_buffer.write( uint( isSphereRadiusFixed ) );
			p_buffer.write( p_representation.radiusCylinder );
			p_buffer.write( uint( p_representation.cylinderColorBlending ) );
			p_buffer.write( uint( p_representation.ribbonColorBlending ) );
			p_buffer.write( p_representation.sesProbeRadius );
			p_buffer.write( Geometry::SES::MAX_PROBE_NEIGHBOR_NB );
		}
	};

	struct CameraState
	{
		static void upload(
			Context::ContextWrapper & p_context,
			const Cache::Camera &	  p_camera,
			const size_t			  p_width,
			const size_t			  p_height
		)
		{
			BinaryBuffer140 buffer;
			const Mat4f		matrixViewInv	   = Util::Math::inverse( p_camera.matView );
			const Mat4f		matrixViewInvTrans = Util::Math::transpose( matrixViewInv );

			buffer.write( p_camera.matView );
			buffer.write( p_camera.matProj );
			buffer.write( matrixViewInv );
			buffer.write( matrixViewInvTrans );
			buffer.write( p_camera.position );
			buffer.write( Vec4f(
				p_camera.camera.near * p_camera.camera.far,
				p_camera.camera.far,
				p_camera.camera.far - p_camera.camera.near,
				p_camera.camera.near
			) );
			buffer.write( Vec2i( int( p_width ), int( p_height ) ) );
			buffer.write( Vec2i() );
			buffer.write( uint( p_camera.camera.projection == PROJECTION::PERSPECTIVE ) );
			buffer.close();

			p_context.setBuffer( { "Camera" }, buffer );
		}
	};

	struct GraphicsConfigState
	{
		static void upload( Context::ContextWrapper & p_context, const GraphicsConfig & p_config )
		{
			// Util::ScopedChrono timer( "[BUILDER] GraphicsConfigState::upload" );

			BinaryBuffer140 bufferShading;
			bufferShading.write( p_config.colorBackground );
			bufferShading.write( p_config.colorLight );
			bufferShading.write( p_config.colorFog );
			bufferShading.write( uint32_t( p_config.shadingMode ) );
			bufferShading.write( p_config.specularFactor );
			bufferShading.write( p_config.shininess );
			bufferShading.write( p_config.toonSteps );
			bufferShading.write( p_config.fogNear );
			bufferShading.write( p_config.fogFar );
			bufferShading.write( p_config.activeFog ? p_config.fogDensity : 0.f );
			bufferShading.close();
			p_context.setBuffer( { "Shading" }, bufferShading );

			if ( p_config.activeSSAO )
			{
				BinaryBuffer140 bufferSSAO;
				bufferSSAO.write( p_config.ssaoIntensity );
				bufferSSAO.close();
				p_context.setBuffer( { "SSAO" }, bufferSSAO );

				BinaryBuffer140 bufferBlurX;
				bufferBlurX.write( Vec2i( 1, 0 ) );
				bufferBlurX.write( p_config.blurSize );
				bufferBlurX.close();
				p_context.setBuffer( { "BlurX" }, bufferBlurX );

				BinaryBuffer140 bufferBlurY;
				bufferBlurY.write( Vec2i( 0, 1 ) );
				bufferBlurY.write( p_config.blurSize );
				bufferBlurY.close();
				p_context.setBuffer( { "BlurY" }, bufferBlurY );
			}
			if ( p_config.activeOutline )
			{
				BinaryBuffer140 bufferOutline;
				bufferOutline.write( p_config.colorOutline );
				bufferOutline.write( p_config.outlineSensitivity );
				bufferOutline.write( p_config.outlineThickness );
				bufferOutline.close();
				p_context.setBuffer( { "Outline" }, bufferOutline );
			}
			if ( p_config.activeSelection )
			{
				BinaryBuffer140 bufferSelection;
				bufferSelection.write( p_config.colorSelection );
				bufferSelection.close();
				p_context.setBuffer( { "Selection" }, bufferSelection );
			}
		}
	};

	struct ColorLayoutState
	{
		static void upload( Context::ContextWrapper & p_context, const Color::Layout & p_layout )
		{
			// Util::ScopedChrono timer( "[BUILDER] ColorLayoutState::upload" );

			p_context.setBuffer<Util::Color::Rgba>( { "ColorLayout" }, p_layout.colors );
		}
	};

	struct ResidueLayout
	{
		static void allocateInputs( Context::ContextWrapper & p_context, Layouts & p_layouts )
		{
			// Util::ScopedChrono timer( "[BUILDER] ResidueLayout::allocateInputs" );

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
			// Util::ScopedChrono timer( "[BUILDER] ResidueLayout::uploadInput" );

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
			// Util::ScopedChrono timer( "[BUILDER] ResidueLayout::uploadRepresentations" );

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
			// Util::ScopedChrono timer( "[BUILDER] ResidueLayout::uploadSelection" );

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
			// Util::ScopedChrono timer( "[BUILDER] ResidueLayout::uploadIds" );

			using namespace Layout;

			const auto &	   construction	 = p_geometries.ribbons.construction( p_handle );
			const Index		   countResidues = p_layouts.residues.size( p_handle );
			std::vector<UID32> residueIds( countResidues );

			for ( Index i = 0; i < countResidues; ++i )
			{
				const Index residueIndex = construction.residues[ i ].index;
				residueIds[ i ]			 = p_system.data.residueUids->first + residueIndex;
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
				auto		ss			 = p_system.data.topology->residueSecondaryStructureTypes[ residueIndex ];
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
			// Util::ScopedChrono timer( "[BUILDER] RibbonGeometry::writeOutput" );

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
			// Util::ScopedChrono timer( "[BUILDER] GeometryBuffers::allocateOutputs" );

			p_geometries.resizeSystems( p_context );
		}

		static void writeOutput(
			Context::ContextWrapper & p_context,
			Geometries &			  p_geometries,
			const Desc::Handle		  p_handle
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] GeometryBuffers::writeOutput" );

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
			uint32_t		modelIndex = 0;
			for ( auto entry : p_systems.entries() )
			{
				Cache::System & system = entry.resource;
				system.modelIndex	   = modelIndex++;

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
		template<typename Systems>
		static void buildDrawRanges(
			Context::ContextWrapper & p_context,
			Geometries &			  p_geometries,
			Systems &				  p_systems
		)
		{
			// Util::ScopedChrono timer( "[BUILDER] DrawRanges::buildDrawRanges" );

			p_geometries.buildDrawRanges( p_context, p_systems );
		}
	};
} // namespace VTX::Renderer::Builder

#endif
