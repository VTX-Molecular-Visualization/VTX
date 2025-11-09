#include "ui/qt/model.hpp"

namespace
{
	using namespace VTX;
	using namespace VTX::UI::QT;
	/**
	 * @brief Pack minimum information to identify an item in the model into a single uint64.
	 */
	quintptr pack( const Model::E_SYSTEM_LEVEL p_level, const uint p_system, const Index p_index )
	{
		// [ L:2 | mol:30 | local:32 ]  <= 64 bits
		return ( quintptr( p_index ) ) | ( quintptr( p_system ) << 32 ) | ( quintptr( p_level ) << 62 );
	}

	/**
	 * @brief Unpack quintptr.
	 */
	void unpack( const quintptr p_v, Model::E_SYSTEM_LEVEL & p_level, uint & p_system, Index & p_index )
	{
		p_level	 = Model::E_SYSTEM_LEVEL( ( p_v >> 62 ) & 0x3 );
		p_system = uint( ( p_v >> 32 ) & 0x3FFFFFFF );
		p_index	 = Index( p_v & 0xFFFFFFFFu );
	}
} // namespace

namespace VTX::UI::QT
{
	Model::Model( QObject * p_parent ) : QAbstractItemModel( p_parent )
	{
		// Connect system construction event.
		App::REG().on_construct<Core::Struct::System>().connect<&Model::_onConstructSystem>( this );
	}

	int Model::columnCount( const QModelIndex & p_parent ) const { return 1; }

	int Model::rowCount( const QModelIndex & p_parent ) const
	{
		if ( not p_parent.isValid() )
		{
			return int( _systems.size() );
		}

		E_SYSTEM_LEVEL level;
		Index		   systemIndex, localIndex;
		unpack( p_parent.internalId(), level, systemIndex, localIndex );
		const Core::Struct::System & system = *( _systems[ systemIndex ] );

		switch ( level )
		{
		case E_SYSTEM_LEVEL::SYSTEM: return system.getChainCount();
		case E_SYSTEM_LEVEL::CHAIN:
		{
			return system.chainResidueCounts[ localIndex ];
		}
		case E_SYSTEM_LEVEL::RESIDUE:
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

		E_SYSTEM_LEVEL level;
		Index		   systemIndex, localIndex;
		unpack( p_index.internalId(), level, systemIndex, localIndex );
		const Core::Struct::System & system = *( _systems[ systemIndex ] );

		switch ( p_role )
		{
		case Qt::DisplayRole:
		case NameRole:
			switch ( level )
			{
			case E_SYSTEM_LEVEL::SYSTEM: return QString( "Molecule %1" ).arg( systemIndex );
			case E_SYSTEM_LEVEL::CHAIN: return QString::fromStdString( system.chainNames[ localIndex ] );
			case E_SYSTEM_LEVEL::RESIDUE: return QString::fromStdString( system.residueNames[ localIndex ] );
			case E_SYSTEM_LEVEL::ATOM: return QString::fromStdString( system.atomNames[ localIndex ] );
			}
			return {};
		case TypeRole: return int( level );
		case SystemRole: return systemIndex;
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
			if ( p_row >= int( _systems.size() ) )
			{
				return {};
			}
			return createIndex( p_row, p_column, pack( E_SYSTEM_LEVEL::SYSTEM, Index( p_row ), 0 ) );
		}

		E_SYSTEM_LEVEL level;
		Index		   systemIndex, localIndex;
		unpack( p_parent.internalId(), level, systemIndex, localIndex );
		const Core::Struct::System & system = *( _systems[ systemIndex ] );

		switch ( level )
		{
		// Chain.
		case E_SYSTEM_LEVEL::SYSTEM:
		{
			if ( uint( p_row ) >= system.getChainCount() )
			{
				return {};
			}

			return createIndex( p_row, p_column, pack( E_SYSTEM_LEVEL::CHAIN, systemIndex, p_row ) );
		}
		// Residue.
		case E_SYSTEM_LEVEL::CHAIN:
		{
			if ( uint( p_row ) >= system.chainResidueCounts[ localIndex ] )
			{
				return {};
			}

			return createIndex(
				p_row,
				p_column,
				pack( E_SYSTEM_LEVEL::RESIDUE, systemIndex, system.chainFirstResidues[ localIndex ] + p_row )
			);
		}
		// Atom.
		case E_SYSTEM_LEVEL::RESIDUE:
		{
			if ( uint( p_row ) >= system.residueAtomCounts[ localIndex ] )
			{
				return {};
			}

			return createIndex(
				p_row,
				p_column,
				pack( E_SYSTEM_LEVEL::ATOM, systemIndex, system.residueFirstAtomIndexes[ localIndex ] + p_row )
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

		E_SYSTEM_LEVEL level;
		Index		   systemIndex, localIndex;
		unpack( p_index.internalId(), level, systemIndex, localIndex );

		// Root.
		if ( level == E_SYSTEM_LEVEL::SYSTEM )
		{
			return {};
		}

		Q_ASSERT( systemIndex >= 0 && systemIndex < Index( _systems.size() ) );
		const Core::Struct::System & system = *( _systems[ systemIndex ] );

		switch ( level )
		{
		case E_SYSTEM_LEVEL::CHAIN:
		{
			return createIndex( systemIndex, 0, pack( E_SYSTEM_LEVEL::SYSTEM, systemIndex, 0 ) );
		}
		case E_SYSTEM_LEVEL::RESIDUE:
		{
			Q_ASSERT( localIndex >= 0 && localIndex < Index( system.residueNames.size() ) );
			const Index chain		= system.residueChainIndexes[ localIndex ];
			const int	rowInSystem = int( chain );
			return createIndex( rowInSystem, 0, pack( E_SYSTEM_LEVEL::CHAIN, systemIndex, chain ) );
		}
		case E_SYSTEM_LEVEL::ATOM:
		{
			Q_ASSERT( localIndex >= 0 && localIndex < Index( system.atomNames.size() ) );
			const Index residue	   = system.atomResidueIndexes[ localIndex ];
			const Index chain	   = system.residueChainIndexes[ residue ];
			const int	rowInChain = int( residue - system.chainFirstResidues[ chain ] );
			return createIndex( rowInChain, 0, pack( E_SYSTEM_LEVEL::RESIDUE, systemIndex, residue ) );
		}
		default: return {};
		}
	}

	void Model::_onConstructSystem( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		const auto & system = p_r.get<Core::Struct::System>( p_e );
		const int	 row	= int( _systems.size() );
		beginInsertRows( QModelIndex(), row, row );
		_systems.push_back( &system );
		endInsertRows();
	}
} // namespace VTX::UI::QT
