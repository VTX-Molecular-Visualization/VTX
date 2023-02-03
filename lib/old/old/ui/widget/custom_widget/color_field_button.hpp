#ifndef __VTX_UI_WIDGET_CUSTOM_COLOR_FIELD_BUTTON__
#define __VTX_UI_WIDGET_CUSTOM_COLOR_FIELD_BUTTON__

#include "color/rgb.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QColor>
#include <QColorDialog>
#include <QImage>
#include <QPaintEvent>
#include <QPushButton>

namespace VTX::UI::Widget::CustomWidget
{
	class ColorFieldButton : public BaseManualWidget<QPushButton>, public TMultiDataFieldEquatable<const Color::Rgb>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~ColorFieldButton() = default;

		const Color::Rgb & getColor() const { return _color; };
		void			   setColor( const Color::Rgb & p_color );

		void localize() override;

		void setEnabled( const bool p_enable );

		void resetState() override;

	  signals:
		void onValueChange( const Color::Rgb & p_newColor );

	  protected:
		ColorFieldButton( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		const Color::Rgb & _getValue() const override { return _color; }
		void			   _setSingleValue( const Color::Rgb & p_value ) override { setColor( p_value ); }
		void			   _displayDifferentsDataFeedback() override;

		virtual void paintEvent( QPaintEvent * ) override;

	  private:
		QColorDialog * _colorDialog;
		QImage		   _colorImage;

		Color::Rgb _color;
		Color::Rgb _colorOnDialogOpen;

		void _fillImageWithColor( const Color::Rgb & p_color );

		void _onClickButton();
		void _onColorChanged( const QColor & p_color );
		void _onCanceled();

		Color::Rgb _getRgbColorFromQColor( const QColor & p_color ) const;
		QColor	   _getQColorFromRgbColor( const Color::Rgb & p_color ) const;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
