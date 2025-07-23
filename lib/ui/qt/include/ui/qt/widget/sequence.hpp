#ifndef __VTX_UI_QT_WIDGET_SEQUENCE__
#define __VTX_UI_QT_WIDGET_SEQUENCE__

#include "ui/qt/core/base_widget.hpp"
#include <QAbstractScrollArea>

namespace VTX::UI::QT::Widget
{
	class Sequence : public QAbstractScrollArea
	{
		Q_OBJECT

	  public:
		Sequence( QWidget * );

		void setSequence( const QVector<QChar> &, const QVector<QColor> & );

	  signals:
		void residueClicked( int );

	  protected:
		void paintEvent( QPaintEvent * ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void resizeEvent( QResizeEvent * ) override;

	  private:
		static const uint DEFAULT_CHAR_WIDTH  = 12;
		static const uint DEFAULT_CHAR_HEIGHT = 18;

		QVector<QChar>	_sequence;
		QVector<QColor> _colors;

		void updateScrollBars();
	};

} // namespace VTX::UI::QT::Widget

#endif
