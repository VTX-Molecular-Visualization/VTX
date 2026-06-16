#ifndef __VTX_UI_QT_WIDGET_VIEWPOINT__
#define __VTX_UI_QT_WIDGET_VIEWPOINT__

#include "ui/qt/widget/transform.hpp"
#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>
#include <app/ecs.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget to inspect and edit a saved viewpoint.
	 */
	class ViewPoint : public QWidget
	{
	  public:
		ViewPoint( const Entity, QWidget * const p_parent = nullptr );
		~ViewPoint();

	  private:
		const Entity _entity;

		QPointer<QVBoxLayout> _layout;
		QPointer<Transform>   _transform;

		Util::EventHub::Connection _connTransformChanged;

		void _setTransform();
		void _transformUpdated( Registry &, Entity );
	};
} // namespace VTX::UI::QT::Widget

#endif
