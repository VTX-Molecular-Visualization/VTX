#include "ui/old_ui/ui/widget/custom_widget/textual_model_drop_area.hpp"
#include <QVBoxLayout>

namespace VTX::UI::Widget::CustomWidget
{
	TextualModelDropArea::TextualModelDropArea( QWidget * p_parent ) : ModelDropArea( p_parent ) {};

	void TextualModelDropArea::_setupUi( const QString & p_name )
	{
		ModelDropArea::_setupUi( p_name );

		QVBoxLayout * const layout = new QVBoxLayout( this );

		p_dropLabel = new QLabel( this );
		p_dropLabel->setAlignment( Qt::AlignmentFlag::AlignCenter );
		p_dropLabel->setText( "Drop model(s) here" );

		layout->addWidget( p_dropLabel );
	}

	void TextualModelDropArea::_setupSlots() { ModelDropArea::_setupSlots(); }

	void TextualModelDropArea::localize() { ModelDropArea::localize(); };

} // namespace VTX::UI::Widget::CustomWidget
