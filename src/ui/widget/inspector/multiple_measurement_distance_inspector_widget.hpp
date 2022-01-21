#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MEASUREMENT_DISTANCE__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MEASUREMENT_DISTANCE__

#include "model/measurement/distance.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/widget/custom_widget/vector3_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::Widget::Inspector
{
	class MultipleMeasurmentDistanceWidget : public MultipleModelInspectorWidget<Model::Measurement::Distance>
	{
		VTX_WIDGET

	  public:
		~MultipleMeasurmentDistanceWidget();

		void localize() override;

	  protected:
		MultipleMeasurmentDistanceWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	  private:
		InspectorSectionVLayout *			_positionSection	 = nullptr;
		CustomWidget::Vector3Widget *		_positionInfoWidget	 = nullptr;
		InspectorSectionVLayout *			_infoSection		 = nullptr;
		CustomWidget::QLineEditMultiField * _nameWidget			 = nullptr;
		QPushButton *						_setAutoNameWidget	 = nullptr;
		CustomWidget::QLabelMultiField *	_distanceLabelWidget = nullptr;
		QPushButton *						_goToFirstAtom		 = nullptr;
		QPushButton *						_goToSecondAtom		 = nullptr;

		void _resetFieldStates( const SectionFlag & p_flag );

		void _setAutoNameAction() const;
		void _renameAction() const;
		void _orientOnFirstAtom() const;
		void _orientOnSecondAtom() const;
	};
} // namespace VTX::UI::Widget::Inspector

#endif
