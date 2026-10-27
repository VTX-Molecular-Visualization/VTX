#include "app/helper/system.hpp"
#include "app/helper/trajectory.hpp"
#include "app/system/color.hpp"
#include "app/system/representation.hpp"
#include <algorithm>
#include <core/struct/topology.hpp>
#include <stdexcept>
#include <util/exceptions.hpp>

namespace
{
	VTX::App::System::E_VISIBLE_STATE _getVisibleState( const VTX::Util::Math::BitSet &		p_visibility,
														const VTX::Core::Struct::IndexRange p_range )
	{
		using namespace VTX::App::System;

		return p_visibility.test( p_range )	 ? E_VISIBLE_STATE::VISIBLE
			   : p_visibility.any( p_range ) ? E_VISIBLE_STATE::PARTIAL
											 : E_VISIBLE_STATE::HIDDEN;
	}

	VTX::App::System::E_SELECTION_STATE _getSelectionState( const VTX::Util::Math::BitSet &		p_selection,
															const VTX::Core::Struct::IndexRange p_range )
	{
		using namespace VTX::App::System;

		return p_selection.test( p_range )	? E_SELECTION_STATE::FULL
			   : p_selection.any( p_range ) ? E_SELECTION_STATE::PARTIAL
											: E_SELECTION_STATE::NONE;
	}

	VTX::Index _getIndex( const VTX::App::Helper::System::SystemItemView & p_system )
	{
		if ( not p_system.index )
		{
			throw VTX::VTXException( "System item {} requires an index", VTX::toUnderlying( p_system.item ) );
		}
		return *p_system.index;
	}

} // namespace

namespace VTX::App::Helper::System
{
	Vec3f getAtomPosition( const Entity p_entity, const Index p_atom )
	{
		Vec3f	   position;
		const bool available = App::Helper::Trajectory::visitCurrentFrame(
			p_entity,
			[ & ]( const Core::Struct::FrameView p_positions )
			{
				if ( p_atom >= p_positions.size() )
				{
					throw std::out_of_range( "Atom position is unavailable." );
				}
				position = p_positions[ p_atom ];
			} );
		if ( not available )
		{
			throw std::out_of_range( "Atom position is unavailable." );
		}

		return position;
	}

	App::System::E_VISIBLE_STATE getVisibleState( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const Entity ent		= p_system.entity;
		const auto & reg		= REG();
		const auto & topology	= reg.get<Core::Struct::Topology>( ent );
		const auto & visibility = reg.get<App::System::Visibility>( ent );

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM: return _getVisibleState( visibility.atoms, topology.getAtomRange() );
		case E_SYSTEM_ITEM::CATEGORY:
		{
			const IndexRangeList atoms = topology.getCategoryAtomRangeList( _getIndex( p_system ) );
			if ( atoms.isEmpty() )
			{
				return E_VISIBLE_STATE::HIDDEN;
			}
			return visibility.atoms.test( atoms )  ? E_VISIBLE_STATE::VISIBLE
				   : visibility.atoms.any( atoms ) ? E_VISIBLE_STATE::PARTIAL
												   : E_VISIBLE_STATE::HIDDEN;
		}
		case E_SYSTEM_ITEM::CHAIN:
			return _getVisibleState( visibility.atoms, topology.getChainAtomRange( _getIndex( p_system ) ) );
		case E_SYSTEM_ITEM::RESIDUE:
			return _getVisibleState( visibility.atoms, topology.getResidueAtomRange( _getIndex( p_system ) ) );
		case E_SYSTEM_ITEM::ATOM:
			return visibility.atoms.test( _getIndex( p_system ) ) ? E_VISIBLE_STATE::VISIBLE : E_VISIBLE_STATE::HIDDEN;
		default: break;
		}

		return E_VISIBLE_STATE::HIDDEN;
	}

	App::System::E_SELECTION_STATE getSelectionState( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const Entity ent	   = p_system.entity;
		const auto & reg	   = REG();
		const auto & topology  = reg.get<Core::Struct::Topology>( ent );
		const auto & selection = reg.get<App::System::Selection>( ent );

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM: return _getSelectionState( selection.atoms, topology.getAtomRange() );
		case E_SYSTEM_ITEM::CATEGORY:
		{
			const IndexRangeList atoms = topology.getCategoryAtomRangeList( _getIndex( p_system ) );
			if ( atoms.isEmpty() )
			{
				return E_SELECTION_STATE::NONE;
			}
			return selection.atoms.test( atoms )  ? E_SELECTION_STATE::FULL
				   : selection.atoms.any( atoms ) ? E_SELECTION_STATE::PARTIAL
												  : E_SELECTION_STATE::NONE;
		}
		case E_SYSTEM_ITEM::CHAIN:
			return _getSelectionState( selection.atoms, topology.getChainAtomRange( _getIndex( p_system ) ) );
		case E_SYSTEM_ITEM::RESIDUE:
			return _getSelectionState( selection.atoms, topology.getResidueAtomRange( _getIndex( p_system ) ) );
		case E_SYSTEM_ITEM::ATOM:
			return selection.atoms.test( _getIndex( p_system ) ) ? E_SELECTION_STATE::FULL : E_SELECTION_STATE::NONE;
		default: break;
		}

		return E_SELECTION_STATE::NONE;
	}

	std::optional<Renderer::E_COLOR_SCHEME> getColorScheme( const SystemItemView & p_system )
	{
		using namespace Core::Struct;

		const auto &   reg		= REG();
		const auto &   topology = reg.get<Core::Struct::Topology>( p_system.entity );
		const auto &   color	= reg.get<App::System::Color>( p_system.entity );
		IndexRangeList atoms;

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM: atoms.addRange( topology.getAtomRange() ); break;
		case E_SYSTEM_ITEM::CATEGORY: atoms = topology.getCategoryAtomRangeList( _getIndex( p_system ) ); break;
		case E_SYSTEM_ITEM::CHAIN: atoms.addRange( topology.getChainAtomRange( _getIndex( p_system ) ) ); break;
		case E_SYSTEM_ITEM::RESIDUE: atoms.addRange( topology.getResidueAtomRange( _getIndex( p_system ) ) ); break;
		case E_SYSTEM_ITEM::ATOM: atoms.addRange( IndexRange( _getIndex( p_system ) ) ); break;
		default: break;
		}

		for ( const auto & [ scheme, rangeList ] : color.colorSchemeAtoms )
		{
			if ( rangeList.contains( atoms ) )
			{
				return scheme;
			}
		}
		for ( const auto & [ _, rangeList ] : color.customColorAtoms )
		{
			if ( rangeList.contains( atoms ) )
			{
				return Renderer::E_COLOR_SCHEME::CUSTOM;
			}
		}
		for ( const auto & [ _, rangeList ] : color.carbonCustomColorAtoms )
		{
			if ( rangeList.contains( atoms ) )
			{
				return Renderer::E_COLOR_SCHEME::CARBON_CUSTOM;
			}
		}

		return std::nullopt;
	}

	std::optional<Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE> getSecondaryStructureColorScheme(
		const SystemItemView & p_system )
	{
		using namespace Core::Struct;

		const auto &   reg		= REG();
		const auto &   topology = reg.get<Core::Struct::Topology>( p_system.entity );
		const auto &   color	= reg.get<App::System::Color>( p_system.entity );
		IndexRangeList residues;

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM:
			residues.addRange( IndexRange::fromFirstCount( 0, topology.getResidueCount() ) );
			break;
		case E_SYSTEM_ITEM::CATEGORY:
			for ( const Index residue :
				  topology.getCategoryResidues( static_cast<Core::ChemDB::Category::TYPE>( _getIndex( p_system ) ) ) )
			{
				residues.addRange( IndexRange( residue ) );
			}
			break;
		case E_SYSTEM_ITEM::CHAIN: residues.addRange( topology.getChainResidueRange( _getIndex( p_system ) ) ); break;
		case E_SYSTEM_ITEM::RESIDUE: residues.addRange( IndexRange( _getIndex( p_system ) ) ); break;
		case E_SYSTEM_ITEM::ATOM:
			residues.addRange( IndexRange( topology.getAtomResidueIndex( _getIndex( p_system ) ) ) );
			break;
		default: break;
		}

		for ( const auto & [ scheme, rangeList ] : color.colorSchemeSecondaryStructureResidues )
		{
			if ( rangeList.contains( residues ) )
			{
				return scheme;
			}
		}
		for ( const auto & [ _, rangeList ] : color.customSecondaryStructureColorResidues )
		{
			if ( rangeList.contains( residues ) )
			{
				return Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE::CUSTOM;
			}
		}

		return std::nullopt;
	}

	size_t countAssignedColorAtoms( const App::System::Color & p_color )
	{
		size_t count = 0;

		for ( const auto & [ _, rangeList ] : p_color.colorSchemeAtoms )
		{
			count += rangeList.count();
		}
		for ( const auto & [ _, rangeList ] : p_color.customColorAtoms )
		{
			count += rangeList.count();
		}
		for ( const auto & [ _, rangeList ] : p_color.carbonCustomColorAtoms )
		{
			count += rangeList.count();
		}

		return count;
	}

	size_t countAssignedSecondaryStructureResidues( const App::System::Color & p_color )
	{
		size_t count = 0;

		for ( const auto & [ _, rangeList ] : p_color.colorSchemeSecondaryStructureResidues )
		{
			count += rangeList.count();
		}
		for ( const auto & [ _, rangeList ] : p_color.customSecondaryStructureColorResidues )
		{
			count += rangeList.count();
		}

		return count;
	}

	bool isColorSchemeRoot( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const Entity ent	  = p_system.entity;
		const auto & reg	  = REG();
		const auto & topology = reg.get<Core::Struct::Topology>( ent );
		const auto & color	  = reg.get<Color>( ent );

		auto isRootForRanges = [ & ]( const Core::Struct::IndexRangeList & ranges )
		{
			// Check if indexes contained but not parent.
			switch ( p_system.item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
				if ( ranges.count() == topology.getAtomCount() )
				{
					return true;
				}
				break;
			case E_SYSTEM_ITEM::CATEGORY:
			{
				const IndexRange systemRange = topology.getAtomRange();
				if ( ranges.contains( topology.getCategoryAtomRangeList( _getIndex( p_system ) ) )
					 && not ranges.contains( systemRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::CHAIN:
			{
				const IndexRange systemRange = topology.getAtomRange();
				if ( ranges.contains( topology.getChainAtomRange( _getIndex( p_system ) ) )
					 && not ranges.contains( systemRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::RESIDUE:
			{
				const IndexRange chainRange
					= topology.getChainAtomRange( topology.getResidueChainIndex( _getIndex( p_system ) ) );
				if ( ranges.contains( topology.getResidueAtomRange( _getIndex( p_system ) ) )
					 && not ranges.contains( chainRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::ATOM:
			{
				const IndexRange residueRange
					= topology.getResidueAtomRange( topology.getAtomResidueIndex( _getIndex( p_system ) ) );
				if ( ranges.contains( _getIndex( p_system ) ) && not ranges.contains( residueRange ) )
				{
					return true;
				}
				break;
			}
			default: break;
			}

			return false;
		};

		for ( const auto & [ _, ranges ] : color.colorSchemeAtoms )
		{
			if ( isRootForRanges( ranges ) )
			{
				return true;
			}
		}
		for ( const auto & [ _, ranges ] : color.customColorAtoms )
		{
			if ( isRootForRanges( ranges ) )
			{
				return true;
			}
		}
		for ( const auto & [ _, ranges ] : color.carbonCustomColorAtoms )
		{
			if ( isRootForRanges( ranges ) )
			{
				return true;
			}
		}

		auto isRootForResidueRanges = [ & ]( const Core::Struct::IndexRangeList & ranges )
		{
			switch ( p_system.item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
				if ( ranges.count() == topology.getResidueCount() )
				{
					return true;
				}
				break;
			case E_SYSTEM_ITEM::CATEGORY:
			{
				Core::Struct::IndexRangeList categoryResidues;
				for ( const Index residue : topology.getCategoryResidues(
						  static_cast<Core::ChemDB::Category::TYPE>( _getIndex( p_system ) )
					  ) )
				{
					categoryResidues.addRange( Core::Struct::IndexRange( residue ) );
				}
				const Core::Struct::IndexRange systemRange
					= Core::Struct::IndexRange::fromFirstCount( 0, topology.getResidueCount() );
				if ( ranges.contains( categoryResidues ) && not ranges.contains( systemRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::CHAIN:
			{
				const Core::Struct::IndexRange systemRange
					= Core::Struct::IndexRange::fromFirstCount( 0, topology.getResidueCount() );
				if ( ranges.contains( topology.getChainResidueRange( _getIndex( p_system ) ) )
					 && not ranges.contains( systemRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::RESIDUE:
			{
				const Core::Struct::IndexRange chainRange
					= topology.getChainResidueRange( topology.getResidueChainIndex( _getIndex( p_system ) ) );
				if ( ranges.contains( _getIndex( p_system ) ) && not ranges.contains( chainRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::ATOM:
			{
				const Index					   residueIndex = topology.getAtomResidueIndex( _getIndex( p_system ) );
				const Core::Struct::IndexRange chainRange
					= topology.getChainResidueRange( topology.getResidueChainIndex( residueIndex ) );
				if ( ranges.contains( residueIndex ) && not ranges.contains( chainRange ) )
				{
					return true;
				}
				break;
			}
			default: break;
			}

			return false;
		};

		for ( const auto & [ _, ranges ] : color.colorSchemeSecondaryStructureResidues )
		{
			if ( isRootForResidueRanges( ranges ) )
			{
				return true;
			}
		}
		return std::ranges::any_of( color.customSecondaryStructureColorResidues,
									[ &isRootForResidueRanges ]( const auto & entry )
									{ return isRootForResidueRanges( entry.second ); } );
	}

	std::optional<Entity> getRepresentation( const SystemItemView & p_system )
	{
		using namespace Core::Struct;

		const auto &   reg			  = REG();
		const auto &   topology		  = reg.get<Core::Struct::Topology>( p_system.entity );
		const auto &   representation = reg.get<App::System::Representation>( p_system.entity );
		IndexRangeList atoms;

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM: atoms.addRange( topology.getAtomRange() ); break;
		case E_SYSTEM_ITEM::CATEGORY: atoms = topology.getCategoryAtomRangeList( _getIndex( p_system ) ); break;
		case E_SYSTEM_ITEM::CHAIN: atoms.addRange( topology.getChainAtomRange( _getIndex( p_system ) ) ); break;
		case E_SYSTEM_ITEM::RESIDUE: atoms.addRange( topology.getResidueAtomRange( _getIndex( p_system ) ) ); break;
		case E_SYSTEM_ITEM::ATOM: atoms.addRange( IndexRange( _getIndex( p_system ) ) ); break;
		default: break;
		}

		for ( const auto & [ scheme, rangeList ] : representation.presetAtoms )
		{
			if ( rangeList.contains( atoms ) )
			{
				return scheme;
			}
		}

		return std::nullopt;
	}

	bool isRepresentationRoot( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const Entity ent			= p_system.entity;
		const auto & reg			= REG();
		const auto & topology		= reg.get<Core::Struct::Topology>( ent );
		const auto & representation = reg.get<Representation>( ent );

		for ( const auto & [ _, ranges ] : representation.presetAtoms )
		{
			// Check if indexes contained but not parent.
			switch ( p_system.item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
				if ( ranges.count() == topology.getAtomCount() )
				{
					return true;
				}
				break;
			case E_SYSTEM_ITEM::CATEGORY:
			{
				const IndexRange systemRange = topology.getAtomRange();
				if ( ranges.contains( topology.getCategoryAtomRangeList( _getIndex( p_system ) ) )
					 && not ranges.contains( systemRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::CHAIN:
			{
				const IndexRange systemRange = topology.getAtomRange();
				if ( ranges.contains( topology.getChainAtomRange( _getIndex( p_system ) ) )
					 && not ranges.contains( systemRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::RESIDUE:
			{
				const IndexRange chainRange
					= topology.getChainAtomRange( topology.getResidueChainIndex( _getIndex( p_system ) ) );
				if ( ranges.contains( topology.getResidueAtomRange( _getIndex( p_system ) ) )
					 && not ranges.contains( chainRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::ATOM:
			{
				const IndexRange residueRange
					= topology.getResidueAtomRange( topology.getAtomResidueIndex( _getIndex( p_system ) ) );
				if ( ranges.contains( _getIndex( p_system ) ) && not ranges.contains( residueRange ) )
				{
					return true;
				}
				break;
			}
			default: break;
			}
		}

		return false;
	}

} // namespace VTX::App::Helper::System
