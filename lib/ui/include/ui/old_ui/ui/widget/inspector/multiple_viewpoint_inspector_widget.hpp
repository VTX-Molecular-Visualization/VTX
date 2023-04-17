#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_VIEWPOINT__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_VIEWPOINT__

#include "ui/old_ui/ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/embedded_data_push_button.hpp"
#include "ui/old_ui/ui/widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/old_ui/ui/widget/custom_widget/transform_widget.hpp"
#include "ui/old_ui/ui/widget/inspector/inspector_section.hpp"
#include "ui/old_ui/ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include <QPushButton>
#include <QWidget>
#include <app/old_app/model/viewpoint.hpp>

namespace VTX::UI::Widget::Inspector
{
	class MultipleViewpointWidget : public MultipleModelInspectorWidget<Model::Viewpoint>
	{
		VTX_WIDGET

	  public:
		~MultipleViewpointWidget();

		void localize() override;

	  protected:
		MultipleViewpointWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	  private:
		InspectorSectionVLayout *		_transformSection = nullptr;
		CustomWidget::TransformWidget * _transformWidget  = nullptr;

		QWidget *				  _actionContainer = nullptr;
		InspectorSectionVLayout * _actionSection   = nullptr;

		QPushButton * _relocateButton = nullptr;

		std::vector<CustomWidget::EmbeddedDataPushButton *> _gotoButtons
			= std::vector<CustomWidget::EmbeddedDataPushButton *>();

		void _resetFieldStates( const SectionFlag & p_flag );

		void _onTransformChange( const Math::Transform & ) const;
		void _onPositionDragged( const Vec3f & ) const;
		void _onRotationDragged( const Vec3f & ) const;

		void _goToAction( const QVariant & p_viewpointIndex ) const;
		void _relocateAction() const;
	};
} // namespace VTX::UI::Widget::Inspector

#endif
