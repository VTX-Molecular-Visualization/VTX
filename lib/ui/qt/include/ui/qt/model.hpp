#ifndef __VTX_UI_QT_MODEL__
#define __VTX_UI_QT_MODEL__

#include <QAbstractItemModel>
#include <QString>
#include <app/ecs.hpp>
#include <app/events.hpp>
#include <app/scene/root.hpp>
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
		 * @brief Roles for data retrieval.
		 */
		enum Roles
		{
			ItemRole = Qt::UserRole + 1,
			RootRole,
			LocalRole,
			NameRole,
			VisibleRole
		};

		/**
		 * @brief Root row structure.
		 */
		struct Row
		{
			int										   position;
			RootUID									   index;
			App::ECS::Entity						   entity;
			App::Scene::E_ITEM						   item;
			std::variant<const Core::Struct::System *> data;
		};

		/**
		 * @brief Constructor.
		 */
		Model( QObject * p_parent = nullptr );

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
		QModelIndex makeIndex( const int p_row, const App::Scene::E_ITEM, const RootUID, const Index ) const;

		/**
		 * @brief Pack minimum information to identify an item in the model into a single uint64.
		 */
		static quintptr pack( const App::Scene::E_ITEM, const RootUID, const Index );

		/**
		 * @brief Unpack quintptr.
		 */
		static void unpack( const quintptr, App::Scene::E_ITEM &, RootUID &, Index & );

		inline const std::unordered_map<RootUID, const Row *> & getMapRootToRows() const { return _mapRootRow; }

		inline const std::unordered_map<RootUID, App::ECS::Entity> & getMapRootToEntity() const
		{
			return _mapRootEntity;
		}

	  private:
		/**
		 * @brief Root rows of the model.
		 */
		std::vector<std::unique_ptr<Row>> _rows;

		/**
		 * @brief Maps for quick access to rows.
		 */
		// TODO: redo
		std::unordered_map<App::ECS::Entity, const Row *> _mapEntityRow;
		std::unordered_map<RootUID, const Row *>		  _mapRootRow;
		std::unordered_map<RootUID, App::ECS::Entity>	  _mapRootEntity;

		/**
		 * @brief Callback on system construction to add it to the model.
		 */
		void _onSystemLoad( const App::Events::SystemLoad & p_e );

		/**
		 * @brief Callback on system destruction to remove it from the model.
		 */
		void _onSystemDestroy( App::ECS::Registry &, App::ECS::Entity );
	};

} // namespace VTX::UI::QT
#endif
