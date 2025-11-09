#ifndef __VTX_UI_QT_MODEL__
#define __VTX_UI_QT_MODEL__

#include "selection.hpp"
#include <QAbstractItemModel>
#include <QString>
#include <app/ecs.hpp>
#include <core/struct/system.hpp>
#include <vector>

namespace VTX::UI::QT
{
	/**
	 * @brief Model representing all data in App.
	 */
	class Model : public QAbstractItemModel
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Levels of the system hierarchy.
		 */
		enum struct E_SYSTEM_LEVEL : uint8_t
		{
			SYSTEM,
			CHAIN,
			RESIDUE,
			ATOM
		};

		/**
		 * @brief Roles for data retrieval.
		 */
		enum Roles
		{
			TypeRole = Qt::UserRole + 1,
			MolRole,
			LocalRole,
			NameRole
		};

		/**
		 * @brief Constructor.
		 */
		Model( QObject * p_parent = nullptr ) : QAbstractItemModel( p_parent )
		{
			// Connect system construction event.
			App::REG().on_construct<Core::Struct::System>().connect<&Model::_onConstructSystem>( this );
		}

		/**
		 * @brief Number of columns is always 1.
		 */
		int columnCount( const QModelIndex & p_parent ) const override { return 1; }

		/**
		 * @brief Number of rows depending on the parent index.
		 */
		int rowCount( const QModelIndex & p_parent ) const override
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

		/**
		 * @brief Data retrieval (display, roles, etc.).
		 */
		QVariant data( const QModelIndex & p_index, int p_role = Qt::DisplayRole ) const override

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
			case MolRole: return systemIndex;
			case LocalRole: return localIndex;
			default: return {};
			}
		}

		/**
		 * @brief Create index for given row/column/parent.
		 */
		QModelIndex index( int p_row, int p_column, const QModelIndex & p_parent ) const override
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
					pack( E_SYSTEM_LEVEL::ATOM, systemIndex, system.residueFirstBondIndexes[ localIndex ] + p_row )
				);
			}
			default: return {};
			}
		}

		QModelIndex parent( const QModelIndex & p_index ) const override
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

			const Core::Struct::System & system = *( _systems[ systemIndex ] );

			switch ( level )
			{
			case E_SYSTEM_LEVEL::CHAIN:
			{
				return createIndex( systemIndex, 0, pack( E_SYSTEM_LEVEL::SYSTEM, systemIndex, 0 ) );
			}
			case E_SYSTEM_LEVEL::RESIDUE:
			{
				const Index chain	 = system.residueChainIndexes[ localIndex ];
				const int	rowInMol = int( chain );
				return createIndex( rowInMol, 0, pack( E_SYSTEM_LEVEL::CHAIN, systemIndex, chain ) );
			}
			case E_SYSTEM_LEVEL::ATOM:
			{
				const Index residue = system.atomResidueIndexes[ localIndex ];
				const int	rowInChain
					= int( residue - system.chainFirstResidues[ system.residueChainIndexes[ residue ] ] );
				return createIndex( rowInChain, 0, pack( E_SYSTEM_LEVEL::RESIDUE, systemIndex, residue ) );
			}
			default: return {};
			}
		}

		/**
		 * @brief Pack minimum information to identify an item in the model into a single uint64.
		 */
		static inline quintptr pack( const E_SYSTEM_LEVEL p_level, const uint p_system, const Index p_index )
		{
			// [ L:2 | mol:30 | local:32 ]  <= 64 bits
			return ( quintptr( p_index ) ) | ( quintptr( p_system ) << 32 ) | ( quintptr( p_level ) << 62 );
		}

		/**
		 * @brief Unpack quintptr.
		 */
		static inline void unpack( const quintptr p_v, E_SYSTEM_LEVEL & p_level, uint & p_system, Index & p_index )
		{
			p_level	 = E_SYSTEM_LEVEL( ( p_v >> 62 ) & 0x3 );
			p_system = uint( ( p_v >> 32 ) & 0x3FFFFFFF );
			p_index	 = Index( p_v & 0xFFFFFFFFu );
		}

	  private:
		/**
		 * @brief The systems contained in the model.
		 */
		std::vector<const Core::Struct::System *> _systems;
		// TODO: add viewpoints?

		void _onConstructSystem( App::ECS::Registry & p_r, App::ECS::Entity p_e )
		{
			const auto & system	  = p_r.get<Core::Struct::System>( p_e );
			const int	 firstRow = int( _systems.size() );
			beginInsertRows( QModelIndex(), firstRow, firstRow );
			_systems.push_back( &system );
			endInsertRows();
		}
	};

} // namespace VTX::UI::QT
#endif
