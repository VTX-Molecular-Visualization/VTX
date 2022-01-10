#include "distance_render_view.hpp"
#include "model/atom.hpp"
#include "style.hpp"
#include "ui/main_window.hpp"
#include "util/string.hpp"
#include "vtx_app.hpp"
#include <QFont>
#include <QPainter>
#include <QPoint>
#include <QVBoxLayout>
#include <string>

namespace VTX::View::UI::Widget::Measurement
{
	DistanceRenderView::DistanceRenderView( Model::Measurement::Distance * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Measurement::Distance>( p_model ), VTX::UI::Widget::BaseManualWidget<QLabel>( p_parent )
	{
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
		const QRect & parentRect = parentWidget()->contentsRect();
		const QPoint  renderPos	 = QPoint( parentRect.width() / 2, parentRect.height() / 2 );

		const Object3D::Camera & camera = VTXApp::get().getScene().getCamera();

		const Mat4f & projectionMatrix = camera.getProjectionMatrix();
		const Mat4f & viewMatrix	   = camera.getViewMatrix();
		const Vec4f	  screenPos
			= projectionMatrix * ( viewMatrix * Vec4f( _worldPosition.x, _worldPosition.y, _worldPosition.z, 1.f ) );

		if ( screenPos.w != 0 )
		{
			const Vec3f	 viewportPos	= Vec3f( screenPos ) / screenPos.w;
			const QPoint screenPointPos = QPoint( ( viewportPos.x + 1.f ) * 0.5f * parentRect.width(),
												  ( -viewportPos.y + 1.f ) * 0.5f * parentRect.height() );

			move( screenPointPos );
		}
	}

	void DistanceRenderView::_refreshText()
	{
		const std::string strTxt
			= Util::String::floatToStr( _model->getDistance(), Style::MEASUREMENT_DISTANCE_NB_DECIMALS ) + "Å";
		setStyleSheet( "color:red;" );
		setText( QString::fromStdString( strTxt ) );

		_worldPosition
			= ( _model->getFirstAtom().getWorldPosition() + _model->getSecondAtom().getWorldPosition() ) / 2.f;
	}

	/*void DistanceRenderView::paintEvent( QPaintEvent * event )
	{
		QPainter painter( this );
		painter.save();
		painter.setWorldMatrixEnabled( false );
		painter.setPen( Qt::GlobalColor::red );
		painter.setBrush( Qt::NoBrush );

		// const QPoint centerPos = _worldPosition;

		const QPoint topLeftPos = QPoint( 0, 0 );
		painter.drawStaticText( topLeftPos, _distanceTxt );
	}*/
} // namespace VTX::View::UI::Widget::Measurement
