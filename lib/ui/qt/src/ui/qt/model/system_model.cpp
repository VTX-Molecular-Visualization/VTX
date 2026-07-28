#include "ui/qt/model/system_model.hpp"
#include "app/system/uid.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <algorithm>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <io/reader.hpp>
#include <iterator>
#include <string_view>
#include <util/enum.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <variant>

namespace VTX::UI::QT::Model
{
	SystemModel::SystemModel( const Entity p_system, QObject * p_parent ) :
		_system( p_system ), QAbstractItemModel( p_parent )
	{
		_viewMode = static_cast<ViewMode>(
			SETTINGS().value( SETTING_KEY_TREE_VIEW_MODE, VTX::toUnderlying( ViewMode::ByChain ) ).toInt()
		);
	}

	int SystemModel::columnCount( const QModelIndex & p_parent ) const { return 1; }

	int SystemModel::rowCount( const QModelIndex & p_parent ) const
	{
		using namespace Core::Struct;

		const auto & data = App::REG().get<Core::Struct::Topology>( _system );

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
			if ( _viewMode == ViewMode::ByCategory )
			{
				int count = 0;
				for ( const auto & residues : data.categoryResidues )
				{
					if ( not residues.empty() )
					{
						++count;
					}
				}
				return count;
			}

			return data.getChainCount();
		}
		case E_SYSTEM_ITEM::CATEGORY:
		{
			if ( index >= data.categoryResidues.size() )
			{
				return 0;
			}

			return int( data.categoryResidues[ index ].size() );
		}
		case E_SYSTEM_ITEM::CHAIN:
		{
			if ( index >= data.getChainCount() )
			{
				return 0;
			}

			return data.chainResidueCounts[ index ];
		}
		case E_SYSTEM_ITEM::RESIDUE:
		{
			if ( index >= data.getResidueCount() )
			{
				return 0;
			}

			return data.residueAtomCounts[ index ];
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

		const auto & data = App::REG().get<Core::Struct::Topology>( _system );

		switch ( p_role )
		{
		case Qt::DisplayRole:
			switch ( item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
			{
				const auto & metadata = App::REG().get<IO::Metadata>( _system );

				if ( not metadata.name.empty() )
				{
					return QString::fromStdString( metadata.name );
				}
				else if ( metadata.pdbIDCode != IO::PDB_ID_CODE_DEFAULT )
				{
					return QString::fromStdString( metadata.pdbIDCode );
				}
				else
				{
					return QString::fromStdString( metadata.path.stem().string() );
				}
			}
			case E_SYSTEM_ITEM::CHAIN:
			{
				assert( index < data.getChainCount() );

				if ( not data.chainNames[ index ].empty() )
				{
					return QString::fromStdString( data.chainNames[ index ] );
				}
				else
				{
					return "-";
				}
			}
			case E_SYSTEM_ITEM::CATEGORY:
			{
				assert( index < Core::ChemDB::Category::TYPE_STR.size() );
				const std::string_view name = Core::ChemDB::Category::TYPE_STR[ index ];
				return QString::fromUtf8( name.data(), int( name.size() ) );
			}
			case E_SYSTEM_ITEM::RESIDUE:
			{
				assert( index < data.getResidueCount() );

				if ( not data.residueNames[ index ].empty() )
				{
					return QString::fromStdString( data.residueNames[ index ] );
				}
				else
				{
					return "-";
				}
			}
			case E_SYSTEM_ITEM::ATOM:
			{
				assert( index < data.getAtomCount() );

				if ( not data.atomNames[ index ].empty() )
				{
					return QString::fromStdString( data.atomNames[ index ] );
				}
				else
				{
					return "-";
				}
			}
			}

			return {};
		case ItemRole: return toUnderlying( item );
		case SelectionStateRole:
			return toUnderlying( App::Helper::System::getSelectionState( { _system, item, index } ) );
		case Qt::DecorationRole:
			switch ( item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
			{
				return STYLE().iconFromCodepoint( Icons::SYSTEM );
			}
			case E_SYSTEM_ITEM::CHAIN:
			{
				return STYLE().iconFromCodepoint( Icons::CHAIN );
			}
			case E_SYSTEM_ITEM::CATEGORY:
			{
				return STYLE().iconFromCodepoint( Icons::CATEGORY );
			}
			case E_SYSTEM_ITEM::RESIDUE:
			{
				return STYLE().iconFromCodepoint( Icons::RESIDUE );
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

		if ( p_column != 0 || p_row < 0 )
		{
			return {};
		}

		const auto & data = App::REG().get<Core::Struct::Topology>( _system );

		// System.
		if ( not p_parent.isValid() )
		{
			if ( p_row != 0 )
			{
				return {};
			}

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
			if ( _viewMode == ViewMode::ByCategory )
			{
				const Index category = _categoryFromRow( p_row );
				if ( category >= toUnderlying( Core::ChemDB::Category::TYPE::COUNT ) )
				{
					return {};
				}

				return createIndex( p_row, p_column, pack( E_SYSTEM_ITEM::CATEGORY, category ) );
			}

			if ( p_row >= static_cast<int>( data.getChainCount() ) )
			{
				return {};
			}

			return createIndex( p_row, p_column, pack( E_SYSTEM_ITEM::CHAIN, p_row ) );
		}
		case E_SYSTEM_ITEM::CATEGORY:
		{
			if ( index >= data.categoryResidues.size()
				 || p_row >= static_cast<int>( data.categoryResidues[ index ].size() ) )
			{
				return {};
			}

			return createIndex(
				p_row, p_column, pack( E_SYSTEM_ITEM::RESIDUE, data.categoryResidues[ index ][ p_row ] )
			);
		}
		// Residue.
		case E_SYSTEM_ITEM::CHAIN:
		{
			if ( index >= data.getChainCount() || p_row >= static_cast<int>( data.chainResidueCounts[ index ] ) )
			{
				return {};
			}

			return createIndex(
				p_row, p_column, pack( E_SYSTEM_ITEM::RESIDUE, data.chainFirstResidues[ index ] + p_row )
			);
		}
		// Atom.
		case E_SYSTEM_ITEM::RESIDUE:
		{
			if ( index >= data.getResidueCount() || p_row >= static_cast<int>( data.residueAtomCounts[ index ] ) )
			{
				return {};
			}

			return createIndex(
				p_row, p_column, pack( E_SYSTEM_ITEM::ATOM, data.residueFirstAtomIndexes[ index ] + p_row )
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

		const auto & data = App::REG().get<Core::Struct::Topology>( _system );

		// Root.
		if ( item == E_SYSTEM_ITEM::SYSTEM )
		{
			return {};
		}

		switch ( item )
		{
		case E_SYSTEM_ITEM::CATEGORY:
		{
			if ( index >= data.categoryResidues.size() )
			{
				return {};
			}

			return createIndex( 0, 0, pack( E_SYSTEM_ITEM::SYSTEM, 0 ) );
		}
		case E_SYSTEM_ITEM::CHAIN:
		{
			if ( index >= data.getChainCount() )
			{
				return {};
			}

			return createIndex( 0, 0, pack( E_SYSTEM_ITEM::SYSTEM, 0 ) );
		}
		case E_SYSTEM_ITEM::RESIDUE:
		{
			if ( index >= data.getResidueCount() )
			{
				return {};
			}

			if ( _viewMode == ViewMode::ByCategory )
			{
				const Index category = Index( toUnderlying( data.getResidueCategory( index ) ) );
				const int	row		 = _categoryRow( category );
				if ( row < 0 )
				{
					return {};
				}

				return createIndex( row, 0, pack( E_SYSTEM_ITEM::CATEGORY, category ) );
			}

			const Index chain		= data.residueChainIndexes[ index ];
			const int	rowInSystem = int( chain );
			return createIndex( rowInSystem, 0, pack( E_SYSTEM_ITEM::CHAIN, chain ) );
		}
		case E_SYSTEM_ITEM::ATOM:
		{
			if ( index >= data.getAtomCount() )
			{
				return {};
			}

			const Index residue = data.atomResidueIndexes[ index ];
			if ( _viewMode == ViewMode::ByCategory )
			{
				const Index category = Index( toUnderlying( data.getResidueCategory( residue ) ) );
				if ( category >= data.categoryResidues.size() )
				{
					return {};
				}

				const auto & residues = data.categoryResidues[ category ];
				const auto	 it		  = std::find( residues.begin(), residues.end(), residue );
				if ( it == residues.end() )
				{
					return {};
				}

				return createIndex(
					int( std::distance( residues.begin(), it ) ), 0, pack( E_SYSTEM_ITEM::RESIDUE, residue )
				);
			}

			const Index chain	   = data.residueChainIndexes[ residue ];
			const int	rowInChain = int( residue - data.chainFirstResidues[ chain ] );
			return createIndex( rowInChain, 0, pack( E_SYSTEM_ITEM::RESIDUE, residue ) );
		}
		default: return {};
		}
	}

	void SystemModel::setViewMode( const ViewMode p_mode )
	{
		if ( _viewMode == p_mode )
		{
			return;
		}

		beginResetModel();
		_viewMode = p_mode;
		endResetModel();
	}

	Index SystemModel::_categoryFromRow( const int p_row ) const
	{
		const auto & data = App::REG().get<Core::Struct::Topology>( _system );

		int row = 0;
		for ( Index category = 0; category < data.categoryResidues.size(); ++category )
		{
			if ( data.categoryResidues[ category ].empty() )
			{
				continue;
			}

			if ( row == p_row )
			{
				return category;
			}

			++row;
		}

		return Index( data.categoryResidues.size() );
	}

	int SystemModel::_categoryRow( const Index p_category ) const
	{
		const auto & data = App::REG().get<Core::Struct::Topology>( _system );
		if ( p_category >= data.categoryResidues.size() || data.categoryResidues[ p_category ].empty() )
		{
			return -1;
		}

		int row = 0;
		for ( Index category = 0; category < p_category; ++category )
		{
			if ( not data.categoryResidues[ category ].empty() )
			{
				++row;
			}
		}

		return row;
	}

	QModelIndex SystemModel::makeIndex( const int p_row, const Core::Struct::E_SYSTEM_ITEM p_type, const Index p_index )
		const
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
