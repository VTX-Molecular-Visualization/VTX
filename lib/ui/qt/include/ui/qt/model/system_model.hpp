#ifndef __VTX_UI_QT_MODEL__
#define __VTX_UI_QT_MODEL__

#include <QAbstractItemModel>
#include <QString>
#include <app/ecs.hpp>
#include <app/events.hpp>
#include <app/system/metadata.hpp>
#include <core/struct/topology.hpp>
#include <vector>

namespace VTX::UI::QT::Model
{
	/**
	 * @brief Model representing all data in App.
	 */
	class SystemModel : public QAbstractItemModel
	{
	  public:
		/**
		 * @brief Roles for data retrieval.
		 */
		enum Roles
		{
			// SYSTEM, CHAIN, RESIDUE, ATOM.
			ItemRole = Qt::UserRole + 1,
			// NONE, PARTIAL, FULL.
			SelectionStateRole,
			// VISIBLE, PARTIAL, HIDDEN.
			VisibleRole,
			// true/false.
			ColorSchemeRootRole,
			// true/false.
			RepresentationRootRole
		};

		/**
		 * @brief Constructor.
		 */
		SystemModel( const App::ECS::Entity, QObject * = nullptr );

		/**
		 * @brief Number of columns is always 1.
		 */
		int columnCount( const QModelIndex & p_parent ) const override;

		/**
		 * @brief Number of rows depending on the parent index.
		 */
		int rowCount( const QModelIndex & p_parent ) const override;

		/**
		 * @brief Data retrieval (display, roles, etc.).
		 */
		QVariant data( const QModelIndex & p_index, int p_role = Qt::DisplayRole ) const override;

		/**
		 * @brief Create index for given row/column/parent.
		 */
		QModelIndex index( int p_row, int p_column, const QModelIndex & p_parent ) const override;

		/**
		 * @brief Get parent index of given index.
		 */
		QModelIndex parent( const QModelIndex & p_index ) const override;

		/**
		 * @brief Public index creation.
		 */
		QModelIndex makeIndex( const int p_row, const Core::Struct::E_SYSTEM_ITEM, const Index ) const;

		/**
		 * @brief Pack minimum information to identify an item in the model into a single uint64.
		 */
		static quintptr pack( const Core::Struct::E_SYSTEM_ITEM, const Index );

		/**
		 * @brief Unpack quintptr.
		 */
		static void unpack( const quintptr, Core::Struct::E_SYSTEM_ITEM &, Index & );

	  private:
		/**
		 * @brief Entity of fetch data from.
		 */
		const App::ECS::Entity _system;

		/**
		 * @brief Reference to the system data.
		 */
		std::reference_wrapper<const App::System::Metadata>	 _metadata;
		std::reference_wrapper<const Core::Struct::Topology> _data;
	};

} // namespace VTX::UI::QT::Model
#endif
