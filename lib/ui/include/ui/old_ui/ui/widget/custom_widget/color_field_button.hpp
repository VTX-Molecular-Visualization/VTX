#ifndef __VTX_UI_WIDGET_CUSTOM_COLOR_FIELD_BUTTON__
#define __VTX_UI_WIDGET_CUSTOM_COLOR_FIELD_BUTTON__

#include <QColor>
#include <QColorDialog>
#include <QImage>
#include <QPaintEvent>
#include <QPushButton>
#include <app/old_app/color/rgba.hpp>
#include <ui/old_ui/ui/multi_data_field.hpp>
#include <ui/old_ui/ui/widget/base_manual_widget.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	class ColorFieldButton : public BaseManualWidget<QPushButton>, public TMultiDataFieldEquatable<const Color::Rgba>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~ColorFieldButton() = default;

		const Color::Rgba & getColor() const { return _color; };
		void				setColor( const Color::Rgba & p_color );

		void localize() override;

		void setEnabled( const bool p_enable );

		void resetState() override;

	  signals:
		void onValueChange( const Color::Rgba & p_newColor );

	  protected:
		ColorFieldButton( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		const Color::Rgba & _getValue() const override { return _color; }
		void				_setSingleValue( const Color::Rgba & p_value ) override { setColor( p_value ); }
		void				_displayDifferentsDataFeedback() override;

		virtual void paintEvent( QPaintEvent * ) override;

	  private:
		QColorDialog * _colorDialog;
		QImage		   _colorImage;

		Color::Rgba _color;
		Color::Rgba _colorOnDialogOpen;

		void _fillImageWithColor( const Color::Rgba & p_color );

		void _onClickButton();
		void _onColorChanged( const QColor & p_color );
		void _onCanceled();

		Color::Rgba _getRgbColorFromQColor( const QColor & p_color ) const;
		QColor		_getQColorFromRgbColor( const Color::Rgba & p_color ) const;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
