#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/widget/tree/camera.hpp"
#include "ui/qt/widget/tree/color_layout_presets.hpp"
#include "ui/qt/widget/tree/graphics_config_presets.hpp"
#include "ui/qt/widget/tree/representation_presets.hpp"
#include "ui/qt/widget/tree/system.hpp"
#include "ui/qt/widget/tree/trajectory_player.hpp"
#include <QComboBox>
#include <QPointer>
#include <QTimer>
#include <app/threading/base_thread.hpp>
#include <ui/qt/widget/thread.hpp>

namespace VTX::UI::QT::DockWidget
{

	/**
	 * @brief Display a tree widget with loaded systems.
	 * TODO: add searchbar with QSortFilterProxyModel.
	 * TODO: add option to sync tree expand with selection model.
	 */
	class Scene : public BaseDockWidget<Scene>
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Scene( QWidget * p_parent );

	  private:
		/**
		 * @brief Trajoectory players trees.
		 */
		std::unordered_map<Entity, QPointer<Widget::Tree::TrajectoryPlayer>> _mapTrajTreeWidgets;

		QPointer<Widget::Tree::GraphicsConfigPresets> _treeGraphicsConfigPresets;
		QPointer<Widget::Tree::ColorLayoutPresets>	  _treeColorLayoutPresets;
		QPointer<Widget::Tree::RepresentationPresets> _treeRepresentationPresets;
		QPointer<Widget::Tree::Camera>				  _treeCamera;

		/**
		 * @brief System trees.
		 */
		std::unordered_map<Entity, QPointer<Widget::Tree::System>> _mapSystemTreeWidgets;

		/**
		 * @brief Thread widgets.
		 */
		std::unordered_map<App::Threading::BaseThread::ID, QPointer<Widget::Thread>> _mapThreadWidgets;

		/**
		 * @brief Custom spacer to fill empty space.
		 */
		QPointer<QWidget> _filler;

		/**
		 * @brief Remove tree when system is removed.
		 */
		void _onCameraConstruct( Registry &, Entity );

		/**
		 * @brief Add tree when system is loaded.
		 */
		void _onSystemLoad( const App::Events::SystemLoad & );

		/**
		 * @brief Remove tree when system is removed.
		 */
		void _onSystemDestroy( Registry &, Entity );

		/**
		 * @brief Update selection.
		 */
		void _onUpdateVisibility( Registry &, Entity p_e );

		/**
		 * @brief Update selection.
		 */
		void _onUpdateSelection( Registry &, Entity p_e );

		/**
		 * @brief Lock or unlock selection.
		 */
		void _onSelectionLocked( const Events::SelectionLocked & );

		/**
		 * @brief Called when a trajectory is added to any system.
		 */
		void _onTrajectoryCreated( Registry &, Entity p_entity );

		/**
		 * @brief Add or update thread widget.
		 */
		void _onThreadProgress( const App::Events::ThreadProgress & );

		/**
		 * @brief Remove thread widget.
		 */
		void _onThreadTerminated( const App::Events::ThreadTerminated & );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
