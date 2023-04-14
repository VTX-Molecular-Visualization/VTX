#include "measure_in_progress_render_view.hpp"
#include "old_tool/util/measurement.hpp"
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPoint>
#include <QVBoxLayout>
#include <algorithm>
#include <app/old_app/model/atom.hpp>
#include <string>
#include <ui/old_ui/style.hpp>
#include <ui/old_ui/ui/main_window.hpp>
#include <ui/old_ui/util/ui_render.hpp>
#include <ui/old_ui/vtx_app.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	MeasureInProgressRenderView::MeasureInProgressRenderView( Model::Measurement::MeasureInProgress * const p_model,
															  QWidget * const								p_parent ) :
		View::BaseView<Model::Measurement::MeasureInProgress>( p_model ),
		VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>( p_parent )
	{
		_linePen = QPen();
		_linePen.setColor( VTX::UI::Style::MEASUREMENT_ANGLE_LINE_COLOR );
		_linePen.setStyle( VTX::UI::Style::MEASUREMENT_ANGLE_LINE_STYLE );
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

		const Vec3f centerWorldPos	= Util::UIRender::getCenter( atomPositions );
		_paintData.distanceToCamera = Util::UIRender::distanceToCamera( camera, centerWorldPos );

		// TODO Sigmoid
		const float ratioScale
			= 1.f
			  - std::clamp( ( _paintData.distanceToCamera - VTX::UI::Style::WORLD_LABEL_NEAR_CLIP )
								/ ( VTX::UI::Style::WORLD_LABEL_FAR_CLIP - VTX::UI::Style::WORLD_LABEL_NEAR_CLIP ),
							0.f,
							1.f );
		_paintData.lineSize = VTX::UI::Style::MEASUREMENT_ANGLE_LABEL_MIN_LINE_THICKNESS
							  + ( VTX::UI::Style::MEASUREMENT_ANGLE_LABEL_MAX_LINE_THICKNESS
								  - VTX::UI::Style::MEASUREMENT_ANGLE_LABEL_MIN_LINE_THICKNESS )
									* ratioScale;
		_paintData.pointRadius = ( _paintData.lineSize / 2 ) + VTX::UI::Style::LABEL_RENDER_POINT_RADIUS;

		const QRect		   renderRect	  = parentWidget()->rect();
		std::vector<Vec3f> vec3fPositions = std::vector<Vec3f>();
		Util::UIRender::fillScreenPositions( atomPositions, vec3fPositions, camera, renderRect );

		switch ( _model->getPotentialNextTargetType() )
		{
		case Model::Measurement::MeasureInProgress::PotentialTargetType::POSITION:
			vec3fPositions.emplace_back(
				Util::UIRender::qPointToVec3f( parentWidget()->mapFromGlobal( _model->getPotentialNextPosition() ) ) );
			break;
		}

		std::vector<QPoint> qPointPositions = std::vector<QPoint>();
		qPointPositions.reserve( vec3fPositions.size() );
		for ( const Vec3f & vec : vec3fPositions )
			qPointPositions.emplace_back( Util::UIRender::vec3fToQPoint( vec ) );

		int minX, maxX, minY, maxY;
		Util::UIRender::getMinMax( qPointPositions, minX, maxX, minY, maxY );
		const int pointDiameter = _paintData.pointRadius * 2;
		minX -= pointDiameter;
		maxX += pointDiameter;
		minY -= pointDiameter;
		maxY += pointDiameter;

		const QPoint topLeftPos = QPoint( minX, minY );
		move( topLeftPos );

		const QSize size = QSize( maxX - minX, maxY - minY );
		setFixedSize( size );

		_paintData.points = vec3fPositions;

		repaint();
	}

	void MeasureInProgressRenderView::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		if ( _paintData.points.size() <= 0 )
			return;

		QPainter painter( this );
		painter.save();

		// Draw line //////////////////////////////////////////////////////
		_linePen.setWidth( _paintData.lineSize );

		painter.setPen( _linePen );
		painter.setBrush( _lineBrush );
		painter.setRenderHint( QPainter::RenderHint::Antialiasing );

		const Vec3f vec3fThisPos		= Util::UIRender::qPointToVec3f( pos() );
		Vec3f		previousRelativePos = _paintData.points[ 0 ] - vec3fThisPos;
		painter.drawEllipse(
			Util::UIRender::vec3fToQPoint( previousRelativePos ), _paintData.pointRadius, _paintData.pointRadius );

		for ( int i = 1; i < _paintData.points.size(); i++ )
		{
			const Vec3f relativePos = _paintData.points[ i ] - vec3fThisPos;

			if ( relativePos.z >= 0 )
				painter.drawEllipse(
					Util::UIRender::vec3fToQPoint( relativePos ), _paintData.pointRadius, _paintData.pointRadius );

			painter.drawLine( Util::UIRender::getScreenLine( previousRelativePos, relativePos ) );

			previousRelativePos = relativePos;
		}

		painter.restore();
	}
} // namespace VTX::View::UI::Widget::Measurement
