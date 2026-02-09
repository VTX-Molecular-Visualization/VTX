#include "ui/qt/model/system_model.hpp"
#include "app/system/uid.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <variant>

namespace VTX::UI::QT::Model
{
	SystemModel::SystemModel( const App::ECS::Entity p_system, QObject * p_parent ) :
		_system( p_system ), _data( App::REG().get<Core::Struct::System>( p_system ) ), QAbstractItemModel( p_parent )
	{
	}

	int SystemModel::columnCount( const QModelIndex & p_parent ) const { return 1; }

	int SystemModel::rowCount( const QModelIndex & p_parent ) const
	{
		using namespace Core::Struct;

		// Top level.
		if ( not p_parent.isValid() )
		{
			return 1;
		}

		E_SYSTEM_ITEM item;
		Index		  index;
		unpack( p_parent.internalId(), item, index );

		switch ( item )
		{
		case E_SYSTEM_ITEM::SYSTEM:
		{
			return _data.get().getChainCount();
		}
		case E_SYSTEM_ITEM::CHAIN:
		{
			if ( index >= _data.get().getChainCount() )
			{
				return 0;
			}

			return _data.get().chainResidueCounts[ index ];
		}
		case E_SYSTEM_ITEM::RESIDUE:
		{
			if ( index >= _data.get().getResidueCount() )
			{
				return 0;
			}

			return _data.get().residueAtomCounts[ index ];
		}
		default: return 0;
		}
	}

	QVariant SystemModel::data( const QModelIndex & p_index, int p_role ) const
	{
		using namespace Core::Struct;
		using namespace Style;

		if ( not p_index.isValid() )
		{
			return {};
		}

		E_SYSTEM_ITEM item;
		Index		  index;
		unpack( p_index.internalId(), item, index );

		switch ( p_role )
		{
		case Qt::DisplayRole:

			switch ( item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
			{
				return QString::fromStdString( _data.get().name );
			}
			case E_SYSTEM_ITEM::CHAIN:
			{
				assert( index < _data.get().getChainCount() );
				return QString::fromStdString( _data.get().chainNames[ index ] );
			}
			case E_SYSTEM_ITEM::RESIDUE:
			{
				assert( index < _data.get().getResidueCount() );
				return QString::fromStdString( _data.get().residueNames[ index ] );
			}
			case E_SYSTEM_ITEM::ATOM:
			{
				assert( index < _data.get().getAtomCount() );
				return QString::fromStdString( _data.get().atomNames[ index ] );
			}
			}
			return {};
		case ItemRole: return toUnderlying( item );
		case Qt::DecorationRole:
			switch ( item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
			{
				return STYLE().iconFromCodepoint( Icons::HUB );
			}
			case E_SYSTEM_ITEM::CHAIN:
			{
				return STYLE().iconFromCodepoint( Icons::TIMELINE );
			}
			case E_SYSTEM_ITEM::RESIDUE:
			{
				return STYLE().iconFromCodepoint( Icons::ATR );
			}
			case E_SYSTEM_ITEM::ATOM:
			{
				return {};
			}
			}
		case VisibleRole: return toUnderlying( App::Helper::System::getVisibleState( { _system, item, index } ) );
		case ColorSchemeRootRole: return App::Helper::System::isColorSchemeRoot( { _system, item, index } );
		case RepresentationRootRole: return App::Helper::System::isRepresentationRoot( { _system, item, index } );
		default: return {};
		}
	}

	QModelIndex SystemModel::index( int p_row, int p_column, const QModelIndex & p_parent ) const
	{
		using namespace Core::Struct;

		assert( p_column == 0 && p_row >= 0 );

		// System.
		if ( not p_parent.isValid() )
		{
			assert( p_row == 0 );
			return createIndex( p_row, p_column, pack( E_SYSTEM_ITEM::SYSTEM, 0 ) );
		}

		E_SYSTEM_ITEM item;
		Index		  index;
		unpack( p_parent.internalId(), item, index );

		switch ( item )
		{
		// Chain.
		case E_SYSTEM_ITEM::SYSTEM:
		{
			assert( p_row < static_cast<int>( _data.get().getChainCount() ) );
			return createIndex( p_row, p_column, pack( E_SYSTEM_ITEM::CHAIN, p_row ) );
		}
		// Residue.
		case E_SYSTEM_ITEM::CHAIN:
		{
			assert( index < _data.get().getChainCount() );
			assert( p_row < static_cast<int>( _data.get().chainResidueCounts[ index ] ) );
			return createIndex(
				p_row, p_column, pack( E_SYSTEM_ITEM::RESIDUE, _data.get().chainFirstResidues[ index ] + p_row )
			);
		}
		// Atom.
		case E_SYSTEM_ITEM::RESIDUE:
		{
			assert( index < _data.get().getResidueCount() );
			assert( p_row < static_cast<int>( _data.get().residueAtomCounts[ index ] ) );
			return createIndex(
				p_row, p_column, pack( E_SYSTEM_ITEM::ATOM, _data.get().residueFirstAtomIndexes[ index ] + p_row )
			);
		}
		default: return {};
		}
	}

	QModelIndex SystemModel::parent( const QModelIndex & p_index ) const
	{
		using namespace Core::Struct;

		if ( not p_index.isValid() )
		{
			return {};
		}

		E_SYSTEM_ITEM item;
		Index		  index;
		unpack( p_index.internalId(), item, index );

		// Root.
		if ( item == E_SYSTEM_ITEM::SYSTEM )
		{
			return {};
		}

		switch ( item )
		{
		case E_SYSTEM_ITEM::CHAIN:
		{
			assert( index < _data.get().getChainCount() );
			return createIndex( 0, 0, pack( E_SYSTEM_ITEM::SYSTEM, 0 ) );
		}
		case E_SYSTEM_ITEM::RESIDUE:
		{
			assert( index < _data.get().getResidueCount() );
			const Index chain		= _data.get().residueChainIndexes[ index ];
			const int	rowInSystem = int( chain );
			return createIndex( rowInSystem, 0, pack( E_SYSTEM_ITEM::CHAIN, chain ) );
		}
		case E_SYSTEM_ITEM::ATOM:
		{
			assert( index < _data.get().getAtomCount() );
			const Index residue	   = _data.get().atomResidueIndexes[ index ];
			const Index chain	   = _data.get().residueChainIndexes[ residue ];
			const int	rowInChain = int( residue - _data.get().chainFirstResidues[ chain ] );
			return createIndex( rowInChain, 0, pack( E_SYSTEM_ITEM::RESIDUE, residue ) );
		}
		default: return {};
		}
	}

	QModelIndex SystemModel::makeIndex(
		const int						  p_row,
		const Core::Struct::E_SYSTEM_ITEM p_type,
		const Index						  p_index
	) const
	{
		quintptr id = pack( p_type, p_index );
		return createIndex( p_row, 0, id );
	}

	quintptr SystemModel::pack( const Core::Struct::E_SYSTEM_ITEM p_item, const Index p_index )
	{
		static_assert( sizeof( quintptr ) >= 8, "pack/unpack requires 64-bit (quintptr >= 8 bytes)" );

		// [ p_item:8 |  p_index:32 ]  <= 40 bits
		return ( quintptr( p_item ) << 32 ) | // bits 32..39
			   ( quintptr( p_index ) );		  // bits  0..31
	}

	void SystemModel::unpack( const quintptr p_v, Core::Struct::E_SYSTEM_ITEM & p_item, Index & p_index )
	{
		p_item	= Core::Struct::E_SYSTEM_ITEM( ( p_v >> 32 ) & 0xFFu ); // 8 bits
		p_index = Index( p_v & 0xFFFFFFFFu );							// 32 bits
	}
} // namespace VTX::UI::QT::Model
