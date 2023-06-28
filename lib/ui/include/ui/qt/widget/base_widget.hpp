#ifndef __VTX_UI_QT_WIDGET_BASE_WIDGET__
#define __VTX_UI_QT_WIDGET_BASE_WIDGET__

#include <app/old/core/event/base_event_receiver_vtx.hpp>
#include <QPainter>
#include <QStyleOption>
#include <QWidget>

namespace VTX::UI::QT::Widget
{
	template<typename W, typename T, typename = std::enable_if<std::is_base_of<QWidget, W>::value>>
	class BaseWidget : public W, public T, public VTX::App::Old::Core::Event::BaseEventReceiverVTX
	{
	  public:
		BaseWidget( QWidget * p_parent = 0 ) : W( p_parent ) { T::setupUi( this ); }

		// Need to override paintEvent to read style on custom widgets
		inline void paintEvent( QPaintEvent * event ) override
		{
			QStyleOption opt;
			opt.initFrom( this );
			QPainter p( this );
			W::style()->drawPrimitive( QStyle::PE_Widget, &opt, &p, this );

			W::paintEvent( event );
		};
	};
} // namespace VTX::UI::QT::Widget

#endif
