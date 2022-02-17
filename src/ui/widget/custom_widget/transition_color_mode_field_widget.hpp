#ifndef __VTX_UI_WIDGET_TRANSITION_COLOR_MODE_FIELD_WIDGET__
#define __VTX_UI_WIDGET_TRANSITION_COLOR_MODE_FIELD_WIDGET__

#include "color/rgb.hpp"
#include "color_field_button.hpp"
#include "generic/base_colorable.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/qt_multi_data_field.hpp"
#include <QBoxLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class TransitionColorModeFieldWidget :
		public BaseManualWidget<QWidget>,
		public TMultiDataField<const Generic::COLOR_TRANSITION_MODE>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		~TransitionColorModeFieldWidget() {};
		void localize() override;

		const Generic::COLOR_TRANSITION_MODE & getColorMode() const { return _colorMode; };
		void								   setColorMode( const Generic::COLOR_TRANSITION_MODE & p_colorMode );

		void updateWithNewValue( const Generic::COLOR_TRANSITION_MODE & ) override;
		void resetState() override;

	  signals:
		void colorModeChanged( const Generic::COLOR_TRANSITION_MODE & p_colorMode );
		void colorChanged( const Color::Rgb & p_color );

	  protected:
		TransitionColorModeFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ), TMultiDataField() {};

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refresh();

		void _displayDifferentsDataFeedback() override;

		void _colorModeChange( int index );
		void _applyColor( const Color::Rgb & p_color );
		// !V0.1
		// void _openColorSettings();

	  private:
		Generic::COLOR_TRANSITION_MODE _colorMode;
		QHBoxLayout *				   _layout;
		// !V0.1
		// QPushButton *						_openColorSettingsButton;
		CustomWidget::QComboBoxMultiField * _colorModeComboBox;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
