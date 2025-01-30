#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QPointer>
#include <QTreeWidget>
#include <app/component/chemistry/system.hpp>
/*#include <app/component/scene/scene_item_component.hpp>
*/

namespace VTX::UI::QT::DockWidget
{

	/**
	 * @brief Display a tree widget with loaded systems.
	 * Load only minimal data on expand/collapse.
	 */
	class Scene : public Core::BaseDockWidget<Scene>
	{
	  public:
		Scene( QWidget * );

	  private:
		enum struct E_DEPTH
		{
			TREE = 0,
			SYSTEM,
			CHAIN,
			RESIDUE,
			ATOM
		};

		using WidgetData = size_t;

		/**
		 * @brief Load data function.
		 */
		using LoadFunc = std::function<void( const E_DEPTH, QTreeWidgetItem * const )>;

		/**
		 * @brief Store data to create a tree item.
		 */
		struct TreeItemData
		{
			std::string_view name;
			WidgetData		 data;
			size_t			 childrenCount;
		};

		QPointer<QTreeWidget> _tree;

		/**
		 * @brief Map top level items to data loading functions (to request App).
		 */
		std::map<const QTreeWidgetItem * const, LoadFunc> _loadFuncs;

		/**
		 * @brief Map top level items to system components.
		 */
		std::map<const QTreeWidgetItem * const, App::Component::Chemistry::System * const> _systemComponents;

		/**
		 * @brief Add a tree item.
		 * @param the TreeItemData to add.
		 * @param the parent item.
		 * @param the optional loading function.
		 * @param the optional system component.
		 */
		void _addTreeItem(
			const TreeItemData &,
			QTreeWidgetItem * const							   = nullptr,
			std::optional<const LoadFunc>					   = std::nullopt,
			std::optional<App::Component::Chemistry::System *> = std::nullopt
		);

		/**
		 * @brief Get the depth of the item.
		 * @param p_item the widget item.
		 * @return [E_DEPTH, QTreeWidgetItem * const] the depth and the top level item.
		 */
		std::pair<E_DEPTH, QTreeWidgetItem * const> _getDepth( QTreeWidgetItem * const p_item ) const;

		/**
		 * @brief Reset the item tree (unload content on collapse).
		 * @param the item to reset.
		 */
		void _resetTreeItem( QTreeWidgetItem * const );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
