#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_RENDER__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_RENDER__

#include "id.hpp"
#include "model/measurement/distance.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/render/base_integrated_widget.hpp"
#include "view/base_view.hpp"
#include <QLabel>
#include <QPaintEvent>
//#include <QStaticText>
#include <string>

namespace VTX::View::UI::Widget::Measurement
{
	class DistanceRenderView :
		public View::BaseView<VTX::Model::Measurement::Distance>,
		public VTX::UI::Widget::BaseManualWidget<QLabel>,
		public VTX::UI::Widget::Render::BaseIntegratedWidget
	{
		VTX_WIDGET
		VTX_VIEW

	  public:
		void localize() override;
		void updatePosition() override;

	  protected:
		DistanceRenderView( Model::Measurement::Distance * const p_model, QWidget * const p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		// void paintEvent( QPaintEvent * event ) override;

		void _refreshText();

	  private:
		// QStaticText _distanceTxt;
		Vec3f _worldPosition = VEC3F_ZERO;
	};

} // namespace VTX::View::UI::Widget::Measurement
#endif
