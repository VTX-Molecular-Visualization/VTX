#include "app/helper/system.hpp"
#include "app/system/color.hpp"
#include "app/system/metadata.hpp"
#include "app/system/representation.hpp"
#include <core/struct/topology.hpp>

namespace
{
	VTX::App::System::E_VISIBLE_STATE _getVisibleState(
		const VTX::Util::Math::BitSet &		p_visibility,
		const VTX::Core::Struct::IndexRange p_range
	)
	{
		using namespace VTX::App::System;

		return p_visibility.test( p_range )	 ? E_VISIBLE_STATE::VISIBLE
			   : p_visibility.any( p_range ) ? E_VISIBLE_STATE::PARTIAL
											 : E_VISIBLE_STATE::HIDDEN;
	}

	VTX::App::System::E_SELECTION_STATE _getSelectionState(
		const VTX::Util::Math::BitSet &		p_selection,
		const VTX::Core::Struct::IndexRange p_range
	)
	{
		using namespace VTX::App::System;

		return p_selection.test( p_range )	? E_SELECTION_STATE::FULL
			   : p_selection.any( p_range ) ? E_SELECTION_STATE::PARTIAL
											: E_SELECTION_STATE::NONE;
	}
} // namespace

namespace VTX::App::Helper::System
{
	ECS::Entity getSystemByName( const std::string_view p_name )
	{
		for ( auto e : REG().view<App::System::Metadata>() )
		{
			auto & metadata = REG().get<App::System::Metadata>( e );

			if ( metadata.name == p_name )
				return e;
		}

		return ECS::InvalidEntity;
	}

	ECS::Entity getSystemByPdb( const std::string_view p_pdb )
	{
		for ( auto e : REG().view<App::System::Metadata>() )
		{
			auto & metadata = REG().get<App::System::Metadata>( e );

			if ( metadata.pdbIDCode == p_pdb )
				return e;
		}

		return ECS::InvalidEntity;
	}

	App::System::E_VISIBLE_STATE getVisibleState( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const ECS::Entity ent		 = p_system.entity;
		const auto &	  reg		 = REG();
		const auto &	  topology	 = reg.get<Core::Struct::Topology>( ent );
		const auto &	  visibility = reg.get<App::System::Visibility>( ent );

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM: return _getVisibleState( visibility.atoms, topology.getAtomRange() );
		case E_SYSTEM_ITEM::CHAIN:
			assert( p_system.index );
			return _getVisibleState( visibility.atoms, topology.getChainAtomRange( *p_system.index ) );
		case E_SYSTEM_ITEM::RESIDUE:
			assert( p_system.index );
			return _getVisibleState( visibility.atoms, topology.getResidueAtomRange( *p_system.index ) );
		case E_SYSTEM_ITEM::ATOM:
			assert( p_system.index );
			return visibility.atoms.test( *p_system.index ) ? E_VISIBLE_STATE::VISIBLE : E_VISIBLE_STATE::HIDDEN;
		default: break;
		}

		return E_VISIBLE_STATE::HIDDEN;
	}

	App::System::E_SELECTION_STATE getSelectionState( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const ECS::Entity ent		= p_system.entity;
		const auto &	  reg		= REG();
		const auto &	  topology	= reg.get<Core::Struct::Topology>( ent );
		const auto &	  selection = reg.get<App::System::Selection>( ent );

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM: return _getSelectionState( selection.atoms, topology.getAtomRange() );
		case E_SYSTEM_ITEM::CHAIN:
			assert( p_system.index );
			return _getSelectionState( selection.atoms, topology.getChainAtomRange( *p_system.index ) );
		case E_SYSTEM_ITEM::RESIDUE:
			assert( p_system.index );
			return _getSelectionState( selection.atoms, topology.getResidueAtomRange( *p_system.index ) );
		case E_SYSTEM_ITEM::ATOM:
			assert( p_system.index );
			return selection.atoms.test( *p_system.index ) ? E_SELECTION_STATE::FULL : E_SELECTION_STATE::NONE;
		default: break;
		}

		return E_SELECTION_STATE::NONE;
	}

	std::optional<App::System::E_COLOR_SCHEME> getColorScheme( const SystemItemView & p_system )
	{
		using namespace Core::Struct;

		const auto & reg	  = REG();
		const auto & topology = reg.get<Core::Struct::Topology>( p_system.entity );
		const auto & color	  = reg.get<App::System::Color>( p_system.entity );
		IndexRange	 atoms;

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM: atoms = topology.getAtomRange(); break;
		case E_SYSTEM_ITEM::CHAIN:
			assert( p_system.index );
			atoms = topology.getChainAtomRange( *p_system.index );
			break;
		case E_SYSTEM_ITEM::RESIDUE:
			assert( p_system.index );
			atoms = topology.getResidueAtomRange( *p_system.index );
			break;
		case E_SYSTEM_ITEM::ATOM:
			assert( p_system.index );
			atoms = IndexRange( *p_system.index );
			break;
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
				return App::System::E_COLOR_SCHEME::CUSTOM;
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

		return count;
	}

	bool isColorSchemeRoot( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const ECS::Entity ent	   = p_system.entity;
		const auto &	  reg	   = REG();
		const auto &	  topology = reg.get<Core::Struct::Topology>( ent );
		const auto &	  color	   = reg.get<Color>( ent );

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
			case E_SYSTEM_ITEM::CHAIN:
			{
				assert( p_system.index );
				const IndexRange systemRange = topology.getAtomRange();
				if ( ranges.contains( topology.getChainAtomRange( *p_system.index ) )
					 && not ranges.contains( systemRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::RESIDUE:
			{
				assert( p_system.index );
				const IndexRange chainRange
					= topology.getChainAtomRange( topology.getResidueChainIndex( *p_system.index ) );
				if ( ranges.contains( topology.getResidueAtomRange( *p_system.index ) )
					 && not ranges.contains( chainRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::ATOM:
			{
				assert( p_system.index );
				const IndexRange residueRange
					= topology.getResidueAtomRange( topology.getAtomResidueIndex( *p_system.index ) );
				if ( ranges.contains( *p_system.index ) && not ranges.contains( residueRange ) )
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

		return false;
	}

	std::optional<ECS::Entity> getRepresentation( const SystemItemView & p_system )
	{
		using namespace Core::Struct;

		const auto & reg			= REG();
		const auto & topology		= reg.get<Core::Struct::Topology>( p_system.entity );
		const auto & representation = reg.get<App::System::Representation>( p_system.entity );
		IndexRange	 atoms;

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM: atoms = topology.getAtomRange(); break;
		case E_SYSTEM_ITEM::CHAIN:
			assert( p_system.index );
			atoms = topology.getChainAtomRange( *p_system.index );
			break;
		case E_SYSTEM_ITEM::RESIDUE:
			assert( p_system.index );
			atoms = topology.getResidueAtomRange( *p_system.index );
			break;
		case E_SYSTEM_ITEM::ATOM:
			assert( p_system.index );
			atoms = IndexRange( *p_system.index );
			break;
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

		const ECS::Entity ent			 = p_system.entity;
		const auto &	  reg			 = REG();
		const auto &	  topology		 = reg.get<Core::Struct::Topology>( ent );
		const auto &	  representation = reg.get<Representation>( ent );

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
			case E_SYSTEM_ITEM::CHAIN:
			{
				assert( p_system.index );
				const IndexRange systemRange = topology.getAtomRange();
				if ( ranges.contains( topology.getChainAtomRange( *p_system.index ) )
					 && not ranges.contains( systemRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::RESIDUE:
			{
				assert( p_system.index );
				const IndexRange chainRange
					= topology.getChainAtomRange( topology.getResidueChainIndex( *p_system.index ) );
				if ( ranges.contains( topology.getResidueAtomRange( *p_system.index ) )
					 && not ranges.contains( chainRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::ATOM:
			{
				assert( p_system.index );
				const IndexRange residueRange
					= topology.getResidueAtomRange( topology.getAtomResidueIndex( *p_system.index ) );
				if ( ranges.contains( *p_system.index ) && not ranges.contains( residueRange ) )
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
