#include "ui/qt/model.hpp"
#include "app/system/uid.hpp"
#include <app/services.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <variant>

namespace VTX::UI::QT
{
	Model::Model( QObject * p_parent ) : QAbstractItemModel( p_parent )
	{
		// Connect system construction event.
		App::HUB().connect<App::Events::SystemLoad, &Model::_onSystemLoad>( this );
		App::REG().on_destroy<Core::Struct::System>().connect<&Model::_onDestroySystem>( this );
	}

	int Model::columnCount( const QModelIndex & p_parent ) const { return 1; }

	int Model::rowCount( const QModelIndex & p_parent ) const
	{
		using namespace App::Scene;
		if ( not p_parent.isValid() )
		{
			return int( _rows.size() );
		}

		E_ITEM	item;
		RootUID rootIndex;
		Index	localIndex;
		unpack( p_parent.internalId(), item, rootIndex, localIndex );

		if ( not _mapGlobalIndexRow.contains( rootIndex ) )
		{
			return 0;
		}

		const Core::Struct::System & system
			= *std::get<const Core::Struct::System *>( _mapGlobalIndexRow.at( rootIndex )->data );

		switch ( item )
		{
		case E_ITEM::SYSTEM:
		{
			return system.getChainCount();
		}
		case E_ITEM::CHAIN:
		{
			if ( localIndex >= system.getChainCount() )
			{
				return 0;
			}

			return system.chainResidueCounts[ localIndex ];
		}
		case E_ITEM::RESIDUE:
		{
			if ( localIndex >= system.getResidueCount() )
			{
				return 0;
			}

			return system.residueAtomCounts[ localIndex ];
		}
		default: return 0;
		}
	}

	QVariant Model::data( const QModelIndex & p_index, int p_role ) const
	{
		using namespace App::Scene;

		if ( not p_index.isValid() )
		{
			return {};
		}

		E_ITEM	item;
		RootUID rootIndex;
		Index	localIndex;
		unpack( p_index.internalId(), item, rootIndex, localIndex );

		if ( not _mapGlobalIndexRow.contains( rootIndex ) )
		{
			return {};
		}

		const Core::Struct::System & system
			= *std::get<const Core::Struct::System *>( _mapGlobalIndexRow.at( rootIndex )->data );

		switch ( p_role )
		{
		case Qt::DisplayRole:
		case NameRole:
			switch ( item )
			{
			case E_ITEM::SYSTEM:
			{
				return QString::fromStdString( system.name );
			}
			case E_ITEM::CHAIN:
			{
				if ( localIndex >= system.getChainCount() )
				{
					return {};
				}

				return QString::fromStdString( system.chainNames[ localIndex ] );
			}
			case E_ITEM::RESIDUE:
			{
				if ( localIndex >= system.getResidueCount() )
				{
					return {};
				}

				return QString::fromStdString( system.residueNames[ localIndex ] );
			}
			case E_ITEM::ATOM:
			{
				if ( localIndex >= system.getAtomCount() )
				{
					return {};
				}

				return QString::fromStdString( system.atomNames[ localIndex ] );
			}
			}
			return {};
		case ItemRole: return int( item );
		case RootRole: return rootIndex;
		case LocalRole: return localIndex;
		case VisibleRole: // TODO.
			return true;
		default: return {};
		}
	}

	QModelIndex Model::index( int p_row, int p_column, const QModelIndex & p_parent ) const
	{
		using namespace App::Scene;

		if ( p_column != 0 || p_row < 0 )
		{
			return {};
		}

		// System.
		if ( not p_parent.isValid() )
		{
			if ( p_row >= int( _rows.size() ) )
			{
				return {};
			}
			return createIndex( p_row, p_column, pack( E_ITEM::SYSTEM, _rows[ p_row ]->index, 0 ) );
		}

		E_ITEM	item;
		RootUID rootIndex;
		Index	localIndex;
		unpack( p_parent.internalId(), item, rootIndex, localIndex );

		if ( not _mapGlobalIndexRow.contains( rootIndex ) )
		{
			return {};
		}

		const Core::Struct::System & system
			= *std::get<const Core::Struct::System *>( _mapGlobalIndexRow.at( rootIndex )->data );

		switch ( item )
		{
		// Chain.
		case E_ITEM::SYSTEM:
		{
			if ( uint( p_row ) >= system.getChainCount() )
			{
				return {};
			}

			return createIndex( p_row, p_column, pack( E_ITEM::CHAIN, rootIndex, p_row ) );
		}
		// Residue.
		case E_ITEM::CHAIN:
		{
			if ( localIndex >= system.getChainCount() )
			{
				return {};
			}

			if ( uint( p_row ) >= system.chainResidueCounts[ localIndex ] )
			{
				return {};
			}

			return createIndex(
				p_row, p_column, pack( E_ITEM::RESIDUE, rootIndex, system.chainFirstResidues[ localIndex ] + p_row )
			);
		}
		// Atom.
		case E_ITEM::RESIDUE:
		{
			if ( localIndex >= system.getResidueCount() )
			{
				return {};
			}

			if ( uint( p_row ) >= system.residueAtomCounts[ localIndex ] )
			{
				return {};
			}

			return createIndex(
				p_row, p_column, pack( E_ITEM::ATOM, rootIndex, system.residueFirstAtomIndexes[ localIndex ] + p_row )
			);
		}
		default: return {};
		}
	}

	QModelIndex Model::parent( const QModelIndex & p_index ) const
	{
		using namespace App::Scene;

		if ( not p_index.isValid() )
		{
			return {};
		}

		E_ITEM	item;
		RootUID rootIndex;
		Index	localIndex;
		unpack( p_index.internalId(), item, rootIndex, localIndex );

		// Root.
		if ( item == E_ITEM::SYSTEM )
		{
			return {};
		}

		if ( not _mapGlobalIndexRow.contains( rootIndex ) )
		{
			return {};
		}

		const Core::Struct::System & system
			= *std::get<const Core::Struct::System *>( _mapGlobalIndexRow.at( rootIndex )->data );

		switch ( item )
		{
		case E_ITEM::CHAIN:
		{
			if ( localIndex >= system.getChainCount() )
			{
				return {};
			}

			return createIndex( _mapGlobalIndexRow.at( rootIndex )->position, 0, pack( E_ITEM::SYSTEM, rootIndex, 0 ) );
		}
		case E_ITEM::RESIDUE:
		{
			if ( localIndex >= system.getResidueCount() )
			{
				return {};
			}

			const Index chain		= system.residueChainIndexes[ localIndex ];
			const int	rowInSystem = int( chain );
			return createIndex( rowInSystem, 0, pack( E_ITEM::CHAIN, rootIndex, chain ) );
		}
		case E_ITEM::ATOM:
		{
			if ( localIndex >= system.getAtomCount() )
			{
				return {};
			}

			const Index residue	   = system.atomResidueIndexes[ localIndex ];
			const Index chain	   = system.residueChainIndexes[ residue ];
			const int	rowInChain = int( residue - system.chainFirstResidues[ chain ] );
			return createIndex( rowInChain, 0, pack( E_ITEM::RESIDUE, rootIndex, residue ) );
		}
		default: return {};
		}
	}

	void Model::_onSystemLoad( const App::Events::SystemLoad & p_e )
	{
		using namespace App::Scene;

		const auto & reg	  = App::REG();
		const auto	 entity	  = p_e.system;
		const auto & system	  = reg.get<Core::Struct::System>( entity );
		const auto & uid	  = reg.get<App::System::UID>( entity );
		const int	 position = int( _rows.size() );

		beginInsertRows( QModelIndex(), position, position );

		_rows.emplace_back(
			std::make_unique<Row>(
				position, uid.system, entity, E_ITEM::SYSTEM, std::variant<const Core::Struct::System *>( &system )
			)
		);

		auto & row						 = _rows.back();
		_mapEntityRow[ row->entity ]	 = row.get();
		_mapGlobalIndexRow[ row->index ] = row.get();

		endInsertRows();
	}

	void Model::_onDestroySystem( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		assert( _mapEntityRow.contains( p_e ) );

		const Row * rowPtr	 = _mapEntityRow.at( p_e );
		const int	position = rowPtr->position;

		beginRemoveRows( QModelIndex(), position, position );

		_mapGlobalIndexRow.erase( rowPtr->index );
		_mapEntityRow.erase( p_e );

		_rows.erase( _rows.begin() + position );

		for ( int i = position; i < int( _rows.size() ); ++i )
		{
			Row * r						   = _rows[ i ].get();
			r->position					   = i;
			_mapEntityRow[ r->entity ]	   = r;
			_mapGlobalIndexRow[ r->index ] = r;
		}

		endRemoveRows();
	}

	quintptr Model::pack( const App::Scene::E_ITEM p_item, const RootUID p_rootIndex, const Index p_index )
	{
		// [ p_item:8 | p_rootIndex:16 | p_index:32 ]  <= 56 bits
		return ( quintptr( p_item ) << 48 ) |	   // bits 48..55
			   ( quintptr( p_rootIndex ) << 32 ) | // bits 32..47
			   ( quintptr( p_index ) );			   // bits  0..31
	}

	void Model::unpack( const quintptr p_v, App::Scene::E_ITEM & p_item, RootUID & p_rootIndex, Index & p_index )
	{
		p_item		= App::Scene::E_ITEM( ( p_v >> 48 ) & 0xFF ); // 8 bits
		p_rootIndex = RootUID( ( p_v >> 32 ) & 0xFFFF );		  // 16 bits
		p_index		= Index( p_v & 0xFFFFFFFFu );				  // 32 bits
	}
} // namespace VTX::UI::QT
