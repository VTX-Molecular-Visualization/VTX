#ifndef __VTX_UI_QT_WIDGET_SEQUENCE__
#define __VTX_UI_QT_WIDGET_SEQUENCE__

#include "ui/qt/core/base_widget.hpp"
#include <QAbstractScrollArea>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>

namespace VTX::UI::QT::Widget
{
	class Sequence : public QAbstractScrollArea
	{
	  public:
		Sequence( const App::Component::Chemistry::System &, QWidget * );

	  protected:
		void paintEvent( QPaintEvent * ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void resizeEvent( QResizeEvent * ) override;

	  private:
		const App::Component::Chemistry::System & _system;

		void updateScrollBars();
	};

} // namespace VTX::UI::QT::Widget

#endif
