#ifndef __VTX_UI_QT_WIDGET_TREE_SYSTEM__
#define __VTX_UI_QT_WIDGET_TREE_SYSTEM__

#include "ui/qt/widget/trajectory_player.hpp"
#include "ui/qt/widget/tree/base_tree.hpp"
#include "ui/qt/widget/tree/system_model.hpp"
#include "ui/qt/widget/tree/system_selection_model.hpp"
#include <QContextMenuEvent>
#include <QTreeView>
#include <QVBoxLayout>

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Container widget holding a system tree and optional trajectory player.
	 */
	class System : public QWidget
	{
		Q_OBJECT

	  public:
		System( const App::ECS::Entity, QWidget * );

		/**
		 * @brief Access the underlying tree view.
		 */
		inline QTreeView * treeView() { return _tree; }

		/**
		 * @brief Forward common tree view methods.
		 */
		void setSelectionMode( QAbstractItemView::SelectionMode p_mode ) { _tree->setSelectionMode( p_mode ); }
		QWidget * viewport() { return _tree->viewport(); }

		inline const SystemModel &	  getSystemModel() const { return *static_cast<SystemModel *>( _tree->model() ); }
		inline SystemSelectionModel & getSystemSelectionModel()
		{
			return *static_cast<SystemSelectionModel *>( _tree->selectionModel() );
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

		/**
		 * @brief Trajectory player (visible only if system has trajectory).
		 */
		TrajectoryPlayer * _trajectoryPlayer = nullptr;

		/**
		 * @brief Inner tree view class.
		 */
		class InnerTree : public Widget::Tree::BaseTree<InnerTree, QTreeView>
		{
		  public:
			InnerTree( QWidget * p_parent ) : Widget::Tree::BaseTree<InnerTree, QTreeView>( p_parent ) {}
			void contextMenuEvent( QContextMenuEvent * p_e ) override;
		};

		InnerTree * _tree;

		/**
		 * @brief Called when a trajectory is added to any system.
		 */
		void _onTrajectoryCreated( App::ECS::Registry &, App::ECS::Entity p_entity );
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
