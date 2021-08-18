#include "snapshoter.hpp"
#include "action/action_manager.hpp"
#include "action/setting.hpp"
#include "renderer/gl/gl.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"
#include <QPainter>
#include <QSvgRenderer>
#include <vector>

//#define VTX_DEBUG_WATERMARK

namespace VTX
{
	namespace Worker
	{
		const void Snapshoter::_takeSnapshotGL() const
		{
			if ( _path.exists() )
			{
				VTX_ERROR( "File already exists" );
				return;
			}

			// Force AA and disable coutner.
			UI::Widget::Render::OpenGLWidget & glWidget = VTXApp::get().getMainWindow().getOpenGLWidget();
			glWidget.setShowCounter( false );

			const bool activeAA = VTX_RENDER_EFFECT().getAA();
			if ( activeAA == false )
			{
				VTX_ACTION( new Action::Setting::ActiveAA( true ) );
			}

			glWidget.update();

			// Grab image.
			QImage render = glWidget.grabFramebuffer();

			// Restore values.
			glWidget.setShowCounter( true );

			if ( activeAA == false )
			{
				VTX_ACTION( new Action::Setting::ActiveAA( false ) );
			}

			glWidget.update();

			// Add watermark.
			_addWatermark( render );

#ifndef VTX_DEBUG_WATERMARK
			// Save.
			if ( render.save( _path.qpath(), "png", 0 ) )
			{
				VTX_INFO( "Snapshot taken: " + _path.filename() );
			}
			else
			{
				VTX_ERROR( "Snapshot failed" );
			}
#endif
		}

		void Snapshoter::_addWatermark( QImage & p_image ) const
		{
			QSvgRenderer watermarkSvg( QString( ":/sprite/logo_vect.svg" ) );
			QSize		 watermarkSize = watermarkSvg.defaultSize();

			// Compute size.
			uint  desiredHeight = p_image.size().height() / 6;
			float ratio			= desiredHeight / (float)watermarkSize.height();
			watermarkSize.setHeight( desiredHeight );
			watermarkSize.setWidth( watermarkSize.width() * ratio );

			// Create image to render svg into.
			QImage watermarkImg( watermarkSize, QImage::Format_RGBA64 );
			watermarkImg.fill( QColor( 255, 255, 255, 0 ) );
			QPainter watermarkPainter = QPainter( &watermarkImg );
			watermarkSvg.render( &watermarkPainter );

			// Reduce alpha.
			for ( int i = 0; i < watermarkImg.width(); ++i )
			{
				for ( int j = 0; j < watermarkImg.height(); ++j )
				{
					QColor pixelColor = watermarkImg.pixelColor( i, j );
					pixelColor.setAlpha( pixelColor.alpha() / 2 );
					watermarkImg.setPixelColor( i, j, pixelColor );
				}
			}

			// Invert color to match background.
			if ( VTX_RENDER_EFFECT().getBackgroundColor().brightness() < 0.5f
				 && VTX_SETTING().getSnapshotBackgroundOpacity() > 0.5f )
			{
				watermarkImg.invertPixels( QImage::InvertMode::InvertRgb );
			}

#ifdef VTX_DEBUG_WATERMARK
			watermarkImg.save( QString( _path.string().c_str() ), "png", 0 );
			return;
#endif

			// Paint watermark over the desired image.
			QRect	 rect		  = QRect( QPoint( p_image.size().width() - watermarkImg.width(),
										   p_image.size().height() - watermarkImg.height() ),
								   watermarkImg.size() );
			QPainter imagePainter = QPainter( &p_image );
			imagePainter.drawImage( rect, watermarkImg );
		}

	} // namespace Worker
} // namespace VTX
