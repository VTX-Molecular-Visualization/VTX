#ifndef __VTX_UI_WIDGET_CUSTOM_COLOR_FIELD_BUTTON__
#define __VTX_UI_WIDGET_CUSTOM_COLOR_FIELD_BUTTON__

#include <QColor>
#include <QColorDialog>
#include <QImage>
#include <QPaintEvent>
#include <QPushButton>
#include <util/color/rgba.hpp>
#include <ui/old_ui/ui/multi_data_field.hpp>
#include <ui/old_ui/ui/widget/base_manual_widget.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	class ColorFieldButton : public BaseManualWidget<QPushButton>, public TMultiDataFieldEquatable<const Util::Color::Rgba>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~ColorFieldButton() = default;

		const Util::Color::Rgba & getColor() const { return _color; };
		void				setColor( const Util::Color::Rgba & p_color );

		void localize() override;

		void setEnabled( const bool p_enable );

		void resetState() override;

	  signals:
		void onValueChange( const Util::Color::Rgba & p_newColor );

	  protected:
		ColorFieldButton( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		const Util::Color::Rgba & _getValue() const override { return _color; }
		void				_setSingleValue( const Util::Color::Rgba & p_value ) override { setColor( p_value ); }
		void				_displayDifferentsDataFeedback() override;

		virtual void paintEvent( QPaintEvent * ) override;

	  private:
		QColorDialog * _colorDialog;
		QImage		   _colorImage;

		Util::Color::Rgba _color;
		Util::Color::Rgba _colorOnDialogOpen;

		void _fillImageWithColor( const Util::Color::Rgba & p_color );

		void _onClickButton();
		void _onColorChanged( const QColor & p_color );
		void _onCanceled();

		Util::Color::Rgba _getRgbColorFromQColor( const QColor & p_color ) const;
		QColor		_getQColorFromRgbColor( const Util::Color::Rgba & p_color ) const;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
