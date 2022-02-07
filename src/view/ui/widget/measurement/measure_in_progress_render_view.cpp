#include "measure_in_progress_render_view.hpp"
#include "model/atom.hpp"
#include "style.hpp"
#include "ui/main_window.hpp"
#include "util/measurement.hpp"
#include "util/ui_render.hpp"
#include "vtx_app.hpp"
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPoint>
#include <QVBoxLayout>
#include <algorithm>
#include <string>

namespace VTX::View::UI::Widget::Measurement
{
	MeasureInProgressRenderView::MeasureInProgressRenderView( Model::Measurement::MeasureInProgress * const p_model,
															  QWidget * const								p_parent ) :
		View::BaseView<Model::Measurement::MeasureInProgress>( p_model ),
		VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>( p_parent )
	{
		_linePen = QPen();
		_linePen.setColor( Style::MEASUREMENT_ANGLE_LINE_COLOR );
		_linePen.setStyle( Style::MEASUREMENT_ANGLE_LINE_STYLE );
		_lineBrush = QBrush( Qt::BrushStyle::NoBrush );
	}

	void MeasureInProgressRenderView::_setupUi( const QString & p_name )
	{
		VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>::_setupUi( p_name );

		updatePosition();
		setVisible( true );
	}
	void MeasureInProgressRenderView::_setupSlots() {}
	void MeasureInProgressRenderView::localize() {}

	void MeasureInProgressRenderView::_refreshView()
	{
		// updatePosition called because it resizing the widget. Maybe resize can be done in a specific function
		updatePosition();
		repaint();
	}

	void MeasureInProgressRenderView::updatePosition()
	{
		if ( !_model->isValid() )
			return;

		const std::vector<const Model::Atom *> & atoms		   = _model->getAtoms();
		std::vector<Vec3f>						 atomPositions = std::vector<Vec3f>();
		atomPositions.reserve( atoms.size() );

		for ( const Model::Atom * const atom : atoms )
			atomPositions.emplace_back( atom->getWorldPosition() );

		switch ( _model->getPotentialNextTargetType() )
		{
		case Model::Measurement::MeasureInProgress::PotentialTargetType::ATOM:
			atomPositions.emplace_back( _model->getPotentialNextAtom()->getWorldPosition() );
			break;

		case Model::Measurement::MeasureInProgress::PotentialTargetType::ATOMPAIR:
			atomPositions.emplace_back( _model->getPotentialNextAtomPair().first->getWorldPosition() );
			atomPositions.emplace_back( _model->getPotentialNextAtomPair().second->getWorldPosition() );
			break;
		}

		const Object3D::Camera & camera = VTXApp::get().getScene().getCamera();

		const bool visible = Util::UIRender::anyVisibleToCamera( camera, atomPositions );

		if ( !visible )
		{
			_paintData.points.clear();
			return;
		}

		const Vec3f	 centerWorldPos = Util::UIRender::getCenter( atomPositions );
		const QRect	 renderRect		= parentWidget()->rect();
		const QPoint centerPos		= Util::UIRender::worldToScreen( centerWorldPos, camera, renderRect );

		std::vector<QPoint> pointPositions = std::vector<QPoint>();
		Util::UIRender::fillScreenPositions( atomPositions, pointPositions, camera, renderRect );

		switch ( _model->getPotentialNextTargetType() )
		{
		case Model::Measurement::MeasureInProgress::PotentialTargetType::POSITION:
			pointPositions.emplace_back( parentWidget()->mapFromGlobal( _model->getPotentialNextPosition() ) );
			break;
		}

		int minX, maxX, minY, maxY;
		Util::UIRender::getMinMax( pointPositions, minX, maxX, minY, maxY );
		minX -= Style::LABEL_RENDER_POINT_MAX_DIAMETER;
		maxX += Style::LABEL_RENDER_POINT_MAX_DIAMETER;
		minY -= Style::LABEL_RENDER_POINT_MAX_DIAMETER;
		maxY += Style::LABEL_RENDER_POINT_MAX_DIAMETER;

		const QPoint topLeftPos = QPoint( minX, minY );
		move( topLeftPos );

		const QSize size = QSize( maxX - minX, maxY - minY );
		setFixedSize( size );

		_paintData.points			= pointPositions;
		_paintData.distanceToCamera = Util::UIRender::distanceToCamera( camera, centerWorldPos );

		repaint();
	}

	void MeasureInProgressRenderView::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		if ( _paintData.points.size() <= 0 )
			return;

		const float minDist = 20.f;
		const float maxDist = 150.0f;

		const float ratioScale = 1.f
								 - std::clamp( ( _paintData.distanceToCamera - Style::WORLD_LABEL_NEAR_CLIP )
												   / ( Style::WORLD_LABEL_FAR_CLIP - Style::WORLD_LABEL_NEAR_CLIP ),
											   0.f,
											   1.f );

		if ( ratioScale > 0.f )
		{
			QPainter painter( this );
			painter.save();

			// Draw line //////////////////////////////////////////////////////
			// TODO Sigmoid
			const int lineSize = Style::MEASUREMENT_ANGLE_LABEL_MIN_LINE_THICKNESS
								 + ( Style::MEASUREMENT_ANGLE_LABEL_MAX_LINE_THICKNESS
									 - Style::MEASUREMENT_ANGLE_LABEL_MIN_LINE_THICKNESS )
									   * ratioScale;

			_linePen.setWidth( lineSize );

			painter.setPen( _linePen );
			painter.setBrush( _lineBrush );
			painter.setRenderHint( QPainter::RenderHint::Antialiasing );

			const int pointRadius = ( lineSize / 2 ) + Style::LABEL_RENDER_POINT_RADIUS;

			QPoint previousRelativePos = QPoint();
			for ( const QPoint point : _paintData.points )
			{
				const QPoint relativePos = point - pos();
				painter.drawEllipse( relativePos, pointRadius, pointRadius );

				if ( !previousRelativePos.isNull() )
				{
					painter.drawLine( previousRelativePos, relativePos );
				}

				previousRelativePos = relativePos;
			}

			painter.restore();
		}
	}
} // namespace VTX::View::UI::Widget::Measurement
