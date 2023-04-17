#ifndef __VTX_UI_WIDGET_RIBBON_COLOR_MODE_FIELD_WIDGET__
#define __VTX_UI_WIDGET_RIBBON_COLOR_MODE_FIELD_WIDGET__

#include "color_field_button.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/qt_multi_data_field.hpp"
#include <QBoxLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <app/old_app/color/rgba.hpp>
#include <app/old_app/generic/base_colorable.hpp>
#include <app/old_app/model/secondary_structure.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	class RibbonColorModeFieldWidget :
		public BaseManualWidget<QWidget>,
		public TMultiDataField<const std::pair<Generic::SECONDARY_STRUCTURE_COLOR_MODE, Color::Rgba>>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		~RibbonColorModeFieldWidget() {};
		void localize() override;

		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & getColorMode() const { return _colorMode; };
		void setColorMode( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode );

		const Color::Rgba & getColor() { return _color; };
		void				setColor( const Color::Rgba & p_color );

		void updateWithNewValue(
			const std::pair<Generic::SECONDARY_STRUCTURE_COLOR_MODE, Color::Rgba> & p_value ) override;
		void resetState() override;

	  signals:
		void colorModeChanged( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode );
		void colorChanged( const Color::Rgba & p_color );

	  protected:
		RibbonColorModeFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ), TMultiDataField() {};

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refresh();

		void _displayDifferentsDataFeedback() override;

		void _colorModeChange( int index );
		void _applyColor( const Color::Rgba & p_color );
		// !V0.1
		// void _openColorSettings();

	  private:
		Generic::SECONDARY_STRUCTURE_COLOR_MODE _colorMode;
		Color::Rgba								_color;

		QHBoxLayout * _layout;

		ColorFieldButton * _colorSetButton;
		// !V0.1
		// QPushButton *						_openColorSettingsButton;
		CustomWidget::QComboBoxMultiField * _colorModeComboBox;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
