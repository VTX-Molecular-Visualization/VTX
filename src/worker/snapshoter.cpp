#include "snapshoter.hpp"
#include "action/action_manager.hpp"
#include "action/setting.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/gl.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QPainter>
#include <QSvgRenderer>
#include <vector>

//#define VTX_DEBUG_WATERMARK

namespace VTX
{
	namespace Worker
	{
		Snapshoter::Snapshoter( const MODE & p_mode, const IO::FilePath & p_path ) :
			Snapshoter( p_mode,
						p_path,
						VTXApp::get().getMainWindow().getOpenGLWidget().width(),
						VTXApp::get().getMainWindow().getOpenGLWidget().height() )
		{
		}

		Snapshoter::Snapshoter( const MODE &				p_mode,
								const IO::FilePath &		p_path,
								const SNAPSHOT_RESOLUTION & p_resolution ) :
			Snapshoter( p_mode,
						p_path,
						( ( p_resolution == SNAPSHOT_RESOLUTION::Free )
							  ? VTXApp::get().getMainWindow().getOpenGLWidget().width()
							  : RESOLUTION_SIZE[ int( p_resolution ) ].first ),
						( ( p_resolution == SNAPSHOT_RESOLUTION::Free )
							  ? VTXApp::get().getMainWindow().getOpenGLWidget().height()
							  : RESOLUTION_SIZE[ int( p_resolution ) ].second ) )
		{
		}

		Snapshoter::Snapshoter( const MODE &		 p_mode,
								const IO::FilePath & p_path,
								const uint			 p_width,
								const uint			 p_height ) :
			_mode( p_mode ),
			_path( p_path ), _width( p_width ), _height( p_height )
		{
		}

		const void Snapshoter::_takeSnapshotGL() const
		{
			if ( _path.exists() )
			{
				VTX_ERROR( "File already exists" );
				return;
			}

			UI::Widget::Render::OpenGLWidget & glWidget	  = VTXApp::get().getMainWindow().getOpenGLWidget();
			const float						   pixelRatio = VTXApp::get().getPixelRatio();

			// Create offscreen surface.
			QOffscreenSurface surface;
			surface.setFormat( glWidget.format() );
			surface.create();

			if ( surface.isValid() == false )
			{
				VTX_ERROR( "Snapshot failed" );
				return;
			}

			// Force AA.
			const bool activeAA = VTX_RENDER_EFFECT().getAA();
			if ( activeAA == false )
			{
				VTX_ACTION( new Action::Setting::ActiveAA( true ) );
			}

			// Make current.
			glWidget.context()->makeCurrent( &surface );

			// Create FBO.
			QOpenGLFramebufferObject fbo = QOpenGLFramebufferObject( _width, _height );

			// Resize renderer and use new FBO as output.
			VTXApp::get().getScene().getCamera().setScreenSize( _width, _height );
			glWidget.getRenderer().resize( _width * pixelRatio, _height * pixelRatio, fbo.handle() );

			// Render.
			glWidget.getGL()->glViewport( 0, 0, _width, _height );
			glWidget.getRenderer().renderFrame( VTXApp::get().getScene() );

			// Save FBO as image.
			QImage render = fbo.toImage();

			// Clean.
			surface.destroy();
			glWidget.doneCurrent();

			// Resize renderer and reset default surface and FBO.
			glWidget.resizeGL( glWidget.width(), glWidget.height() );

			// Restore values.
			if ( activeAA == false )
			{
				VTX_ACTION( new Action::Setting::ActiveAA( false ) );
			}

			// Add watermark.
			_addWatermark( render );

#ifndef VTX_DEBUG_WATERMARK
			// Save.
			if ( render.save( _path.qpath(), _path.extension().c_str(), _quality ) )
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
