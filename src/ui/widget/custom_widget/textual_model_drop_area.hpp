#ifndef __VTX_UI_WIDGET_TEXTUTAL_MODEL_DROP_AREA__
#define __VTX_UI_WIDGET_TEXTUTAL_MODEL_DROP_AREA__

#include "model_drop_area.hpp"
#include <QLabel>

namespace VTX::UI::Widget::CustomWidget
{
	class TextualModelDropArea : public ModelDropArea
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~TextualModelDropArea() {};
		void localize() override;

	  protected:
		TextualModelDropArea( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QLabel * p_dropLabel = nullptr;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
