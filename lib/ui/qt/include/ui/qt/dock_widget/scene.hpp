#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/widget/tree/camera.hpp"
#include "ui/qt/widget/tree/system.hpp"
#include "ui/qt/widget/tree/trajectory_player.hpp"
#include <QComboBox>
#include <QPointer>
#include <QTimer>

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
		std::unordered_map<App::ECS::Entity, QPointer<Widget::Tree::TrajectoryPlayer>> _mapTrajTreeWidgets;
		/**
		 * @brief System trees.
		 */
		std::unordered_map<App::ECS::Entity, QPointer<Widget::Tree::System>> _mapSystemTreeWidgets;

		/**
		 * @brief Timer for updating trajectory player display.
		 */
		QTimer * _updateTimer = nullptr;

		/**
		 * @brief Custom spacer to fill empty space.
		 */
		QPointer<QWidget> _filler;

		/**
		 * @brief Remove tree when system is removed.
		 */
		void _onCameraConstruct( App::ECS::Registry &, App::ECS::Entity );

		/**
		 * @brief Add tree when system is loaded.
		 */
		void _onSystemLoad( const App::Events::SystemLoad & );

		/**
		 * @brief Remove tree when system is removed.
		 */
		void _onSystemDestroy( App::ECS::Registry &, App::ECS::Entity );

		/**
		 * @brief Update selection.
		 */
		void _onUpdateVisibility( App::ECS::Registry &, App::ECS::Entity p_e );

		/**
		 * @brief Update selection.
		 */
		void _onUpdateSelection( App::ECS::Registry &, App::ECS::Entity p_e );

		/**
		 * @brief Lock or unlock selection.
		 */
		void _onSelectionLocked( const Events::SelectionLocked & );
		void _onUpdateTimer();

		/**
		 * @brief Called when a trajectory is added to any system.
		 */
		void _onTrajectoryCreated( App::ECS::Registry &, App::ECS::Entity p_entity );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
