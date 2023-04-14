#include "distance_render_view.hpp"
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
#include <ui/old_ui/util/ui.hpp>
#include <ui/old_ui/util/ui_render.hpp>
#include <ui/old_ui/vtx_app.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	DistanceRenderView::DistanceRenderView( Model::Measurement::Distance * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Measurement::Distance>( p_model ),
		VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>( p_parent )
	{
		_labelPen	= QPen( Style::WORLD_LABEL_OUTLINE_COLOR );
		_labelBrush = QBrush( Style::WORLD_LABEL_FILL_COLOR );
		_labelBrush.setStyle( Qt::BrushStyle::SolidPattern );

		_linePen = QPen();
		_linePen.setStyle( Style::MEASUREMENT_DISTANCE_LABEL_LINE_STYLE );
		_lineBrush = QBrush( Qt::BrushStyle::SolidPattern );
		//_lineBrush = QBrush( Qt::BrushStyle::SolidPattern );
	}

	void DistanceRenderView::_setupUi( const QString & p_name )
	{
		VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>::_setupUi( p_name );

		_refreshText();
		_refreshColor();
		updatePosition();
		setVisible( true );
	}
	void DistanceRenderView::_setupSlots() {}
	void DistanceRenderView::localize() {}

	void DistanceRenderView::_refreshView()
	{
		_refreshText();
		_refreshColor();

		// updatePosition called because it resizing the widget. Maybe resize can be done in a specific function
		updatePosition();
		repaint();
	}

	void DistanceRenderView::updatePosition()
	{
		if ( !_model->isValid() || !_model->isEnable() )
			return;

		const Vec3f & firstAtomWorldPos	 = _model->getFirstAtom().getWorldPosition();
		const Vec3f & secondAtomWorldPos = _model->getSecondAtom().getWorldPosition();

		const Object3D::Camera & camera = VTXApp::get().getScene().getCamera();

		const bool visible = Util::UIRender::isVisibleToCamera( camera, firstAtomWorldPos )
							 || Util::UIRender::isVisibleToCamera( camera, secondAtomWorldPos );

		setVisible( visible );

		if ( !visible )
		{
			return;
		}

		const Vec3f centerWorldPos		= ( firstAtomWorldPos + secondAtomWorldPos ) * 0.5f;
		_paintData.textDistanceToCamera = Util::UIRender::distanceToCamera( camera, centerWorldPos );

		const float ratioScale = 1.f
								 - std::clamp( ( _paintData.textDistanceToCamera - Style::WORLD_LABEL_NEAR_CLIP )
												   / ( Style::WORLD_LABEL_FAR_CLIP - Style::WORLD_LABEL_NEAR_CLIP ),
											   0.f,
											   1.f );

		if ( ratioScale == 0.f )
		{
			repaint();
			return;
		}

		_paintData.lineSize = Style::MEASUREMENT_DISTANCE_LABEL_MIN_LINE_THICKNESS
							  + ( Style::MEASUREMENT_DISTANCE_LABEL_MAX_LINE_THICKNESS
								  - Style::MEASUREMENT_DISTANCE_LABEL_MIN_LINE_THICKNESS )
									* ratioScale;

		_paintData.pointRadius	= ( _paintData.lineSize / 2 ) + Style::MEASUREMENT_DISTANCE_LABEL_POINT_RADIUS + 2;
		const int pointDiameter = _paintData.pointRadius * 2;

		const QRect renderRect = parentWidget()->rect();

		const Vec3f firstAtomScreenVec3Pos
			= Util::UIRender::worldToScreenVec3f( firstAtomWorldPos, camera, renderRect );
		const Vec3f secondAtomScreenVec3Pos
			= Util::UIRender::worldToScreenVec3f( secondAtomWorldPos, camera, renderRect );

		const QPoint firstAtomScreenPointPos  = Util::UIRender::vec3fToQPoint( firstAtomScreenVec3Pos );
		const QPoint secondAtomScreenPointPos = Util::UIRender::vec3fToQPoint( secondAtomScreenVec3Pos );

		const Vec3f textScreenVec3fPos = Util::UIRender::worldToScreenVec3f( centerWorldPos, camera, renderRect );

		int minX
			= std::min( { firstAtomScreenPointPos.x() - pointDiameter, secondAtomScreenPointPos.x() - pointDiameter } );
		int minY
			= std::min( { firstAtomScreenPointPos.y() - pointDiameter, secondAtomScreenPointPos.y() - pointDiameter } );
		int maxX
			= std::max( { firstAtomScreenPointPos.x() + pointDiameter, secondAtomScreenPointPos.x() + pointDiameter } );
		int maxY
			= std::max( { firstAtomScreenPointPos.y() + pointDiameter, secondAtomScreenPointPos.y() + pointDiameter } );

		if ( textScreenVec3fPos.z > 0 )
		{
			const QPoint textScreenPointPos = Util::UIRender::vec3fToQPoint( textScreenVec3fPos );
			const float	 textScale
				= Style::WORLD_LABEL_MIN_TEXT_SCALE
				  + ( Style::WORLD_LABEL_MAX_TEXT_SCALE - Style::WORLD_LABEL_MIN_TEXT_SCALE ) * ratioScale;

			minX = std::min( { minX, ( textScreenPointPos.x() - _paintData.textSize.width() / 2 ) } );
			minY = std::min( { minY, ( textScreenPointPos.y() - _paintData.textSize.height() / 2 ) } );
			maxX = std::max( { maxX, ( textScreenPointPos.x() + ( _paintData.textSize.width() / 2 ) ) } );
			maxY = std::max( { maxY, ( textScreenPointPos.y() + ( _paintData.textSize.height() / 2 ) ) } );

			_paintData.textScale = textScale;
		}

		const QPoint topLeftPos = QPoint( minX, minY );
		move( topLeftPos );

		const QSize size = QSize( maxX - minX, maxY - minY );
		setFixedSize( size );

		_paintData.firstAtomScreenPos  = firstAtomScreenVec3Pos;
		_paintData.secondAtomScreenPos = secondAtomScreenVec3Pos;
		_paintData.textPosition		   = textScreenVec3fPos;

		repaint();
	}

	void DistanceRenderView::_refreshText() { _setText( Util::Measurement::getDistanceString( *_model ) ); }
	void DistanceRenderView::_refreshColor()
	{
		const QColor lineColor = Util::UI::RgbToQColor( _model->getColor() );
		_linePen.setColor( lineColor );
		_lineBrush.setColor( lineColor );
	}

	void DistanceRenderView::_setText( const std::string & p_txt )
	{
		const QString qstr = QString::fromStdString( p_txt );

		const QFontMetrics labelFontMetric = QFontMetrics( Style::WORLD_LABEL_FONT );

		_paintData.textSize = QSize( labelFontMetric.horizontalAdvance( qstr ), labelFontMetric.height() );

		_painterPath.clear();
		_painterPath.addText( { 0, 0 }, Style::WORLD_LABEL_FONT, qstr );
	}

	void DistanceRenderView::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		if ( !_model->isValid() || !_model->isEnable() )
			return;

		if ( _paintData.textDistanceToCamera < Style::WORLD_LABEL_FAR_CLIP )
		{
			QPainter painter( this );
			painter.save();
			// painter.setWorldMatrixEnabled( false );

			// Draw line //////////////////////////////////////////////////////
			// TODO Sigmoid
			_linePen.setWidth( _paintData.lineSize );

			painter.setPen( _linePen );
			painter.setBrush( _lineBrush );
			painter.setRenderHint( QPainter::RenderHint::Antialiasing );

			const Vec3f firstAtomPos  = _paintData.firstAtomScreenPos - Vec3f( pos().x(), pos().y(), 0 );
			const Vec3f secondAtomPos = _paintData.secondAtomScreenPos - Vec3f( pos().x(), pos().y(), 0 );
			const int	pointRadius	  = ( _paintData.lineSize / 2 ) + Style::MEASUREMENT_DISTANCE_LABEL_POINT_RADIUS;

			if ( firstAtomPos.z >= 0 )
			{
				painter.drawEllipse(
					Util::UIRender::vec3fToQPoint( firstAtomPos ), _paintData.pointRadius, _paintData.pointRadius );
			}
			if ( secondAtomPos.z >= 0 )
			{
				painter.drawEllipse(
					Util::UIRender::vec3fToQPoint( secondAtomPos ), _paintData.pointRadius, _paintData.pointRadius );
			}

			painter.drawLine( Util::UIRender::getScreenLine( firstAtomPos, secondAtomPos ) );
			///////////////////////////////////////////////////////////////////

			// Draw text //////////////////////////////////////////////////////
			if ( _paintData.textPosition.z >= 0 )
			{
				painter.setBrush( _labelBrush );
				painter.setPen( _labelPen );

				const QPoint textCenterPosition = Util::UIRender::vec3fToQPoint( _paintData.textPosition ) - pos();

				const QPointF textPosition
					= textCenterPosition
					  - ( QPoint( _paintData.textSize.width() / 2, -_paintData.textSize.height() / 4 )
						  * _paintData.textScale );

				painter.translate( textPosition );

				painter.scale( _paintData.textScale, _paintData.textScale );
				painter.drawPath( _painterPath );
				painter.resetTransform();
			}
			///////////////////////////////////////////////////////////////////

			// painter.setWorldMatrixEnabled( true );
			painter.restore();
		}
	}
} // namespace VTX::View::UI::Widget::Measurement
