#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_RENDER__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_RENDER__

#include "define.hpp"
#include "id.hpp"
#include "model/measurement/distance.hpp"
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
			QPoint firstAtomScreenPos;
			QPoint secondAtomScreenPos;
			QPoint textPosition;
			QSize  textSize;
			float  textDistanceToCamera;
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

	  private:
		PaintData	 _paintData;
		QPainterPath _painterPath = QPainterPath();

		QPen   _labelPen;
		QBrush _labelBrush;
		QPen   _linePen;
		QBrush _lineBrush;

		QPoint _worldToScreen( const Vec3f & p_worldPos ) const;
		float  _distanceToCamera( const Vec3f & p_worldPos ) const;
		bool   _isVisibleToCamera( const Vec3f & p_worldPos ) const;
	};

} // namespace VTX::View::UI::Widget::Measurement
#endif
