#ifndef __VTX_UI_WIDGET_CUSTOM_AUTO_ROTATE_WIDGET__
#define __VTX_UI_WIDGET_CUSTOM_AUTO_ROTATE_WIDGET__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/vector3_widget.hpp"
#include <QPushButton>
#include <QWidget>
#include <app/old_app/generic/base_auto_rotate.hpp>
#include <unordered_set>

namespace VTX::UI::Widget::CustomWidget
{
	class AutoRotateWidget : public BaseManualWidget<QWidget>, public TMultiDataField<Generic::BaseAutoRotate>
	{
		VTX_WIDGET

	  public:
		void localize() override;

		void resetState() override;
		void updateWithNewValue( Generic::BaseAutoRotate & p_value ) override;

	  protected:
		AutoRotateWidget( QWidget * p_parent = nullptr );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _displayDifferentsDataFeedback() override;

	  private:
		QPushButton * _playButton = nullptr;

		CustomWidget::Vector3Widget *		   _orientationWidget = nullptr;
		CustomWidget::FloatFieldSliderWidget * _speedWidget		  = nullptr;

		std::unordered_set<Generic::BaseAutoRotate *> _targets = std::unordered_set<Generic::BaseAutoRotate *>();

		void _togglePlayAction();
		void _speedChange( const float p_speed ) const;
		void _orientationChange( const Vec3f & p_orientation ) const;
		void _orientationDragged( const Vec3f & p_delta ) const;
	};
} // namespace VTX::UI::Widget::CustomWidget

#endif
