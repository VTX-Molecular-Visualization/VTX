#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "ui/qt/core/base_dock_widget.hpp"
#include "ui/qt/widget/tree.hpp"
#include <QPointer>
#include <QTreeWidget>
#include <core/struct/system.hpp>

namespace VTX::UI::QT::DockWidget
{

	/**
	 * @brief Display a tree widget with loaded systems.
	 * Load only minimal data on expand/collapse.
	 */
	class Scene : public Core::BaseDockWidget<Scene>
	{
	  public:
		Scene( QWidget * p_parent ) : Core::BaseDockWidget<Scene>( "Scene", p_parent ) {}

	  private:
		enum struct E_DEPTH
		{
			TREE = 0,
			SYSTEM,
			CHAIN,
			RESIDUE,
			ATOM
		};

		enum struct E_VISIBILITY
		{
			VISIBLE = 0,
			HIDDEN,
			PARTIAL
		};

		using WidgetData = Index;

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
			Index			 childrenCount;
			E_VISIBILITY	 visibility;
		};

		QPointer<QTreeWidget> _tree;
	};

} // namespace VTX::UI::QT::DockWidget

#endif
