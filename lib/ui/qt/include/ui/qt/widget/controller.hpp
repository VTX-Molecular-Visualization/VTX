#ifndef __VTX_UI_QT_WIDGET_CONTROLLER__
#define __VTX_UI_QT_WIDGET_CONTROLLER__

#include "ui/qt/widget/editable_slider.hpp"
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QPointer>
#include <app/ecs.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Display camera controller settings.
	 */
	class Controller : public QGroupBox
	{
	  public:
		Controller( const Entity, QWidget * );
		~Controller();

	  private:
		const Entity _entity;

		QPointer<QCheckBox>		 _checkBoxInvertY;
		QPointer<QCheckBox>		 _checkBoxElasticityActive;
		QPointer<QLabel>		 _labelElasticityFactor;
		QPointer<EditableSlider> _sliderTranslationSpeed;
		QPointer<EditableSlider> _sliderRotationSpeed;
		QPointer<EditableSlider> _sliderAccelerationFactor;
		QPointer<EditableSlider> _sliderDecelerationFactor;
		QPointer<EditableSlider> _sliderElasticityFactor;

		void _setController();
	};
} // namespace VTX::UI::QT::Widget
#endif
