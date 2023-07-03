#include "tool/old_tool/view/ui/widget/measurement/distance_to_cycle_render_view.hpp"
#include "tool/old_tool/util/measurement.hpp"
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPoint>
#include <QVBoxLayout>
#include <algorithm>
#include <app/old/component/chemistry/atom.hpp>
#include <app/old/vtx_app.hpp>
#include <string>
#include <ui/old_ui/style.hpp>
#include <ui/old_ui/ui/main_window.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	// DistanceRenderView::DistanceRenderView( Model::Measurement::Distance * const p_model, QWidget * const p_parent )
	// : 	App::Old::Core::View::BaseView<Model::Measurement::Distance>( p_model ),
	//	VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>( p_parent )
	//{
	//	_labelPen	= QPen( Style::WORLD_LABEL_OUTLINE_COLOR );
	//	_labelBrush = QBrush( Style::WORLD_LABEL_FILL_COLOR );
	//	_labelBrush.setStyle( Qt::BrushStyle::SolidPattern );

	//	_linePen = QPen();
	//	_linePen.setColor( Style::MEASUREMENT_DISTANCE_LINE_COLOR );
	//	_linePen.setStyle( Style::MEASUREMENT_DISTANCE_LINE_STYLE );
	//	_lineBrush = QBrush( Qt::BrushStyle::NoBrush );
	//	//_lineBrush = QBrush( Qt::BrushStyle::SolidPattern );
	//}

	// void DistanceRenderView::_setupUi( const QString & p_name )
	//{
	//	VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>::_setupUi( p_name );

	//	_refreshText();
	//	updatePosition();
	//	setVisible( true );
	//}
	// void DistanceRenderView::_setupSlots() {}
	// void DistanceRenderView::localize() {}

	// void DistanceRenderView::_refreshView()
	//{
	//	_refreshText();
	//	// updatePosition called because it resizing the widget. Maybe resize can be done in a specific function
	//	updatePosition();
	//	repaint();
	//}

	// void DistanceRenderView::updatePosition()
	//{
	//	if ( !_model->isValid() )
	//		return;

	//	const Vec3f & firstAtomWorldPos	 = _model->getFirstAtom().getWorldPosition();
	//	const Vec3f & secondAtomWorldPos = _model->getSecondAtom().getWorldPosition();

	//	const bool visible = _isVisibleToCamera( firstAtomWorldPos ) || _isVisibleToCamera( secondAtomWorldPos );
	//	setVisible( visible );

	//	if ( !visible )
	//	{
	//		return;
	//	}

	//	const Vec3f centerWorldPos = ( firstAtomWorldPos + secondAtomWorldPos ) * 0.5f;

	//	const QPoint firstAtomPos  = _worldToScreen( firstAtomWorldPos );
	//	const QPoint secondAtomPos = _worldToScreen( secondAtomWorldPos );

	//	const QPoint centerPos = _worldToScreen( centerWorldPos );
	//	const QPoint textPos   = centerPos;

	//	const int minX = std::min( { firstAtomPos.x() - Style::LABEL_RENDER_POINT_MAX_DIAMETER,
	//								 secondAtomPos.x() - Style::LABEL_RENDER_POINT_MAX_DIAMETER,
	//								 ( textPos.x() - _paintData.textSize.width() / 2 ) } );
	//	const int minY = std::min( { firstAtomPos.y() - Style::LABEL_RENDER_POINT_MAX_DIAMETER,
	//								 secondAtomPos.y() - Style::LABEL_RENDER_POINT_MAX_DIAMETER,
	//								 ( textPos.y() - _paintData.textSize.height() / 2 ) } );
	//	const int maxX = std::max( { firstAtomPos.x() + Style::LABEL_RENDER_POINT_MAX_DIAMETER,
	//								 secondAtomPos.x() + Style::LABEL_RENDER_POINT_MAX_DIAMETER,
	//								 ( textPos.x() + ( _paintData.textSize.width() / 2 ) ) } );
	//	const int maxY = std::max( { firstAtomPos.y() + Style::LABEL_RENDER_POINT_MAX_DIAMETER,
	//								 secondAtomPos.y() + Style::LABEL_RENDER_POINT_MAX_DIAMETER,
	//								 ( textPos.y() + ( _paintData.textSize.height() / 2 ) ) } );

	//	const QPoint topLeftPos = QPoint( minX, minY );
	//	move( topLeftPos );

	//	const QSize size = QSize( maxX - minX, maxY - minY );
	//	setFixedSize( size );

	//	_paintData.firstAtomScreenPos	= firstAtomPos;
	//	_paintData.secondAtomScreenPos	= secondAtomPos;
	//	_paintData.textPosition			= textPos;
	//	_paintData.textDistanceToCamera = _distanceToCamera( centerWorldPos );

	//	repaint();
	//}

	// QPoint DistanceRenderView::_worldToScreen( const Vec3f & p_worldPos ) const
	//{
	//	const QRect & parentRect = parentWidget()->contentsRect();
	//	const QPoint  renderPos	 = QPoint( parentRect.width() / 2, parentRect.height() / 2 );

	//	const App::Old::Component::Render::Camera & camera = App::Old::VTXApp::get().getScene().getCamera();

	//	const Mat4f & projectionMatrix = camera.getProjectionMatrix();
	//	const Mat4f & viewMatrix	   = camera.getViewMatrix();
	//	const Vec4f	  screenPos
	//		= projectionMatrix * ( viewMatrix * Vec4f( p_worldPos.x, p_worldPos.y, p_worldPos.z, 1.f ) );

	//	QPoint screenPointPos;
	//	if ( screenPos.w != 0 )
	//	{
	//		const float absW		= screenPos.w < 0 ? -screenPos.w : screenPos.w;
	//		const Vec3f viewportPos = Vec3f( screenPos ) / absW;
	//		screenPointPos			= QPoint( ( viewportPos.x + 1.f ) * 0.5f * parentRect.width(),
	//								  ( -viewportPos.y + 1.f ) * 0.5f * parentRect.height() );
	//	}
	//	else
	//	{
	//		screenPointPos = { -1000, -1000 };
	//	}

	//	return screenPointPos;
	//}

	// float DistanceRenderView::_distanceToCamera( const Vec3f & p_worldPos ) const
	//{
	//	const App::Old::Component::Render::Camera & camera = App::Old::VTXApp::get().getScene().getCamera();
	//	return Util::distance( camera.getPosition(), p_worldPos );
	//}

	// bool DistanceRenderView::_isVisibleToCamera( const Vec3f & p_worldPos ) const
	//{
	//	const App::Old::Component::Render::Camera & camera = App::Old::VTXApp::get().getScene().getCamera();

	//	const Vec3f & cameraForward = camera.getFront();
	//	const Vec3f	  clipPos		= camera.getPosition() + cameraForward;
	//	const Vec3f	  cameraToPos	= Util::normalize( p_worldPos - clipPos );

	//	return Util::dot( cameraForward, cameraToPos ) > 0;
	//}

	// void DistanceRenderView::_refreshText() { _setText( Util::Measurement::getDistanceString( *_model ) ); }

	// void DistanceRenderView::_setText( const std::string & p_txt )
	//{
	//	const QString qstr = QString::fromStdString( p_txt );

	//	const QFontMetrics labelFontMetric = QFontMetrics( Style::WORLD_LABEL_FONT );

	//	_paintData.textSize = QSize( labelFontMetric.horizontalAdvance( qstr ), labelFontMetric.height() );

	//	_painterPath.clear();
	//	_painterPath.addText( { 0, 0 }, Style::WORLD_LABEL_FONT, qstr );
	//}

	// void DistanceRenderView::paintEvent( QPaintEvent * event )
	//{
	//	QWidget::paintEvent( event );

	//	const float minDist = 20.f;
	//	const float maxDist = 150.0f;

	//	const float ratioScale = 1.f
	//							 - std::clamp( ( _paintData.textDistanceToCamera - Style::WORLD_LABEL_NEAR_CLIP )
	//											   / ( Style::WORLD_LABEL_FAR_CLIP - Style::WORLD_LABEL_NEAR_CLIP ),
	//										   0.f,
	//										   1.f );

	//	if ( ratioScale > 0.f )
	//	{
	//		QPainter painter( this );
	//		painter.save();
	//		// painter.setWorldMatrixEnabled( false );

	//		// Draw line //////////////////////////////////////////////////////
	//		// TODO Sigmoid
	//		const int lineSize = Style::MEASUREMENT_DISTANCE_LABEL_MIN_LINE_THICKNESS
	//							 + ( Style::MEASUREMENT_DISTANCE_LABEL_MAX_LINE_THICKNESS
	//								 - Style::MEASUREMENT_DISTANCE_LABEL_MIN_LINE_THICKNESS )
	//								   * ratioScale;

	//		_linePen.setWidth( lineSize );

	//		painter.setPen( _linePen );
	//		painter.setBrush( _lineBrush );
	//		painter.setRenderHint( QPainter::RenderHint::Antialiasing );

	//		const QPoint firstAtomPos  = _paintData.firstAtomScreenPos - pos();
	//		const QPoint secondAtomPos = _paintData.secondAtomScreenPos - pos();
	//		const int	 pointRadius   = ( lineSize / 2 ) + Style::MEASUREMENT_DISTANCE_LABEL_POINT_RADIUS;

	//		painter.drawEllipse( firstAtomPos, pointRadius, pointRadius );
	//		painter.drawEllipse( secondAtomPos, pointRadius, pointRadius );
	//		painter.drawLine( firstAtomPos, secondAtomPos );
	//		///////////////////////////////////////////////////////////////////

	//		// Draw text //////////////////////////////////////////////////////

	//		painter.setBrush( _labelBrush );
	//		painter.setPen( _labelPen );

	//		const float textScale
	//			= Style::WORLD_LABEL_MIN_TEXT_SCALE
	//			  + ( Style::WORLD_LABEL_MAX_TEXT_SCALE - Style::WORLD_LABEL_MIN_TEXT_SCALE ) * ratioScale;

	//		const QPointF textPosition
	//			= ( _paintData.textPosition - pos() )
	//			  - ( QPoint( _paintData.textSize.width() / 2, -_paintData.textSize.height() / 4 ) * textScale );

	//		painter.translate( textPosition );

	//		painter.scale( textScale, textScale );
	//		painter.drawPath( _painterPath );
	//		painter.resetTransform();
	//		///////////////////////////////////////////////////////////////////

	//		// painter.setWorldMatrixEnabled( true );
	//		painter.restore();
	//	}
	//}
} // namespace VTX::View::UI::Widget::Measurement
