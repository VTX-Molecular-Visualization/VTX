#include "distance_render_view.hpp"
#include "model/atom.hpp"
#include "style.hpp"
#include "ui/main_window.hpp"
#include "util/string.hpp"
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
	DistanceRenderView::DistanceRenderView( Model::Measurement::Distance * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Measurement::Distance>( p_model ), VTX::UI::Widget::BaseManualWidget<QWidget>( p_parent )
	{
		_labelFont = QFont( "Arial", 20, QFont::Weight::Bold, false );

		_labelPen	= QPen( Qt::GlobalColor::black );
		_labelBrush = QBrush( Qt::GlobalColor::white );
		_labelBrush.setStyle( Qt::BrushStyle::SolidPattern );

		_linePen = QPen();
		_linePen.setColor( Qt::GlobalColor::blue );
		_linePen.setWidth( 4 );
		_linePen.setStyle( Qt::PenStyle::DashLine );
		_lineBrush = QBrush( Qt::BrushStyle::SolidPattern );
	}

	void DistanceRenderView::_setupUi( const QString & p_name )
	{
		BaseManualWidget ::_setupUi( p_name );

		_refreshText();
		updatePosition();
		setVisible( true );
	}
	void DistanceRenderView::_setupSlots() {}
	void DistanceRenderView::localize() {}

	void DistanceRenderView::updatePosition()
	{
		const QPoint firstAtomPos  = worldToScreen( _model->getFirstAtom().getWorldPosition() );
		const QPoint secondAtomPos = worldToScreen( _model->getSecondAtom().getWorldPosition() );

		const QPoint centerPos = ( firstAtomPos + secondAtomPos ) / 2;
		const QSize	 textSize  = _textSize;
		const QPoint textPos   = centerPos;

		const int minX = std::min( { firstAtomPos.x() - POINT_DIAMETER,
									 secondAtomPos.x() - POINT_DIAMETER,
									 ( textPos.x() - textSize.width() / 2 ) } );
		const int minY = std::min( { firstAtomPos.y() - POINT_DIAMETER,
									 secondAtomPos.y() - POINT_DIAMETER,
									 ( textPos.y() - textSize.height() / 2 ) } );
		const int maxX = std::max( { firstAtomPos.x() + POINT_DIAMETER,
									 secondAtomPos.x() + POINT_DIAMETER,
									 ( textPos.x() + ( textSize.width() / 2 ) ) } );
		const int maxY = std::max( { firstAtomPos.y() + POINT_DIAMETER,
									 secondAtomPos.y() + POINT_DIAMETER,
									 ( textPos.y() + ( textSize.height() / 2 ) ) } );

		const QPoint topLeftPos = QPoint( minX, minY );
		move( topLeftPos );

		const QSize size = QSize( maxX - minX, maxY - minY );
		setFixedSize( size );

		_paintData.firstAtomScreenPos  = firstAtomPos;
		_paintData.secondAtomScreenPos = secondAtomPos;
		_paintData.textPosition
			= QPoint( textPos.x() - ( textSize.width() / 2 ), textPos.y() - ( textSize.height() / 2 ) );

		repaint();
	}

	QPoint DistanceRenderView::worldToScreen( const Vec3f & p_worldPos ) const
	{
		const QRect & parentRect = parentWidget()->contentsRect();
		const QPoint  renderPos	 = QPoint( parentRect.width() / 2, parentRect.height() / 2 );

		const Object3D::Camera & camera = VTXApp::get().getScene().getCamera();

		const Mat4f & projectionMatrix = camera.getProjectionMatrix();
		const Mat4f & viewMatrix	   = camera.getViewMatrix();
		const Vec4f	  screenPos
			= projectionMatrix * ( viewMatrix * Vec4f( p_worldPos.x, p_worldPos.y, p_worldPos.z, 1.f ) );

		QPoint screenPointPos;
		if ( screenPos.w != 0 )
		{
			const Vec3f viewportPos = Vec3f( screenPos ) / screenPos.w;
			screenPointPos			= QPoint( ( viewportPos.x + 1.f ) * 0.5f * parentRect.width(),
									  ( -viewportPos.y + 1.f ) * 0.5f * parentRect.height() );
		}
		else
		{
			screenPointPos = { -1000, -1000 };
		}

		return screenPointPos;
	}

	void DistanceRenderView::_refreshText()
	{
		const std::string strTxt
			= Util::String::floatToStr( _model->getDistance(), Style::MEASUREMENT_DISTANCE_NB_DECIMALS ) + "Å";
		_setText( strTxt );
	}

	void DistanceRenderView::_setText( const std::string & p_txt )
	{
		const QString qstr = QString::fromStdString( p_txt );

		const QFontMetrics labelFontMetric = QFontMetrics( _labelFont );

		_textSize = QSize( labelFontMetric.horizontalAdvance( qstr ), labelFontMetric.height() );

		_painterPath.clear();
		_painterPath.addText( QPoint( _textSize.width() / 2, _textSize.height() / 2 ), _labelFont, qstr );
	}

	void DistanceRenderView::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		QPainter painter( this );
		painter.save();
		// painter.setWorldMatrixEnabled( false );

		painter.setPen( QPen( Qt::GlobalColor::red ) );
		// painter.drawRect( rect() );

		// Draw line //////////////////////////////////////////////////////
		painter.setPen( _linePen );
		painter.setBrush( _lineBrush );
		painter.setRenderHint( QPainter::RenderHint::Antialiasing );

		const QPoint firstAtomPos  = _paintData.firstAtomScreenPos - pos();
		const QPoint secondAtomPos = _paintData.secondAtomScreenPos - pos();
		painter.drawEllipse( firstAtomPos, POINT_RADIUS, POINT_RADIUS );
		painter.drawEllipse( secondAtomPos, POINT_RADIUS, POINT_RADIUS );
		painter.drawLine( firstAtomPos, secondAtomPos );
		///////////////////////////////////////////////////////////////////

		// Draw text //////////////////////////////////////////////////////
		painter.setBrush( _labelBrush );
		painter.setPen( _labelPen );

		const QPointF textPosition
			= _paintData.textPosition - pos() + QPoint( -_textSize.width() / 2, _textSize.height() / 2 );
		//_painterPath.setElementPositionAt( 0, textPosition.x(), textPosition.y() );
		painter.drawPath( _painterPath.translated( textPosition ) );
		///////////////////////////////////////////////////////////////////

		// painter.setWorldMatrixEnabled( true );
		painter.restore();
	}
} // namespace VTX::View::UI::Widget::Measurement
