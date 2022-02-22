#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MEASUREMENT_ANGLE__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MEASUREMENT_ANGLE__

#include "color/rgb.hpp"
#include "model/measurement/angle.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/custom_widget/color_field_button.hpp"
#include "ui/widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/widget/custom_widget/vector3_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::Widget::Inspector
{
	class MultipleMeasurmentAngleWidget : public MultipleModelInspectorWidget<Model::Measurement::Angle>
	{
		VTX_WIDGET

	  public:
		~MultipleMeasurmentAngleWidget();

		void localize() override;

	  protected:
		MultipleMeasurmentAngleWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	  private:
		InspectorSectionVLayout *	  _positionSection	  = nullptr;
		CustomWidget::Vector3Widget * _positionInfoWidget = nullptr;

		InspectorSectionVLayout *		 _displaySection = nullptr;
		CustomWidget::ColorFieldButton * _colorWidget	 = nullptr;

		InspectorSectionVLayout *			_infoSection	   = nullptr;
		CustomWidget::QLineEditMultiField * _nameWidget		   = nullptr;
		QPushButton *						_setAutoNameWidget = nullptr;
		CustomWidget::QLabelMultiField *	_angleLabelWidget  = nullptr;
		QPushButton *						_goToFirstAtom	   = nullptr;
		QPushButton *						_goToSecondAtom	   = nullptr;
		QPushButton *						_goToThirdAtom	   = nullptr;

		void _resetFieldStates( const SectionFlag & p_flag );

		void _setAutoNameAction() const;
		void _setLabelColor( const Color::Rgb & p_color ) const;
		void _renameAction() const;
		void _orientOnFirstAtom() const;
		void _orientOnSecondAtom() const;
		void _orientOnThirdAtom() const;
	};
} // namespace VTX::UI::Widget::Inspector

#endif
