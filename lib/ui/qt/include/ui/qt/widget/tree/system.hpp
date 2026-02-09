#ifndef __VTX_UI_QT_WIDGET_TREE_SYSTEM__
#define __VTX_UI_QT_WIDGET_TREE_SYSTEM__

#include "ui/qt/widget/tree/base_tree.hpp"
#include "ui/qt/widget/tree/system_model.hpp"
#include "ui/qt/widget/tree/system_selection_model.hpp"
#include <QContextMenuEvent>
#include <QTreeView>
#include <QVBoxLayout>

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Scene tree
	 */
	class System : public Widget::Tree::BaseTree<System, QTreeView>
	{
		Q_OBJECT

	  public:
		System( const App::ECS::Entity, QWidget * );
		void						  contextMenuEvent( QContextMenuEvent * p_e ) override;
		inline const SystemModel &	  getSystemModel() { return *static_cast<SystemModel *>( model() ); }
		inline const SystemModel &	  getSystemModel() const { return *static_cast<SystemModel *>( model() ); }
		inline SystemSelectionModel & getSystemSelectionModel()
		{
			return *static_cast<SystemSelectionModel *>( selectionModel() );
		}

	  private:
		/**
		 * @brief Entity to display.
		 */
		const App::ECS::Entity _system;

		/**
		 * @brief Layout.
		 */
		QVBoxLayout * _layout;
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
