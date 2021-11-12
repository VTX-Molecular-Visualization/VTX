#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_VIEWPOINT__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_VIEWPOINT__

#include "model/viewpoint.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/widget/custom_widget/transform_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include <QWidget>

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

		CustomWidget::QPushButtonMultiField * _gotoButton = nullptr;

		void _resetFieldStates( const SectionFlag & p_flag );

		void _onTransformChange( const Math::Transform & ) const;
		void _onPositionDragged( const Vec3f & ) const;
		void _onRotationDragged( const Vec3f & ) const;

		void _goToAction() const;
	};
} // namespace VTX::UI::Widget::Inspector

#endif