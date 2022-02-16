#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_ANGLE_RENDER__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_ANGLE_RENDER__

#include "define.hpp"
#include "id.hpp"
#include "model/measurement/angle.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/render/base_integrated_widget.hpp"
#include "view/base_view.hpp"
#include <QBrush>
#include <QFont>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPen>
#include <QPoint>
#include <QSize>
#include <string>

namespace VTX::View::UI::Widget::Measurement
{
	class AngleRenderView :
		public View::BaseView<VTX::Model::Measurement::Angle>,
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
			Vec3f thirdAtomScreenPos;

			int	  pointRadius;
			float lineSize;

			bool   drawText;
			QPoint textPosition;
			QSize  textSize;
			float  textScale;
			float  textDistanceToCamera;

			int arcRadius;
			int startAngle;
			int angle;
		};

	  public:
		void localize() override;
		void updatePosition() override;

	  protected:
		AngleRenderView( Model::Measurement::Angle * const p_model, QWidget * const p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshView() override;

		void paintEvent( QPaintEvent * event ) override;

		void _setText( const std::string & p_txt );
		void _refreshText();

	  private:
		PaintData	 _paintData;
		QPainterPath _painterPath = QPainterPath();

		QPen   _labelPen;
		QBrush _labelBrush;
		QPen   _linePen;
		QBrush _lineBrush;
		QPen   _arcPen;
		QBrush _arcBrush;
	};

} // namespace VTX::View::UI::Widget::Measurement
#endif
