#ifndef __VTX_UI_WIDGET_ANALYSIS_ALIGN_PARAMETERS__
#define __VTX_UI_WIDGET_ANALYSIS_ALIGN_PARAMETERS__

#include "analysis/structural_alignment.hpp"
#include "ui/layout/attribute_list_layout.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/widget/custom_widget/folding_button.hpp"
#include "ui/widget/custom_widget/integer_field_slider_widget.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QString>
#include <QWidget>

namespace VTX::UI::Widget::Analysis::StructuralAlignment
{
	class AlignParametersWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

		using AlignmentParameters = VTX::Analysis::StructuralAlignment::AlignmentParameters;

	  public:
		void localize() override;

		AlignmentParameters * generateParameters() const;
		void				  applyParameter( const AlignmentParameters & p_alignParameter );

	  protected:
		AlignParametersWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		AlignmentParameters * _generateCEAlignParameter() const;
		AlignmentParameters * _generateCEAlignPymolParameter() const;

		void _methodHasChanged( const int p_methodIndex );

	  private:
		QComboBox * _methodWidget = nullptr;

		Layout::AttributeListLayout * _attributeLayout = nullptr;

		CustomWidget::IntegerFieldSliderWidget * _windowSizeWidget = nullptr;
		CustomWidget::IntegerFieldSliderWidget * _gapMaxWidget	   = nullptr;
		CustomWidget::IntegerFieldSliderWidget * _maxPathWidget	   = nullptr;

		CustomWidget::FloatFieldSliderWidget * _d0Widget = nullptr;
		CustomWidget::FloatFieldSliderWidget * _d1Widget = nullptr;

		QCheckBox * _considerHiddenResiduesButton = nullptr;
		QCheckBox * _considerWaterButton		  = nullptr;
	};

} // namespace VTX::UI::Widget::Analysis::StructuralAlignment

#endif
