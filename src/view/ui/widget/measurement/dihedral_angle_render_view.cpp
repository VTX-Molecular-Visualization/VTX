#include "dihedral_angle_render_view.hpp"
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
	DihedralAngleRenderView::DihedralAngleRenderView( Model::Measurement::DihedralAngle * const p_model,
													  QWidget * const							p_parent ) :
		View::BaseView<Model::Measurement::DihedralAngle>( p_model ),
		VTX::UI::Widget::BaseManualWidget<QWidget>( p_parent )
	{
		_labelPen	= QPen( Style::WORLD_LABEL_OUTLINE_COLOR );
		_labelBrush = QBrush( Style::WORLD_LABEL_FILL_COLOR );
		_labelBrush.setStyle( Qt::BrushStyle::SolidPattern );

		_linePen = QPen();
		_linePen.setColor( Style::MEASUREMENT_DIHEDRAL_ANGLE_LINE_COLOR );
		_linePen.setStyle( Style::MEASUREMENT_DIHEDRAL_ANGLE_LINE_STYLE );
		_lineBrush = QBrush( Qt::BrushStyle::NoBrush );
	}

	void DihedralAngleRenderView::_setupUi( const QString & p_name )
	{
		BaseManualWidget ::_setupUi( p_name );

		_refreshText();
		updatePosition();
		setVisible( true );
	}
	void DihedralAngleRenderView::_setupSlots() {}
	void DihedralAngleRenderView::localize() {}

	void DihedralAngleRenderView::_refreshView()
	{
		_refreshText();
		// updatePosition called because it resizing the widget. Maybe resize can be done in a specific function
		updatePosition();
		repaint();
	}

	void DihedralAngleRenderView::updatePosition()
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

		const Vec3f centerWorldPos		= Util::UIRender::getCenter( atomPositions );
		_paintData.textDistanceToCamera = Util::UIRender::distanceToCamera( camera, centerWorldPos );

		const float ratioScale = 1.f
								 - std::clamp( ( _paintData.textDistanceToCamera - Style::WORLD_LABEL_NEAR_CLIP )
												   / ( Style::WORLD_LABEL_FAR_CLIP - Style::WORLD_LABEL_NEAR_CLIP ),
											   0.f,
											   1.f );

		if ( ratioScale == 0.f )
			return;

		const QRect			renderRect	   = parentWidget()->rect();
		std::vector<QPoint> pointPositions = std::vector<QPoint>();
		Util::UIRender::fillScreenPositions( atomPositions, pointPositions, camera, renderRect );

		const float pixmapSizeScaled
			= Util::UIRender::linearInterpolation( Style::MEASUREMENT_DIHEDRAL_ANGLE_ICON_MIN_SIZE,
												   Style::MEASUREMENT_DIHEDRAL_ANGLE_ICON_MAX_SIZE,
												   ratioScale );

		const QPixmap & pixmap = Style::IconConst::get().DIHEDRAL_ANGLE_RENDER_ICON;

		const QPoint iconPos = ( pointPositions[ 1 ] + pointPositions[ 2 ] ) * 0.5f;

		const float textScale = Util::UIRender::linearInterpolation(
			Style::WORLD_LABEL_MIN_TEXT_SCALE, Style::WORLD_LABEL_MAX_TEXT_SCALE, ratioScale );

		const int textYOffset
			= ( ( _paintData.textSize.height() / 2 ) + Style::MEASUREMENT_DIHEDRAL_ANGLE_LABEL_TEXT_OFFSET ) * textScale
			  + ( pixmapSizeScaled / 2 );

		const QPoint textPos = iconPos + QPoint( 0, -textYOffset );

		int minX, maxX, minY, maxY;
		Util::UIRender::getMinMax( pointPositions, minX, maxX, minY, maxY );
		minX -= Style::LABEL_RENDER_POINT_MAX_DIAMETER;
		maxX += Style::LABEL_RENDER_POINT_MAX_DIAMETER;
		minY -= Style::LABEL_RENDER_POINT_MAX_DIAMETER;
		maxY += Style::LABEL_RENDER_POINT_MAX_DIAMETER;

		const int minXText = textPos.x() - ( _paintData.textSize.width() * ratioScale ) / 2;
		const int maxXText = textPos.x() + ( _paintData.textSize.width() * ratioScale ) / 2;
		const int minYText = textPos.y() - ( _paintData.textSize.height() * ratioScale ) / 2;
		const int maxYText = textPos.y() + ( _paintData.textSize.height() * ratioScale ) / 2;

		const int minXIcon = iconPos.x() - ( pixmapSizeScaled / 2 );
		const int maxXIcon = iconPos.x() + ( pixmapSizeScaled / 2 );
		const int minYIcon = iconPos.y() - ( pixmapSizeScaled / 2 );
		const int maxYIcon = iconPos.y() + ( pixmapSizeScaled / 2 );

		minX = std::min( { minX, minXText, minXIcon } );
		maxX = std::max( { maxX, maxXText, maxXIcon } );
		minY = std::min( { minY, minYText, minYIcon } );
		maxY = std::max( { maxY, maxYText, maxYIcon } );

		const QPoint topLeftPos = QPoint( minX, minY );
		move( topLeftPos );

		const QSize size = QSize( maxX - minX, maxY - minY );
		setFixedSize( size );

		_paintData.lineSize
			= Util::UIRender::linearInterpolation( Style::MEASUREMENT_DIHEDRAL_ANGLE_LABEL_MIN_LINE_THICKNESS,
												   Style::MEASUREMENT_DIHEDRAL_ANGLE_LABEL_MAX_LINE_THICKNESS,
												   ratioScale );

		_paintData.firstAtomScreenPos  = pointPositions[ 0 ];
		_paintData.secondAtomScreenPos = pointPositions[ 1 ];
		_paintData.thirdAtomScreenPos  = pointPositions[ 2 ];
		_paintData.fourthAtomScreenPos = pointPositions[ 3 ];

		_paintData.textPosition = textPos;
		_paintData.textScale	= textScale;

		_paintData.dihedralIconPosition = iconPos;
		_paintData.iconSize				= pixmapSizeScaled;

		repaint();
	}

	void DihedralAngleRenderView::_refreshText() { _setText( Util::Measurement::getDihedralAngleString( *_model ) ); }

	void DihedralAngleRenderView::_setText( const std::string & p_txt )
	{
		const QString qstr = QString::fromStdString( p_txt );

		const QFontMetrics labelFontMetric = QFontMetrics( Style::WORLD_LABEL_FONT );

		_paintData.textSize = QSize( labelFontMetric.horizontalAdvance( qstr ), labelFontMetric.height() );

		_painterPath.clear();
		_painterPath.addText( { 0, 0 }, Style::WORLD_LABEL_FONT, qstr );
	}

	void DihedralAngleRenderView::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		if ( Style::WORLD_LABEL_NEAR_CLIP < _paintData.textDistanceToCamera
			 && _paintData.textDistanceToCamera < Style::WORLD_LABEL_FAR_CLIP )
		{
			QPainter painter( this );
			painter.save();
			// painter.setWorldMatrixEnabled( false );

			// Draw line //////////////////////////////////////////////////////
			_linePen.setWidth( _paintData.lineSize );

			painter.setPen( _linePen );
			painter.setBrush( _lineBrush );
			painter.setRenderHint( QPainter::RenderHint::Antialiasing );

			const QPoint firstAtomPos  = _paintData.firstAtomScreenPos - pos();
			const QPoint secondAtomPos = _paintData.secondAtomScreenPos - pos();
			const QPoint thirdAtomPos  = _paintData.thirdAtomScreenPos - pos();
			const QPoint fourthAtomPos = _paintData.fourthAtomScreenPos - pos();
			const int	 pointRadius   = ( _paintData.lineSize / 2 ) + Style::LABEL_RENDER_POINT_RADIUS;

			painter.drawEllipse( firstAtomPos, pointRadius, pointRadius );
			painter.drawEllipse( secondAtomPos, pointRadius, pointRadius );
			painter.drawEllipse( thirdAtomPos, pointRadius, pointRadius );
			painter.drawEllipse( fourthAtomPos, pointRadius, pointRadius );
			painter.drawLine( firstAtomPos, secondAtomPos );
			painter.drawLine( secondAtomPos, thirdAtomPos );
			painter.drawLine( thirdAtomPos, fourthAtomPos );

			const QPixmap & pixmap = Style::IconConst::get().DIHEDRAL_ANGLE_RENDER_ICON;
			const QPoint	imageTopLeftPosition
				= _paintData.dihedralIconPosition - pos() - QPoint( _paintData.iconSize / 2, _paintData.iconSize / 2 );
			painter.drawPixmap(
				imageTopLeftPosition.x(), imageTopLeftPosition.y(), _paintData.iconSize, _paintData.iconSize, pixmap );
			///////////////////////////////////////////////////////////////////

			// Draw text //////////////////////////////////////////////////////
			const QPointF textTopLeftPosition
				= _paintData.textPosition - pos()
				  - ( QPoint( _paintData.textSize.width() / 2, -_paintData.textSize.height() / 2 )
					  * _paintData.textScale );

			painter.setBrush( _labelBrush );
			painter.setPen( _labelPen );
			painter.translate( textTopLeftPosition );

			painter.scale( _paintData.textScale, _paintData.textScale );
			painter.drawPath( _painterPath );
			painter.resetTransform();
			///////////////////////////////////////////////////////////////////

			// painter.setWorldMatrixEnabled( true );
			painter.restore();
		}
	}
} // namespace VTX::View::UI::Widget::Measurement
