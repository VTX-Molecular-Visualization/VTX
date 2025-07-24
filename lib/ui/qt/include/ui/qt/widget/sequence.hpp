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
		// Q_OBJECT

	  public:
		Sequence( App::Component::Chemistry::System &, QWidget * );

		// void setSequence( const std::vector<QChar> &, const std::vector<QColor> & );

		// signals:
		// void residueClicked( size_t );

	  protected:
		void paintEvent( QPaintEvent * ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void resizeEvent( QResizeEvent * ) override;

	  private:
		static const uint DEFAULT_CHAR_WIDTH  = 12;
		static const uint DEFAULT_CHAR_HEIGHT = 18;

		// std::vector<QChar>	_sequence;
		// std::vector<QColor> _colors;
		App::Component::Chemistry::System & _system;

		void updateScrollBars();
	};

} // namespace VTX::UI::QT::Widget

#endif
