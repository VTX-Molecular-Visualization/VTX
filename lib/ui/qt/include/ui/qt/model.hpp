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
		using ItemIndex = SystemIndex;

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
			VIEWPOINT
		};

		/**
		 * @brief Roles for data retrieval.
		 */
		enum Roles
		{
			TypeRole = Qt::UserRole + 1,
			SystemRole,
			LocalRole,
			NameRole
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

	  private:
		/**
		 * @brief The systems contained in the model.
		 */
		std::vector<const Core::Struct::System *> _systems;
		// TODO: add viewpoints?

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
