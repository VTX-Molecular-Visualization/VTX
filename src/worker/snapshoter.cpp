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

// #define VTX_DEBUG_WATERMARK

namespace VTX::Worker
{
	Snapshoter::Snapshoter( const MODE &					p_mode,
							const IO::FilePath &			p_path,
							const IO::Struct::ImageExport & p_exportData ) :
		_mode( p_mode ),
		_path( p_path ), _exportData( p_exportData )
	{
	}

	Snapshoter::Snapshoter( const MODE &					p_mode,
							QImage *						p_imageTarget,
							const IO::Struct::ImageExport & p_exportData ) :
		Snapshoter( p_mode, IO::FilePath(), p_exportData )
	{
		_imageTarget = p_imageTarget;
	}

	void Snapshoter::_run()
	{
		switch ( _mode )
		{
		case MODE::GL: _takeSnapshotGL(); break;
		default: break;
		}
	}

	const void Snapshoter::_takeSnapshotGL() const
	{
		UI::Widget::Render::RenderWidget & renderWidget
			= VTXApp::get().getMainWindow().getWidget<UI::Widget::Render::RenderWidget>( ID::UI::Window::RENDER );

		UI::Widget::Render::OpenGLWidget & glWidget = renderWidget.getOpenGLWidget();

		const float pixelRatio = glWidget.getScreenPixelRatio();

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

		const int width	 = _exportData.getSize().first;
		const int height = _exportData.getSize().second;

		// Create FBO.
		QOpenGLFramebufferObject fbo = QOpenGLFramebufferObject( width, height );

		// Resize renderer and use new FBO as output.
		VTXApp::get().getScene().getCamera().setScreenSize( width, height );
		glWidget.getRenderer().resize( width * pixelRatio, height * pixelRatio, fbo.handle() );

		// Render.
		glWidget.getGL()->glViewport( 0, 0, width, height );

		// Use Export Data background opacity for the render.
		const float previousBackgroundOpacity = VTX_SETTING().getSnapshotBackgroundOpacity();
		VTX_SETTING().freezeEvent( true );
		VTX_SETTING().setSnapshotBackgroundOpacity( _exportData.getBackgroundOpacity() );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		glWidget.getRenderer().renderFrame( VTXApp::get().getScene() );
		VTX_SETTING().setSnapshotBackgroundOpacity( previousBackgroundOpacity );
		VTX_SETTING().freezeEvent( false );

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
		if ( _imageTarget != nullptr )
		{
			*_imageTarget = render;
		}

		// Save.
		if ( !_path.empty() )
		{
			if ( render.save( _path.qpath(), _path.extension().c_str(), _exportData.getIntQuality() ) )
			{
				VTX_INFO( "Snapshot taken: " + _path.filename() );
			}
			else
			{
				VTX_ERROR( "Snapshot failed" );
			}
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
		if ( VTX_RENDER_EFFECT().getBackgroundColor().brightness() < 0.5f && _exportData.getBackgroundOpacity() > 0.5f )
		{
			watermarkImg.invertPixels( QImage::InvertMode::InvertRgb );
		}

#ifdef VTX_DEBUG_WATERMARK
		watermarkImg.save( QString( _path.string().c_str() ), "png", 0 );
		return;
#endif

		// Paint watermark over the desired image.
		QRect rect = QRect(
			QPoint( p_image.size().width() - watermarkImg.width(), p_image.size().height() - watermarkImg.height() ),
			watermarkImg.size() );
		QPainter imagePainter = QPainter( &p_image );
		imagePainter.drawImage( rect, watermarkImg );
	}

} // namespace VTX::Worker
