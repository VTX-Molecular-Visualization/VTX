#include "old_tool/view/ui/widget/measurement/dihedral_angle_render_view.hpp"
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
	DihedralAngleRenderView::DihedralAngleRenderView( Model::Measurement::DihedralAngle * const p_model,
													  QWidget * const							p_parent ) :
		View::BaseView<Model::Measurement::DihedralAngle>( p_model ),
		VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>( p_parent )
	{
		_labelPen	= QPen( VTX::UI::Style::WORLD_LABEL_OUTLINE_COLOR );
		_labelBrush = QBrush( VTX::UI::Style::WORLD_LABEL_FILL_COLOR );
		_labelBrush.setStyle( Qt::BrushStyle::SolidPattern );

		_linePen = QPen();
		_linePen.setStyle( VTX::UI::Style::MEASUREMENT_DIHEDRAL_ANGLE_LINE_STYLE );
		_lineBrush = QBrush( Qt::BrushStyle::NoBrush );
	}

	void DihedralAngleRenderView::_setupUi( const QString & p_name )
	{
		VTX::UI::Widget::Render::TemplatedIntegratedWidget<QWidget>::_setupUi( p_name );

		_paintData.pixmap = QPixmap( VTX::UI::Style::IconConst::get().DIHEDRAL_ANGLE_RENDER_ICON_MASK.size() );

		_refreshText();
		_refreshColor();
		updatePosition();
		setVisible( true );
	}
	void DihedralAngleRenderView::_setupSlots() {}
	void DihedralAngleRenderView::localize() {}

	void DihedralAngleRenderView::_refreshView()
	{
		_refreshText();
		_refreshColor();
		// updatePosition called because it resizing the widget. Maybe resize can be done in a specific function
		updatePosition();
		repaint();
	}

	void DihedralAngleRenderView::updatePosition()
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

		const float ratioScale
			= 1.f
			  - std::clamp( ( _paintData.textDistanceToCamera - VTX::UI::Style::WORLD_LABEL_NEAR_CLIP )
								/ ( VTX::UI::Style::WORLD_LABEL_FAR_CLIP - VTX::UI::Style::WORLD_LABEL_NEAR_CLIP ),
							0.f,
							1.f );

		if ( ratioScale == 0.f )
		{
			repaint();
			return;
		}

		const QRect		   renderRect	  = parentWidget()->rect();
		std::vector<Vec3f> vec3fPositions = std::vector<Vec3f>();
		Util::UIRender::fillScreenPositions( atomPositions, vec3fPositions, camera, renderRect );

		std::vector<QPoint> qPointPositions = std::vector<QPoint>();
		qPointPositions.reserve( vec3fPositions.size() );
		for ( const Vec3f & vec : vec3fPositions )
			qPointPositions.emplace_back( Util::UIRender::vec3fToQPoint( vec ) );

		_paintData.lineSize
			= Util::UIRender::linearInterpolation( VTX::UI::Style::MEASUREMENT_DIHEDRAL_ANGLE_LABEL_MIN_LINE_THICKNESS,
												   VTX::UI::Style::MEASUREMENT_DIHEDRAL_ANGLE_LABEL_MAX_LINE_THICKNESS,
												   ratioScale );
		_paintData.pointRadius	= ( _paintData.lineSize / 2 ) + VTX::UI::Style::LABEL_RENDER_POINT_RADIUS;
		const int pointDiameter = _paintData.pointRadius * 2;

		int minX, maxX, minY, maxY;
		Util::UIRender::getMinMax( qPointPositions, minX, maxX, minY, maxY );
		minX -= pointDiameter;
		maxX += pointDiameter;
		minY -= pointDiameter;
		maxY += pointDiameter;

		const Vec3f iconVec3Pos = ( vec3fPositions[ 1 ] + vec3fPositions[ 2 ] ) * 0.5f;

		const bool drawTextAndIcon = iconVec3Pos.z >= 0;

		if ( drawTextAndIcon )
		{
			const QPoint iconPos = Util::UIRender::vec3fToQPoint( iconVec3Pos );
			const float	 pixmapSizeScaled
				= Util::UIRender::linearInterpolation( VTX::UI::Style::MEASUREMENT_DIHEDRAL_ANGLE_ICON_MIN_SIZE,
													   VTX::UI::Style::MEASUREMENT_DIHEDRAL_ANGLE_ICON_MAX_SIZE,
													   ratioScale );

			const float textScale = Util::UIRender::linearInterpolation(
				VTX::UI::Style::WORLD_LABEL_MIN_TEXT_SCALE, VTX::UI::Style::WORLD_LABEL_MAX_TEXT_SCALE, ratioScale );

			const int textYOffset = ( ( _paintData.textSize.height() / 2 )
									  + VTX::UI::Style::MEASUREMENT_DIHEDRAL_ANGLE_LABEL_TEXT_OFFSET )
										* textScale
									+ ( pixmapSizeScaled / 2 );

			const Vec3f	 textVec3fPos = iconVec3Pos + Vec3f( 0, -textYOffset, 0 );
			const QPoint textPos	  = Util::UIRender::vec3fToQPoint( textVec3fPos );

			const int minXIcon = iconPos.x() - ( pixmapSizeScaled / 2 );
			const int maxXIcon = iconPos.x() + ( pixmapSizeScaled / 2 );
			const int minYIcon = iconPos.y() - ( pixmapSizeScaled / 2 );
			const int maxYIcon = iconPos.y() + ( pixmapSizeScaled / 2 );

			const int minXText = textPos.x() - ( _paintData.textSize.width() * ratioScale ) / 2;
			const int maxXText = textPos.x() + ( _paintData.textSize.width() * ratioScale ) / 2;
			const int minYText = textPos.y() - ( _paintData.textSize.height() * ratioScale ) / 2;
			const int maxYText = textPos.y() + ( _paintData.textSize.height() * ratioScale ) / 2;

			minX = std::min( { minX, minXText, minXIcon } );
			maxX = std::max( { maxX, maxXText, maxXIcon } );
			minY = std::min( { minY, minYText, minYIcon } );
			maxY = std::max( { maxY, maxYText, maxYIcon } );

			_paintData.textPosition = textVec3fPos;
			_paintData.textScale	= textScale;

			_paintData.dihedralIconPosition = iconVec3Pos;
			_paintData.iconSize				= pixmapSizeScaled;
		}

		const QPoint topLeftPos = QPoint( minX, minY );
		move( topLeftPos );

		const QSize size = QSize( maxX - minX, maxY - minY );
		setFixedSize( size );

		_paintData.lineSize
			= Util::UIRender::linearInterpolation( VTX::UI::Style::MEASUREMENT_DIHEDRAL_ANGLE_LABEL_MIN_LINE_THICKNESS,
												   VTX::UI::Style::MEASUREMENT_DIHEDRAL_ANGLE_LABEL_MAX_LINE_THICKNESS,
												   ratioScale );

		_paintData.drawTextAndIcon = drawTextAndIcon;

		_paintData.firstAtomScreenPos  = vec3fPositions[ 0 ];
		_paintData.secondAtomScreenPos = vec3fPositions[ 1 ];
		_paintData.thirdAtomScreenPos  = vec3fPositions[ 2 ];
		_paintData.fourthAtomScreenPos = vec3fPositions[ 3 ];

		repaint();
	}

	void DihedralAngleRenderView::_refreshText() { _setText( Util::Measurement::getDihedralAngleString( *_model ) ); }
	void DihedralAngleRenderView::_refreshColor()
	{
		const QColor lineColor = Util::UI::RgbToQColor( _model->getColor() );
		_linePen.setColor( lineColor );
		_lineBrush.setColor( lineColor );

		_paintData.pixmap.fill( Util::UI::RgbToQColor( _model->getColor() ) );
		_paintData.pixmap.setMask( VTX::UI::Style::IconConst::get().DIHEDRAL_ANGLE_RENDER_ICON_MASK );
	}

	void DihedralAngleRenderView::_setText( const std::string & p_txt )
	{
		const QString qstr = QString::fromStdString( p_txt );

		const QFontMetrics labelFontMetric = QFontMetrics( VTX::UI::Style::WORLD_LABEL_FONT );

		_paintData.textSize = QSize( labelFontMetric.horizontalAdvance( qstr ), labelFontMetric.height() );

		_painterPath.clear();
		_painterPath.addText( { 0, 0 }, VTX::UI::Style::WORLD_LABEL_FONT, qstr );
	}

	void DihedralAngleRenderView::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		if ( !_model->isValid() || !_model->isEnable() )
			return;

		if ( _paintData.textDistanceToCamera < VTX::UI::Style::WORLD_LABEL_FAR_CLIP )
		{
			QPainter painter( this );
			painter.save();
			// painter.setWorldMatrixEnabled( false );

			// Draw line //////////////////////////////////////////////////////
			_linePen.setWidth( _paintData.lineSize );

			painter.setPen( _linePen );
			painter.setBrush( _lineBrush );
			painter.setRenderHint( QPainter::RenderHint::Antialiasing );

			const Vec3f vec3fPos = Vec3f( pos().x(), pos().y(), 0 );

			const Vec3f relativeFirstAtomPos  = _paintData.firstAtomScreenPos - vec3fPos;
			const Vec3f relativeSecondAtomPos = _paintData.secondAtomScreenPos - vec3fPos;
			const Vec3f relativeThirdAtomPos  = _paintData.thirdAtomScreenPos - vec3fPos;
			const Vec3f relativeFourthAtomPos = _paintData.fourthAtomScreenPos - vec3fPos;

			if ( _paintData.firstAtomScreenPos.z >= 0 )
			{
				painter.drawEllipse( Util::UIRender::vec3fToQPoint( relativeFirstAtomPos ),
									 _paintData.pointRadius,
									 _paintData.pointRadius );
			}
			if ( _paintData.secondAtomScreenPos.z >= 0 )
			{
				painter.drawEllipse( Util::UIRender::vec3fToQPoint( relativeSecondAtomPos ),
									 _paintData.pointRadius,
									 _paintData.pointRadius );
			}
			if ( _paintData.thirdAtomScreenPos.z >= 0 )
			{
				painter.drawEllipse( Util::UIRender::vec3fToQPoint( relativeThirdAtomPos ),
									 _paintData.pointRadius,
									 _paintData.pointRadius );
			}
			if ( _paintData.fourthAtomScreenPos.z >= 0 )
			{
				painter.drawEllipse( Util::UIRender::vec3fToQPoint( relativeFourthAtomPos ),
									 _paintData.pointRadius,
									 _paintData.pointRadius );
			}

			painter.drawLine( Util::UIRender::getScreenLine( relativeFirstAtomPos, relativeSecondAtomPos ) );
			painter.drawLine( Util::UIRender::getScreenLine( relativeSecondAtomPos, relativeThirdAtomPos ) );
			painter.drawLine( Util::UIRender::getScreenLine( relativeThirdAtomPos, relativeFourthAtomPos ) );
			///////////////////////////////////////////////////////////////////

			// Draw text & Pixmap /////////////////////////////////////////////
			if ( _paintData.textPosition.z >= 0 )
			{
				const QPoint iconPos = Util::UIRender::vec3fToQPoint( _paintData.dihedralIconPosition );
				const QPoint textPos = Util::UIRender::vec3fToQPoint( _paintData.textPosition );

				const QPoint imageTopLeftPosition
					= iconPos - pos() - QPoint( _paintData.iconSize / 2, _paintData.iconSize / 2 );
				painter.drawPixmap( imageTopLeftPosition.x(),
									imageTopLeftPosition.y(),
									_paintData.iconSize,
									_paintData.iconSize,
									_paintData.pixmap );

				const QPointF textTopLeftPosition
					= textPos - pos()
					  - ( QPoint( _paintData.textSize.width() / 2, -_paintData.textSize.height() / 2 )
						  * _paintData.textScale );

				painter.setBrush( _labelBrush );
				painter.setPen( _labelPen );
				painter.translate( textTopLeftPosition );

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
