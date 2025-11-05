#ifndef __VTX_UI_QT_WIDGET_SEQUENCE__
#define __VTX_UI_QT_WIDGET_SEQUENCE__

#include "ui/qt/widget/base_widget.hpp"
#include <QAbstractScrollArea>
#include <core/struct/system.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Custom widget for sequence that paint only the displayed area.
	 */
	class Sequence : public QAbstractScrollArea
	{
	  public:
		Sequence( const App::ECS::Entity, QWidget * );

	  protected:
		void paintEvent( QPaintEvent * ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void resizeEvent( QResizeEvent * ) override;

	  private:
		const App::ECS::Entity _system;

		void updateScrollBars();
	};

} // namespace VTX::UI::QT::Widget

#endif
