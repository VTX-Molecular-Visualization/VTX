#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_MEASURE_IN_PROGRESS_RENDER__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_MEASURE_IN_PROGRESS_RENDER__

#include "tool/old_tool/model/measurement/measure_in_progress.hpp"
#include <QBrush>
#include <QFont>
#include <QPaintEvent>
#include <QPen>
#include <QPoint>
#include <QSize>
#include <app/id.hpp>
#include <app/core/view/base_view.hpp>
#include <string>
#include <ui/old_ui/ui/widget/base_manual_widget.hpp>
#include <ui/old_ui/ui/widget/render/base_integrated_widget.hpp>
#include <util/types.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	class MeasureInProgressRenderView :
		public App::Core::View::BaseView<VTX::Model::Measurement::MeasureInProgress>,
		public VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>
	{
		VTX_WIDGET
		VTX_VIEW

	  private:
		class PaintData
		{
		  public:
			std::vector<Vec3f> points;

			float distanceToCamera;

			int lineSize;
			int pointRadius;
		};

	  public:
		void localize() override;
		void updatePosition() override;

	  protected:
		MeasureInProgressRenderView( Model::Measurement::MeasureInProgress * const p_model, QWidget * const p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshView() override;

		void paintEvent( QPaintEvent * event ) override;

	  private:
		PaintData _paintData;

		QPen   _linePen;
		QBrush _lineBrush;
	};

} // namespace VTX::View::UI::Widget::Measurement
#endif
