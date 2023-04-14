#include "angle_render_view.hpp"
#include "old_tool/util/measurement.hpp"
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPoint>
#include <QVBoxLayout>
#include <algorithm>
#include <app/old_app/model/atom.hpp>
#include <app/old_app/vtx_app.hpp>
#include <string>
#include <ui/old_ui/style.hpp>
#include <ui/old_ui/ui/main_window.hpp>
#include <ui/old_ui/util/ui.hpp>
#include <ui/old_ui/util/ui_render.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	AngleRenderView::AngleRenderView( Model::Measurement::Angle * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Measurement::Angle>( p_model ),
		VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>( p_parent )
	{
		_labelPen	= QPen( Style::WORLD_LABEL_OUTLINE_COLOR );
		_labelBrush = QBrush( Style::WORLD_LABEL_FILL_COLOR );
		_labelBrush.setStyle( Qt::BrushStyle::SolidPattern );

		_linePen = QPen();
		_linePen.setStyle( Style::MEASUREMENT_ANGLE_LINE_STYLE );
		_lineBrush = QBrush( Qt::BrushStyle::NoBrush );

		_arcPen = QPen();
		_arcPen.setStyle( Style::MEASUREMENT_ANGLE_ARC_STYLE );
		_arcBrush = QBrush( Qt::BrushStyle::NoBrush );
	}

	void AngleRenderView::_setupUi( const QString & p_name )
	{
		VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>::_setupUi( p_name );

		_refreshText();
		_refreshColor();
		updatePosition();
		setVisible( true );
	}
	void AngleRenderView::_setupSlots() {}
	void AngleRenderView::localize() {}

	void AngleRenderView::_refreshView()
	{
		_refreshText();
		_refreshColor();
		// updatePosition called because it resizing the widget. Maybe resize can be done in a specific function
		updatePosition();
		repaint();
	}

	void AngleRenderView::updatePosition()
	{
		if ( !_model->isValid() || !_model->isEnable() )
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

		const Vec3f centerWorldPos		= Util::UIRender::getCenter( atomPositions );
		_paintData.textDistanceToCamera = Util::UIRender::distanceToCamera( camera, centerWorldPos );

		const float ratioScale = 1.f
								 - std::clamp( ( _paintData.textDistanceToCamera - Style::WORLD_LABEL_NEAR_CLIP )
												   / ( Style::WORLD_LABEL_FAR_CLIP - Style::WORLD_LABEL_NEAR_CLIP ),
											   0.f,
											   1.f );

		if ( ratioScale == 0.f )
			return;

		const QRect		   renderRect	  = parentWidget()->rect();
		std::vector<Vec3f> vec3fPositions = std::vector<Vec3f>();
		Util::UIRender::fillScreenPositions( atomPositions, vec3fPositions, camera, renderRect );

		std::vector<QPoint> qPointPositions = std::vector<QPoint>();
		qPointPositions.reserve( vec3fPositions.size() );
		for ( const Vec3f & vec : vec3fPositions )
			qPointPositions.emplace_back( Util::UIRender::vec3fToQPoint( vec ) );

		_paintData.lineSize = Style::MEASUREMENT_ANGLE_LABEL_MIN_LINE_THICKNESS
							  + ( Style::MEASUREMENT_ANGLE_LABEL_MAX_LINE_THICKNESS
								  - Style::MEASUREMENT_ANGLE_LABEL_MIN_LINE_THICKNESS )
									* ratioScale;

		_paintData.pointRadius = ( _paintData.lineSize / 2 ) + Style::LABEL_RENDER_POINT_RADIUS;

		int minX, maxX, minY, maxY;
		Util::UIRender::getMinMax( qPointPositions, minX, maxX, minY, maxY );
		minX -= _paintData.pointRadius;
		maxX += _paintData.pointRadius;
		minY -= _paintData.pointRadius;
		maxY += _paintData.pointRadius;

		const bool displayArc = vec3fPositions[ 1 ].z >= 0;

		if ( displayArc )
		{
			const QPoint anglePoint = Util::UIRender::vec3fToQPoint( vec3fPositions[ 1 ] );

			const QVector2D vec1 = Util::UIRender::vec3fToQVector2D( vec3fPositions[ 0 ] - vec3fPositions[ 1 ] );
			const QVector2D vec2 = Util::UIRender::vec3fToQVector2D( vec3fPositions[ 2 ] - vec3fPositions[ 1 ] );

			const int		firstLineLength	 = vec1.length();
			const int		secondLineLength = vec2.length();
			const QVector2D screenBisector	 = ( vec1.normalized() + vec2.normalized() ).normalized();

			const int arcRadius = std::min( { int( firstLineLength * 0.8f ),
											  int( secondLineLength * 0.8f ),
											  Style::MEASUREMENT_ANGLE_ARC_RADIUS } );

			const QPoint textPos
				= anglePoint
				  + QPoint( screenBisector.x() * ( arcRadius + Style::MEASUREMENT_ANGLE_LABEL_TEXT_OFFSET ),
							screenBisector.y() * ( arcRadius + Style::MEASUREMENT_ANGLE_LABEL_TEXT_OFFSET ) );

			const float textScale
				= Style::WORLD_LABEL_MIN_TEXT_SCALE
				  + ( Style::WORLD_LABEL_MAX_TEXT_SCALE - Style::WORLD_LABEL_MIN_TEXT_SCALE ) * ratioScale;

			const int minXText = textPos.x() - ( _paintData.textSize.width() * textScale ) / 2;
			const int maxXText = textPos.x() + ( _paintData.textSize.width() * textScale ) / 2;
			const int minYText = textPos.y() - ( _paintData.textSize.height() * textScale ) / 2;
			const int maxYText = textPos.y() + ( _paintData.textSize.height() * textScale ) / 2;

			const int minXAngle = anglePoint.x() - arcRadius;
			const int maxXAngle = anglePoint.x() + arcRadius;
			const int minYAngle = anglePoint.y() - arcRadius;
			const int maxYAngle = anglePoint.y() + arcRadius;

			minX = std::min( { minX, minXText, minXAngle } );
			maxX = std::max( { maxX, maxXText, maxXAngle } );
			minY = std::min( { minY, minYText, minYAngle } );
			maxY = std::max( { maxY, maxYText, maxYAngle } );

			_paintData.textScale	= textScale;
			_paintData.textPosition = textPos;
			_paintData.arcRadius	= arcRadius;
		}

		const QPoint topLeftPos = QPoint( minX, minY );
		move( topLeftPos );

		const QSize size = QSize( maxX - minX, maxY - minY );
		setFixedSize( size );

		_paintData.drawText = displayArc;

		_paintData.firstAtomScreenPos  = vec3fPositions[ 0 ];
		_paintData.secondAtomScreenPos = vec3fPositions[ 1 ];
		_paintData.thirdAtomScreenPos  = vec3fPositions[ 2 ];

		// Prevent perfect alignement of two points which can cause issue during normalization
		const bool canDrawArc = vec3fPositions[ 0 ] != vec3fPositions[ 1 ] && vec3fPositions[ 1 ] != vec3fPositions[ 2 ]
								&& vec3fPositions[ 2 ] != vec3fPositions[ 0 ];

		if ( displayArc && canDrawArc )
		{
			const Vec2f left = Vec2f( 1, 0 );
			const Vec2f vec1 = Util::Math::normalize(
				Vec2f( vec3fPositions[ 0 ].x - vec3fPositions[ 1 ].x, vec3fPositions[ 0 ].y - vec3fPositions[ 1 ].y ) );
			const Vec2f vec2 = Util::Math::normalize(
				Vec2f( vec3fPositions[ 2 ].x - vec3fPositions[ 1 ].x, vec3fPositions[ 2 ].y - vec3fPositions[ 1 ].y ) );

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

			_paintData.angle	  = angle;
			_paintData.startAngle = startAngle;
		}
		else
		{
			_paintData.arcRadius  = 0;
			_paintData.angle	  = 0;
			_paintData.startAngle = 0;
		}

		repaint();
	}

	void AngleRenderView::_refreshText() { _setText( Util::Measurement::getAngleString( *_model ) ); }
	void AngleRenderView::_refreshColor()
	{
		const QColor lineColor = Util::UI::RgbToQColor( _model->getColor() );
		_linePen.setColor( lineColor );
		_lineBrush.setColor( lineColor );
		_arcPen.setColor( lineColor );
	}

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
			_arcPen.setWidth( _paintData.lineSize );

			painter.setPen( _linePen );
			painter.setBrush( _lineBrush );
			painter.setRenderHint( QPainter::RenderHint::Antialiasing );

			const Vec3f vec3fPos = Vec3f( pos().x(), pos().y(), 0 );

			const Vec3f relativeFirstAtomPos  = _paintData.firstAtomScreenPos - vec3fPos;
			const Vec3f relativeSecondAtomPos = _paintData.secondAtomScreenPos - vec3fPos;
			const Vec3f relativeThirdAtomPos  = _paintData.thirdAtomScreenPos - vec3fPos;

			if ( _paintData.firstAtomScreenPos.z >= 0 )
				painter.drawEllipse( Util::UIRender::vec3fToQPoint( relativeFirstAtomPos ),
									 _paintData.pointRadius,
									 _paintData.pointRadius );
			if ( _paintData.secondAtomScreenPos.z >= 0 )
				painter.drawEllipse( Util::UIRender::vec3fToQPoint( relativeSecondAtomPos ),
									 _paintData.pointRadius,
									 _paintData.pointRadius );
			if ( _paintData.thirdAtomScreenPos.z >= 0 )
				painter.drawEllipse( Util::UIRender::vec3fToQPoint( relativeThirdAtomPos ),
									 _paintData.pointRadius,
									 _paintData.pointRadius );

			painter.drawLine( Util::UIRender::getScreenLine( relativeSecondAtomPos, relativeFirstAtomPos ) );
			painter.drawLine( Util::UIRender::getScreenLine( relativeSecondAtomPos, relativeThirdAtomPos ) );

			painter.setPen( _arcPen );
			painter.setBrush( _arcBrush );

			// Don't draw arc when angle == 0
			if ( _paintData.angle > 0 )
			{
				QPoint secondAtomPos = Util::UIRender::vec3fToQPoint( relativeSecondAtomPos );
				painter.drawArc( QRect( secondAtomPos.x() - _paintData.arcRadius,
										secondAtomPos.y() - _paintData.arcRadius,
										_paintData.arcRadius * 2,
										_paintData.arcRadius * 2 ),
								 _paintData.startAngle * 16,
								 _paintData.angle * 16 );
			}
			///////////////////////////////////////////////////////////////////

			// Draw text //////////////////////////////////////////////////////
			if ( _paintData.drawText )
			{
				painter.setBrush( _labelBrush );
				painter.setPen( _labelPen );

				const QPointF textPosition
					= ( _paintData.textPosition - pos() )
					  - ( QPoint( _paintData.textSize.width() / 2, -_paintData.textSize.height() / 2 )
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
