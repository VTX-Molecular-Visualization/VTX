#include "ui/qt/widget/color_picker.hpp"
#include "ui/qt/helper.hpp"
#include <QColorDialog>
#include <QPalette>

namespace VTX::UI::QT::Widget
{
	ColorPicker::ColorPicker( QWidget * p_parent )
	{
		connect(
			this,
			&QPushButton::clicked,
			[ this ]()
			{
				// Open button dialog.
				QColor		 colorOld = _color;
				QColorDialog dialog( _color, this );

				// Connect color changed.
				connect(
					&dialog,
					&QColorDialog::currentColorChanged,
					[ this ]( const QColor & p_color )
					{
						setColor( p_color );
						onColorChanged( p_color );
					}
				);

				dialog.exec();

				// Revert.
				if ( dialog.result() == QDialog::Rejected )
				{
					setColor( colorOld );
					onColorChanged( colorOld );
				}
			}
		);
	}

	ColorPicker::ColorPicker( const QColor & p_color, QWidget * p_parent ) : ColorPicker( p_parent )
	{
		setColor( p_color );
	}

	void ColorPicker::setColor( const QColor & p_color )
	{
		_color					  = p_color;
		Util::Color::Rgba color	  = Helper::fromQColor( p_color );
		QPalette		  palette = this->palette();
		palette.setColor( QPalette::Button, p_color );
		palette.setColor(
			QPalette::ButtonText, color.brightness() > 0.5f ? QColorConstants::Black : QColorConstants::White
		);
		setPalette( palette );
	}

} // namespace VTX::UI::QT::Widget
