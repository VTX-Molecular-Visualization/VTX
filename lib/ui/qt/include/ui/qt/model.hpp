#ifndef __VTX_UI_QT_MODEL__
#define __VTX_UI_QT_MODEL__

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
		using GlobalIndex = ushort;

		/**
		 * @brief Scene items.
		 */
		enum struct E_ITEM : uint8_t
		{
			SYSTEM,
			CATEGORY,
			CHAIN,
			RESIDUE,
			ATOM,
			PATH,
			VIEWPOINT,

			COUNT
		};

		/**
		 * @brief Roles for data retrieval.
		 */
		enum Roles
		{
			ItemRole = Qt::UserRole + 1,
			GlobalRole,
			LocalRole,
			NameRole
		};

		/**
		 * @brief Root row structure.
		 */
		struct Row
		{
			int										   position;
			GlobalIndex								   index;
			App::ECS::Entity						   entity;
			E_ITEM									   item;
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
		 * @brief Pack minimum information to identify an item in the model into a single uint64.
		 */
		static quintptr pack( const E_ITEM, const GlobalIndex, const Index );

		/**
		 * @brief Unpack quintptr.
		 */
		static void unpack( const quintptr, E_ITEM &, GlobalIndex &, Index & );

		inline const std::unordered_map<GlobalIndex, const Row *> & getMapRows() const { return _mapGlobalIndexRow; }

	  private:
		static inline GlobalIndex _COUNTER = 0;

		/**
		 * @brief Root rows of the model.
		 */
		std::vector<std::unique_ptr<Row>> _rows;

		/**
		 * @brief Maps for quick access to rows.
		 */
		std::unordered_map<App::ECS::Entity, const Row *> _mapEntityRow;
		std::unordered_map<GlobalIndex, const Row *>	  _mapGlobalIndexRow;

		/**
		 * @brief Callback on system construction to add it to the model.
		 */
		void _onConstructSystem( App::ECS::Registry & p_r, App::ECS::Entity p_e );

		/**
		 * @brief Callback on system destruction to remove it from the model.
		 */
		void _onDestroySystem( App::ECS::Registry &, App::ECS::Entity );
	};

} // namespace VTX::UI::QT
#endif
