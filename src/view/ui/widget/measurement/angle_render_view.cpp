#include "angle_render_view.hpp"
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
	AngleRenderView::AngleRenderView( Model::Measurement::Angle * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Measurement::Angle>( p_model ), VTX::UI::Widget::BaseManualWidget<QWidget>( p_parent )
	{
		_labelPen	= QPen( Style::WORLD_LABEL_OUTLINE_COLOR );
		_labelBrush = QBrush( Style::WORLD_LABEL_FILL_COLOR );
		_labelBrush.setStyle( Qt::BrushStyle::SolidPattern );

		_linePen = QPen();
		_linePen.setColor( Style::MEASUREMENT_ANGLE_LINE_COLOR );
		_linePen.setStyle( Style::MEASUREMENT_ANGLE_LINE_STYLE );
		_lineBrush = QBrush( Qt::BrushStyle::NoBrush );

		_arcPen = QPen();
		_arcPen.setColor( Style::MEASUREMENT_ANGLE_ARC_COLOR );
		_arcPen.setStyle( Style::MEASUREMENT_ANGLE_ARC_STYLE );
		_arcBrush = QBrush( Qt::BrushStyle::NoBrush );
	}

	void AngleRenderView::_setupUi( const QString & p_name )
	{
		BaseManualWidget ::_setupUi( p_name );

		_refreshText();
		updatePosition();
		setVisible( true );
	}
	void AngleRenderView::_setupSlots() {}
	void AngleRenderView::localize() {}

	void AngleRenderView::_refreshView()
	{
		_refreshText();
		// updatePosition called because it resizing the widget. Maybe resize can be done in a specific function
		updatePosition();
		repaint();
	}

	void AngleRenderView::updatePosition()
	{
		if ( !_model->isValid() )
			return;

		const std::vector<const Model::Atom *> & atoms		   = _model->getAtoms();
		std::vector<Vec3f>						 atomPositions = std::vector<Vec3f>();
		atomPositions.reserve( atoms.size() );
		for ( const Model::Atom * const atom : atoms )
			atomPositions.emplace_back( atom->getWorldPosition() );

		const Object3D::Camera & camera = VTXApp::get().getScene().getCamera();

		const bool visible = Util::UIRender::anyVisibleToCamera( camera, atomPositions );
		setVisible( visible );

		if ( !visible )
			return;

		const Vec3f centerWorldPos = Util::UIRender::getCenter( atomPositions );

		const QRect renderRect = parentWidget()->rect();

		std::vector<QPoint> pointPositions = std::vector<QPoint>();
		Util::UIRender::fillScreenPositions( atomPositions, pointPositions, camera, renderRect );

		const int firstLineLength  = QVector2D( pointPositions[ 0 ] - pointPositions[ 1 ] ).length();
		const int secondLineLength = QVector2D( pointPositions[ 2 ] - pointPositions[ 1 ] ).length();

		const int arcRadius = std::min(
			{ int( firstLineLength * 0.8f ), int( secondLineLength * 0.8f ), Style::MEASUREMENT_ANGLE_ARC_RADIUS } );

		const QPoint	centerPos	   = Util::UIRender::worldToScreen( centerWorldPos, camera, renderRect );
		const QVector2D screenBisector = ( QVector2D( pointPositions[ 0 ] - pointPositions[ 1 ] ).normalized()
										   + QVector2D( pointPositions[ 2 ] - pointPositions[ 1 ] ).normalized() )
											 .normalized();
		const QPoint textPos
			= pointPositions[ 1 ]
			  + QPoint( screenBisector.x() * ( arcRadius + Style::MEASUREMENT_ANGLE_LABEL_TEXT_OFFSET ),
						screenBisector.y() * ( arcRadius + Style::MEASUREMENT_ANGLE_LABEL_TEXT_OFFSET ) );

		int minX, maxX, minY, maxY;
		Util::UIRender::getMinMax( pointPositions, minX, maxX, minY, maxY );
		minX -= Style::LABEL_RENDER_POINT_MAX_DIAMETER;
		maxX += Style::LABEL_RENDER_POINT_MAX_DIAMETER;
		minY -= Style::LABEL_RENDER_POINT_MAX_DIAMETER;
		maxY += Style::LABEL_RENDER_POINT_MAX_DIAMETER;

		const int minXText = textPos.x() - _paintData.textSize.width() / 2;
		const int maxXText = textPos.x() + _paintData.textSize.width() / 2;
		const int minYText = textPos.y() - _paintData.textSize.height() / 2;
		const int maxYText = textPos.y() + _paintData.textSize.height() / 2;

		const int minXAngle = pointPositions[ 1 ].x() - arcRadius;
		const int maxXAngle = pointPositions[ 1 ].x() + arcRadius;
		const int minYAngle = pointPositions[ 1 ].y() - arcRadius;
		const int maxYAngle = pointPositions[ 1 ].y() + arcRadius;

		minX = std::min( { minX, minXText, minXAngle } );
		maxX = std::max( { maxX, maxXText, maxXAngle } );
		minY = std::min( { minY, minYText, minYAngle } );
		maxY = std::max( { maxY, maxYText, maxYAngle } );

		const QPoint topLeftPos = QPoint( minX, minY );
		move( topLeftPos );

		const QSize size = QSize( maxX - minX, maxY - minY );
		setFixedSize( size );

		const Vec2f left = Vec2f( 1, 0 );
		const Vec2f vec1 = Util::Math::normalize( Vec2f( pointPositions[ 0 ].x() - pointPositions[ 1 ].x(),
														 pointPositions[ 0 ].y() - pointPositions[ 1 ].y() ) );
		const Vec2f vec2 = Util::Math::normalize( Vec2f( pointPositions[ 2 ].x() - pointPositions[ 1 ].x(),
														 pointPositions[ 2 ].y() - pointPositions[ 1 ].y() ) );

		const int angle = Util::Math::degrees( Util::Math::angle( vec1, vec2 ) );

		const int startAngle1		  = Util::Math::degrees( Util::Math::angle( left, vec1 ) );
		const int startAngle2		  = Util::Math::degrees( Util::Math::angle( left, vec2 ) );
		const int orientedStartAngle1 = vec1.y < 0 ? startAngle1 : 360 - startAngle1;
		const int orientedStartAngle2 = vec2.y < 0 ? startAngle2 : 360 - startAngle2;

		const int orientedAngle = orientedStartAngle2 > orientedStartAngle1
									  ? ( orientedStartAngle2 - orientedStartAngle1 )
									  : ( orientedStartAngle1 - orientedStartAngle2 );

		int startAngle;
		if ( orientedAngle < 180 )
			startAngle = orientedStartAngle1 < orientedStartAngle2 ? orientedStartAngle1 : orientedStartAngle2;
		else
			startAngle = orientedStartAngle1 > orientedStartAngle2 ? orientedStartAngle1 : orientedStartAngle2;

		_paintData.firstAtomScreenPos	= pointPositions[ 0 ];
		_paintData.secondAtomScreenPos	= pointPositions[ 1 ];
		_paintData.thirdAtomScreenPos	= pointPositions[ 2 ];
		_paintData.textPosition			= textPos;
		_paintData.textDistanceToCamera = Util::UIRender::distanceToCamera( camera, centerWorldPos );

		_paintData.arcRadius  = arcRadius;
		_paintData.angle	  = angle;
		_paintData.startAngle = startAngle;

		repaint();
	}

	void AngleRenderView::_refreshText() { _setText( Util::Measurement::getAngleString( *_model ) ); }

	void AngleRenderView::_setText( const std::string & p_txt )
	{
		const QString qstr = QString::fromStdString( p_txt );

		const QFontMetrics labelFontMetric = QFontMetrics( Style::WORLD_LABEL_FONT );

		_paintData.textSize = QSize( labelFontMetric.horizontalAdvance( qstr ), labelFontMetric.height() );

		_painterPath.clear();
		_painterPath.addText( { 0, 0 }, Style::WORLD_LABEL_FONT, qstr );
	}

	void AngleRenderView::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		const float minDist = 20.f;
		const float maxDist = 150.0f;

		const float ratioScale = 1.f
								 - std::clamp( ( _paintData.textDistanceToCamera - Style::WORLD_LABEL_NEAR_CLIP )
												   / ( Style::WORLD_LABEL_FAR_CLIP - Style::WORLD_LABEL_NEAR_CLIP ),
											   0.f,
											   1.f );

		if ( ratioScale > 0.f )
		{
			QPainter painter( this );
			painter.save();
			// painter.setWorldMatrixEnabled( false );

			// Draw line //////////////////////////////////////////////////////
			// TODO Sigmoid
			const int lineSize = Style::MEASUREMENT_ANGLE_LABEL_MIN_LINE_THICKNESS
								 + ( Style::MEASUREMENT_ANGLE_LABEL_MAX_LINE_THICKNESS
									 - Style::MEASUREMENT_ANGLE_LABEL_MIN_LINE_THICKNESS )
									   * ratioScale;

			_linePen.setWidth( lineSize );
			_arcPen.setWidth( lineSize );

			painter.setPen( _linePen );
			painter.setBrush( _lineBrush );
			painter.setRenderHint( QPainter::RenderHint::Antialiasing );

			const QPoint firstAtomPos  = _paintData.firstAtomScreenPos - pos();
			const QPoint secondAtomPos = _paintData.secondAtomScreenPos - pos();
			const QPoint thirdAtomPos  = _paintData.thirdAtomScreenPos - pos();
			const int	 pointRadius   = ( lineSize / 2 ) + Style::LABEL_RENDER_POINT_RADIUS;

			painter.drawEllipse( firstAtomPos, pointRadius, pointRadius );
			painter.drawEllipse( secondAtomPos, pointRadius, pointRadius );
			painter.drawEllipse( thirdAtomPos, pointRadius, pointRadius );
			painter.drawLine( secondAtomPos, firstAtomPos );
			painter.drawLine( secondAtomPos, thirdAtomPos );

			painter.setPen( _arcPen );
			painter.setBrush( _arcBrush );

			int firstLineLength	 = QVector2D( firstAtomPos - secondAtomPos ).length();
			int secondLineLength = QVector2D( thirdAtomPos - secondAtomPos ).length();

			painter.drawArc( QRect( secondAtomPos.x() - _paintData.arcRadius,
									secondAtomPos.y() - _paintData.arcRadius,
									_paintData.arcRadius * 2,
									_paintData.arcRadius * 2 ),
							 _paintData.startAngle * 16,
							 _paintData.angle * 16 );
			///////////////////////////////////////////////////////////////////

			// Draw text //////////////////////////////////////////////////////
			painter.setBrush( _labelBrush );
			painter.setPen( _labelPen );

			const float textScale
				= Style::WORLD_LABEL_MIN_TEXT_SCALE
				  + ( Style::WORLD_LABEL_MAX_TEXT_SCALE - Style::WORLD_LABEL_MIN_TEXT_SCALE ) * ratioScale;

			const QPointF textPosition
				= ( _paintData.textPosition - pos() )
				  - ( QPoint( _paintData.textSize.width() / 2, -_paintData.textSize.height() / 2 ) * textScale );

			painter.translate( textPosition );

			painter.scale( textScale, textScale );
			painter.drawPath( _painterPath );
			painter.resetTransform();
			///////////////////////////////////////////////////////////////////

			// painter.setWorldMatrixEnabled( true );
			painter.restore();
		}
	}
} // namespace VTX::View::UI::Widget::Measurement
