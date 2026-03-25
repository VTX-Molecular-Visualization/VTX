#ifndef __VTX_UI_QT_WIDGET_SELECTION__
#define __VTX_UI_QT_WIDGET_SELECTION__

#include "ui/qt/widget/transform.hpp"
#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>
#include <app/ecs.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget displaying the current selection.
	 */
	class Selection : public QWidget
	{
	  public:
		Selection( QWidget * const p_parent = nullptr );
		~Selection();

		void refresh();

	  private:
		QPointer<QVBoxLayout>		_layout;
		QPointer<Widget::Transform> _transform;

		/**
		 * @brief Currently selected entities.
		 */
		std::vector<App::ECS::Entity> _entities;

		void _transformUpdated( App::ECS::Registry &, App::ECS::Entity );
	};
} // namespace VTX::UI::QT::Widget

#endif
