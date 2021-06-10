#include "snapshoter.hpp"
#ifdef CUDA_DEFINED
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#endif
#include "action/action_manager.hpp"
#include "action/setting.hpp"
#include "renderer/gl/gl.hpp"
//#include "renderer/ray_tracing/ray_tracer.hpp"
#include "ui/main_window.hpp"
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
			if ( std::filesystem::exists( _path ) )
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

			// Create a new one.
			// QImage image( render.size(), QImage::Format_ARGB32 );

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
			if ( render.save( QString( _path.string().c_str() ), "png", 0 ) )
			{
				VTX_INFO( "Snapshot taken: " + _path.filename().string() );
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

		// TOREDO
		const void Snapshoter::_takeSnapshotRTCPU() const
		{
			/*
			Renderer::GL::GL & renderer = VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL();

			const uint width  = renderer.getWidth();
			const uint height = renderer.getHeight();

			Renderer::RayTracer * rt = new Renderer::RayTracer( renderer.getGL() );
			rt->init( width, height, renderer.getOutputFramebuffer().getId() );
			rt->renderFrame( VTXApp::get().getScene() );
			const std::vector<uchar> & pixels = rt->getPixels();
			// stbi_flip_vertically_on_write( true );
			// stbi_write_png_compression_level = 0;
			// bool res = stbi_write_png( p_path.string().c_str(), width, height, 3, pixels.data(), 0 );
			// VTX_INFO( "Render computed: " + _path.filename().string() );
			delete rt;
			*/
		}

		// TOREDO
		const void Snapshoter::_takeSnapshotRTOptix() const
		{
			const Renderer::GL::GL & renderer = VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL();

			const uint width  = renderer.getWidth();
			const uint height = renderer.getHeight();

#if defined( CUDA_DEFINED ) && defined( OPTIX_DEFINED )
			Renderer::Optix::OptixRayTracer * ort = new Renderer::Optix::OptixRayTracer();
			ort->init( width, height );
			ort->renderFrame( VTXApp::get().getScene() );
			const std::vector<uchar4> & pixels = ort->getPixels();
			stbi_flip_vertically_on_write( true );
			stbi_write_png_compression_level = 0;
			bool res = stbi_write_png( p_path.string().c_str(), width, height, 4, pixels.data(), 0 );
			// VTX_INFO( "Render computed: " + _path.filename().string() );
			delete ort;
#else
			VTX_WARNING( "Optix unavailable!" );
#endif
		}

	} // namespace Worker
} // namespace VTX