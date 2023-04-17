#ifndef __VTX_UI_QT_WIDGET_CUSTOM_INDEXED_ACTION__
#define __VTX_UI_QT_WIDGET_CUSTOM_INDEXED_ACTION__

#include <QAction>
#include <QWidget>

namespace VTX::UI::QT::Widget::CustomWidget
{
	class IndexedAction : public QAction
	{
		Q_OBJECT

	  public:
		IndexedAction( const int p_index, QWidget * const p_parent ) : QAction( p_parent ), _index( p_index )
		{
			connect( this, &QAction::triggered, this, &IndexedAction::_sendTriggeredWithData );
		};

	  signals:
		void triggeredWithIndex( const int & p_index );

	  private:
		inline void _sendTriggeredWithData() { emit triggeredWithIndex( _index ); };

		const int _index;
	};
} // namespace VTX::UI::QT::Widget::CustomWidget
#endif
