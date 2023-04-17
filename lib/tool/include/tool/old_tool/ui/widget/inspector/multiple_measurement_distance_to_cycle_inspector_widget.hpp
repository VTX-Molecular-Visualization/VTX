#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MEASUREMENT_DISTANCE_TO_CYCLE__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MEASUREMENT_DISTANCE_TO_CYCLE__

#include "tool/old_tool/model/measurement/distance_to_cycle.hpp"
#include <QPushButton>
#include <QWidget>
#include <ui/old_ui/ui/widget/custom_widget/collapsing_header_widget.hpp>
#include <ui/old_ui/ui/widget/custom_widget/qt_multi_data_field.hpp>
#include <ui/old_ui/ui/widget/custom_widget/vector3_widget.hpp>
#include <ui/old_ui/ui/widget/inspector/inspector_section.hpp>
#include <ui/old_ui/ui/widget/inspector/multiple_model_inspector_widget.hpp>

namespace VTX::UI::Widget::Inspector
{
	// class MultipleMeasurmentDistanceToCycleWidget :
	//	public MultipleModelInspectorWidget<Model::Measurement::DistanceToCycle>
	//{
	//	VTX_WIDGET

	//  public:
	//	~MultipleMeasurmentDistanceToCycleWidget();

	//	void localize() override;

	//  protected:
	//	MultipleMeasurmentDistanceToCycleWidget( QWidget * p_parent = nullptr );

	//	void		 _setupUi( const QString & p_name ) override;
	//	virtual void _setupSlots() override;

	//	void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	//  private:
	//	InspectorSectionVLayout *			_positionSection	 = nullptr;
	//	CustomWidget::Vector3Widget *		_positionInfoWidget	 = nullptr;
	//	InspectorSectionVLayout *			_infoSection		 = nullptr;
	//	CustomWidget::QLineEditMultiField * _nameWidget			 = nullptr;
	//	QPushButton *						_setAutoNameWidget	 = nullptr;
	//	CustomWidget::QLabelMultiField *	_distanceLabelWidget = nullptr;
	//	QPushButton *						_goToFirstAtom		 = nullptr;
	//	QPushButton *						_goToSecondAtom		 = nullptr;

	//	void _resetFieldStates( const SectionFlag & p_flag );

	//	void _setAutoNameAction() const;
	//	void _renameAction() const;
	//	void _orientOnFirstAtom() const;
	//	void _orientOnSecondAtom() const;
	//};
} // namespace VTX::UI::Widget::Inspector

#endif
