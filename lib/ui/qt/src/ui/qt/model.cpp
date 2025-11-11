#include "ui/qt/model.hpp"

namespace VTX::UI::QT
{
	Model::Model( QObject * p_parent ) : QAbstractItemModel( p_parent )
	{
		// Connect system construction event.
		App::REG().on_construct<Core::Struct::System>().connect<&Model::_onConstructSystem>( this );
		App::REG().on_destroy<Core::Struct::System>().connect<&Model::_onDestroySystem>( this );
	}

	int Model::columnCount( const QModelIndex & p_parent ) const { return 1; }

	int Model::rowCount( const QModelIndex & p_parent ) const
	{
		if ( not p_parent.isValid() )
		{
			return int( _mapSystems.size() );
		}

		E_ITEM		item;
		GlobalIndex globalIndex;
		Index		localIndex;
		unpack( p_parent.internalId(), item, globalIndex, localIndex );
		const Core::Struct::System & system = *_mapSystems[ globalIndex ];

		switch ( item )
		{
		case E_ITEM::SYSTEM: return system.getChainCount();
		case E_ITEM::CHAIN:
		{
			return system.chainResidueCounts[ localIndex ];
		}
		case E_ITEM::RESIDUE:
		{
			return system.residueAtomCounts[ localIndex ];
		}
		default: return 0;
		}
	}

	QVariant Model::data( const QModelIndex & p_index, int p_role ) const

	{
		if ( not p_index.isValid() )
		{
			return {};
		}

		E_ITEM		item;
		GlobalIndex globalIndex;
		Index		localIndex;
		unpack( p_index.internalId(), item, globalIndex, localIndex );
		const Core::Struct::System & system = *_mapSystems[ globalIndex ];

		switch ( p_role )
		{
		case Qt::DisplayRole:
		case NameRole:
			switch ( item )
			{
			case E_ITEM::SYSTEM: return QString( "Molecule %1" ).arg( globalIndex );
			case E_ITEM::CHAIN: return QString::fromStdString( system.chainNames[ localIndex ] );
			case E_ITEM::RESIDUE: return QString::fromStdString( system.residueNames[ localIndex ] );
			case E_ITEM::ATOM: return QString::fromStdString( system.atomNames[ localIndex ] );
			}
			return {};
		case TypeRole: return int( item );
		case SystemRole: return globalIndex;
		case LocalRole: return localIndex;
		default: return {};
		}
	}

	QModelIndex Model::index( int p_row, int p_column, const QModelIndex & p_parent ) const
	{
		if ( p_column != 0 || p_row < 0 )
		{
			return {};
		}

		// System.
		if ( not p_parent.isValid() )
		{
			if ( p_row >= int( _mapSystems.size() ) )
			{
				return {};
			}
			return createIndex( p_row, p_column, pack( E_ITEM::SYSTEM, Index( p_row ), 0 ) );
		}

		E_ITEM		item;
		GlobalIndex globalIndex;
		Index		localIndex;
		unpack( p_parent.internalId(), item, globalIndex, localIndex );
		const Core::Struct::System & system = *_mapSystems[ globalIndex ];

		switch ( item )
		{
		// Chain.
		case E_ITEM::SYSTEM:
		{
			if ( uint( p_row ) >= system.getChainCount() )
			{
				return {};
			}

			return createIndex( p_row, p_column, pack( E_ITEM::CHAIN, globalIndex, p_row ) );
		}
		// Residue.
		case E_ITEM::CHAIN:
		{
			if ( uint( p_row ) >= system.chainResidueCounts[ localIndex ] )
			{
				return {};
			}

			return createIndex(
				p_row, p_column, pack( E_ITEM::RESIDUE, globalIndex, system.chainFirstResidues[ localIndex ] + p_row )
			);
		}
		// Atom.
		case E_ITEM::RESIDUE:
		{
			if ( uint( p_row ) >= system.residueAtomCounts[ localIndex ] )
			{
				return {};
			}

			return createIndex(
				p_row, p_column, pack( E_ITEM::ATOM, globalIndex, system.residueFirstAtomIndexes[ localIndex ] + p_row )
			);
		}
		default: return {};
		}
	}

	QModelIndex Model::parent( const QModelIndex & p_index ) const
	{
		if ( not p_index.isValid() )
		{
			return {};
		}

		E_ITEM		item;
		GlobalIndex globalIndex;
		Index		localIndex;
		unpack( p_index.internalId(), item, globalIndex, localIndex );

		// Root.
		if ( item == E_ITEM::SYSTEM )
		{
			return {};
		}

		assert( globalIndex < _mapSystems.size() );
		const Core::Struct::System & system = *_mapSystems[ globalIndex ];

		switch ( item )
		{
		case E_ITEM::CHAIN:
		{
			return createIndex( globalIndex, 0, pack( E_ITEM::SYSTEM, globalIndex, 0 ) );
		}
		case E_ITEM::RESIDUE:
		{
			assert( localIndex < Index( system.residueNames.size() ) );
			const Index chain		= system.residueChainIndexes[ localIndex ];
			const int	rowInSystem = int( chain );
			return createIndex( rowInSystem, 0, pack( E_ITEM::CHAIN, globalIndex, chain ) );
		}
		case E_ITEM::ATOM:
		{
			assert( localIndex < Index( system.atomNames.size() ) );
			const Index residue	   = system.atomResidueIndexes[ localIndex ];
			const Index chain	   = system.residueChainIndexes[ residue ];
			const int	rowInChain = int( residue - system.chainFirstResidues[ chain ] );
			return createIndex( rowInChain, 0, pack( E_ITEM::RESIDUE, globalIndex, residue ) );
		}
		default: return {};
		}
	}

	void Model::_onConstructSystem( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		const auto & system = p_r.get<Core::Struct::System>( p_e );
		const int	 row	= int( _mapSystems.size() );
		beginInsertRows( QModelIndex(), row, row );
		_mapGlobalIndex.emplace( p_e, row );
		_mapSystems.emplace( row, &system );
		endInsertRows();
	}

	void Model::_onDestroySystem( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		/*
		const auto & system = p_r.get<Core::Struct::System>( p_e );
		const auto	 it		= std::find( _systems.begin(), _systems.end(), &system );
		if ( it != _systems.end() )
		{
			const int row = int( std::distance( _systems.begin(), it ) );
			beginRemoveRows( QModelIndex(), row, row );
			_systems.erase( it );
			endRemoveRows();
		}
		*/
	}

	quintptr Model::pack( const E_ITEM p_item, const GlobalIndex p_globalIndex, const Index p_index )
	{
		// [ p_item:8 | p_globalIndex:16 | p_index:32 ]  <= 56 bits
		return ( quintptr( p_item ) << 48 ) |		 // bits 48..55
			   ( quintptr( p_globalIndex ) << 32 ) | // bits 32..47
			   ( quintptr( p_index ) );				 // bits  0..31
	}

	void Model::unpack( const quintptr p_v, E_ITEM & p_item, GlobalIndex & p_globalIndex, Index & p_index )
	{
		p_item		  = E_ITEM( ( p_v >> 48 ) & 0xFF );		   // 8 bits
		p_globalIndex = GlobalIndex( ( p_v >> 32 ) & 0xFFFF ); // 16 bits
		p_index		  = Index( p_v & 0xFFFFFFFFu );			   // 32 bits
	}
} // namespace VTX::UI::QT
