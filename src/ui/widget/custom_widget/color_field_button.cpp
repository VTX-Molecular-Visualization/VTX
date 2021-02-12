#include "color_field_button.hpp"
#include <QPainter>
#include <QRect>
#include <string>

namespace VTX::UI::Widget::CustomWidget
{
	ColorFieldButton::ColorFieldButton( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

	void ColorFieldButton::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		_colorDialog = new QColorDialog( this );
		// Set minimum size to prevent warning with window help icon
		_colorDialog->setMinimumSize( 100, 100 );
		_colorDialog->hide();

		_colorImage = QImage( 32, 32, QImage::Format::Format_RGB32 );

		_refresh();
	}

	void ColorFieldButton::_setupSlots()
	{
		connect( this, &QPushButton::clicked, this, &ColorFieldButton::_onClickButton );
		connect( _colorDialog,
				 QOverload<const QColor &>::of( &QColorDialog::currentColorChanged ),
				 this,
				 &ColorFieldButton::_onColorChanged );
		connect( _colorDialog, &QColorDialog::rejected, this, &ColorFieldButton::_onCanceled );
	}

	void ColorFieldButton::_onClickButton()
	{
		_colorOnDialogOpen = _color;
		_colorDialog->setCurrentColor( _getQColorFromRgbColor( _color ) );
		_colorDialog->open();
	}
	void ColorFieldButton::_onColorChanged( const QColor & p_color )
	{
		setColor( _getRgbColorFromQColor( p_color ) );
		emit onValueChange( _color );
	}
	void ColorFieldButton::_onCanceled()
	{
		setColor( _colorOnDialogOpen );
		emit onValueChange( _color );
	}

	void ColorFieldButton::_refresh() { _fillImageWithColor( _color ); }

	void ColorFieldButton::localize() {};

	void ColorFieldButton::setEnabled( const bool p_enable )
	{
		if ( !p_enable && _colorDialog->isVisible() )
			_colorDialog->reject();

		QPushButton::setEnabled( p_enable );
	}

	void ColorFieldButton::setColor( const Color::Rgb & p_color )
	{
		if ( _color != p_color )
		{
			_color = p_color;

			_refresh();
			repaint();
		}
	};

	void ColorFieldButton::_fillImageWithColor( const Color::Rgb & p_color )
	{
		QColor color = _getQColorFromRgbColor( p_color );
		_colorImage.fill( color );
	}
	Color::Rgb ColorFieldButton::_getRgbColorFromQColor( const QColor & p_color ) const
	{
		return Color::Rgb( p_color.red(), p_color.green(), p_color.blue() );
	}
	QColor ColorFieldButton::_getQColorFromRgbColor( const Color::Rgb & p_color ) const
	{
		return QColor( p_color.getR() * 255, p_color.getG() * 255, p_color.getB() * 255 );
	}

	void ColorFieldButton::paintEvent( QPaintEvent * p_paintEvent )
	{
		QPushButton::paintEvent( p_paintEvent );

		QPainter painter( this );
		painter.save();
		painter.setWorldMatrixEnabled( false );
		painter.setBrush( Qt::NoBrush );

		const int	border = 4;
		const QRect rect   = QRect( p_paintEvent->rect().x() + border,
									p_paintEvent->rect().y() + border,
									p_paintEvent->rect().width() - border * 2,
									p_paintEvent->rect().height() - ( border * 2 + 1 ) );

		Color::Rgb displayedColorRGB = _color;
		if ( !isEnabled() )
			displayedColorRGB /= 2;

		QColor displayedColor = _getQColorFromRgbColor( displayedColorRGB );

		painter.fillRect( rect, displayedColor );

		painter.setWorldMatrixEnabled( true );
		painter.restore();
	}

} // namespace VTX::UI::Widget::CustomWidget
