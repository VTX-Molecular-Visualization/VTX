#ifndef __VTX_UI_WIDGET_CUSTOM_EMBEDDED_DATA_PUSH_BUTTON__
#define __VTX_UI_WIDGET_CUSTOM_EMBEDDED_DATA_PUSH_BUTTON__

#include <QPushButton>
#include <QVariant>

namespace VTX::UI::Widget::CustomWidget
{
	class EmbeddedDataPushButton : public QPushButton
	{
		Q_OBJECT

	  public:
		EmbeddedDataPushButton( QWidget * const p_parent = nullptr ) : QPushButton( p_parent )
		{
			connect( this, &EmbeddedDataPushButton::clicked, this, &EmbeddedDataPushButton::emitClickedWithData );
		}
		void setData( const QVariant & p_data ) { _data = p_data; }

	  signals:
		void embeddedDataclicked( const QVariant & p_data );

	  private:
		QVariant _data;
		void	 emitClickedWithData() { emit embeddedDataclicked( _data ); }
	};
} // namespace VTX::UI::Widget::CustomWidget

#endif
