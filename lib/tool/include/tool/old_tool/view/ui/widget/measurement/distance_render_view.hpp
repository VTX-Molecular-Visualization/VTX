#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_RENDER__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_RENDER__

#include "tool/old_tool/model/measurement/distance.hpp"
#include <QBrush>
#include <QFont>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPen>
#include <QPoint>
#include <QSize>
#include <app/old_app/id.hpp>
#include <app/old_app/view/base_view.hpp>
#include <string>
#include <ui/old_ui/ui/widget/base_manual_widget.hpp>
#include <ui/old_ui/ui/widget/render/base_integrated_widget.hpp>
#include <util/types.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	class DistanceRenderView :
		public View::BaseView<VTX::Model::Measurement::Distance>,
		public VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>
	{
		VTX_WIDGET
		VTX_VIEW

	  private:
		class PaintData
		{
		  public:
			Vec3f firstAtomScreenPos;
			Vec3f secondAtomScreenPos;

			int lineSize;
			int pointRadius;

			QSize textSize;
			float textScale;
			Vec3f textPosition;
			float textDistanceToCamera;
		};

	  public:
		void localize() override;
		void updatePosition() override;

	  protected:
		DistanceRenderView( Model::Measurement::Distance * const p_model, QWidget * const p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshView() override;

		void paintEvent( QPaintEvent * event ) override;

		void _setText( const std::string & p_txt );
		void _refreshText();
		void _refreshColor();

	  private:
		PaintData	 _paintData;
		QPainterPath _painterPath = QPainterPath();

		QPen   _labelPen;
		QBrush _labelBrush;
		QPen   _linePen;
		QBrush _lineBrush;
		QPen   _underlinePen;
		QBrush _underlineBrush;
		QPen   _pointPen;
		QBrush _pointBrush;
	};

} // namespace VTX::View::UI::Widget::Measurement
#endif
