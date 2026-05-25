#ifndef __VTX_UI_QT_WIDGET_CAMERA__
#define __VTX_UI_QT_WIDGET_CAMERA__

#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/transform.hpp"
#include <QGroupBox>
#include <app/ecs.hpp>

namespace VTX::UI::QT::Widget
{
	class Camera : public QGroupBox
	{
	  public:
		Camera( const App::Entity, QWidget * );
		~Camera();

	  private:
		const App::Entity _entity;

		QPointer<Transform>		 _transform;
		QPointer<EditableSlider> _sliderFov;
		QPointer<EditableSlider> _sliderNear;
		QPointer<EditableSlider> _sliderFar;

		void _setTransform();
		void _setCamera();
	};
} // namespace VTX::UI::QT::Widget
#endif
