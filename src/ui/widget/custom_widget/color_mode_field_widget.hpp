#ifndef __VTX_UI_WIDGET_COLORMODE_FIELD_WIDGET__
#define __VTX_UI_WIDGET_COLORMODE_FIELD_WIDGET__

#include "color/rgb.hpp"
#include "color_field_button.hpp"
#include "generic/base_colorable.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/qt_multi_data_field.hpp"
#include <QBoxLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class ColorModeFieldWidget :
		public BaseManualWidget<QWidget>,
		public TMultiDataField<const std::pair<Generic::COLOR_MODE, Color::Rgb>>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		~ColorModeFieldWidget() {};
		void localize() override;

		const Generic::COLOR_MODE & getColorMode() const { return _colorMode; };
		void						setColorMode( const Generic::COLOR_MODE p_colorMode );

		const Color::Rgb & getColor() { return _color; };
		void			   setColor( const Color::Rgb & p_color );

		void updateWithNewValue( const std::pair<Generic::COLOR_MODE, Color::Rgb> & p_value ) override;
		void resetState() override;

	  signals:
		void colorModeChanged( const Generic::COLOR_MODE & p_colorMode );
		void colorChanged( const Color::Rgb & p_color );

	  protected:
		ColorModeFieldWidget( QWidget * p_parent ) :
			BaseManualWidget( p_parent ), TMultiDataField<const std::pair<Generic::COLOR_MODE, Color::Rgb>>() {};

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refresh();

		void _displayDifferentsDataFeedback() override;

		void _colorModeChange( int index );
		void _applyColor( const Color::Rgb & p_color );
		// !V0.1
		// void _openColorSettings();

		bool _hasToDisplayColorButton( const Generic::COLOR_MODE & p_colorMode ) const;
		bool _hasToDisplaySettingButton( const Generic::COLOR_MODE & p_colorMode ) const;

	  private:
		Generic::COLOR_MODE _colorMode;
		Color::Rgb			_color;

		QHBoxLayout * _layout;

		ColorFieldButton * _colorSetButton;
		// !V0.1
		// QPushButton *		  _openColorSettingsButton;
		QComboBoxMultiField * _colorModeComboBox;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif