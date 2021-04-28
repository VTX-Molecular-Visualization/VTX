#include "snapshoter.hpp"
#ifdef CUDA_DEFINED
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#endif
#include "action/action_manager.hpp"
#include "action/setting.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/ray_tracing/ray_tracer.hpp"
#include "ui/main_window.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"
#include <QPainter>
#include <QSvgRenderer>
#include <vector>

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
			const bool activeAA = VTX_SETTING().activeAA;
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

			// Save.
			if ( render.save( QString( _path.string().c_str() ), "png", 0 ) )
			{
				VTX_INFO( "Snapshot taken: " + _path.filename().string() );
			}
			else
			{
				VTX_ERROR( "Snapshot failed" );
			}
		}

		void Snapshoter::_addWatermark( QImage & p_image ) const
		{
			QSvgRenderer watermarkSvg( QString( ":/sprite/logo_vect.svg" ) );
			QSize		 watermarkSize = watermarkSvg.defaultSize();

			// Compute size.
			// TODO: min/max?
			uint  desiredHeight = p_image.size().height() / 6;
			float ratio			= (float)desiredHeight / (float)watermarkSize.height();
			watermarkSize.setHeight( desiredHeight );
			watermarkSize.setWidth( watermarkSize.width() * ratio );
			watermarkSvg.setAspectRatioMode( Qt::AspectRatioMode::KeepAspectRatioByExpanding );
			QImage	 watermarkImg( watermarkSize, QImage::Format_RGBA8888 );
			QPainter watermarkPainter = QPainter( &watermarkImg );
			// watermarkPainter.setRenderHint( QPainter::Antialiasing, true );
			// watermarkPainter.setRenderHint( QPainter::LosslessImageRendering, true );

			watermarkSvg.render( &watermarkPainter );

			// Compute watermark color.
			Color::Rgb watermakColor
				= VTX_SETTING().backgroundColor.brightness() > 0.5f ? Color::Rgb::BLACK : Color::Rgb::WHITE;
			QColor qWatermarkColor = watermakColor.toQColor();

			// Apply the color.
			for ( int i = 0; i < watermarkImg.width(); ++i )
			{
				for ( int j = 0; j < watermarkImg.height(); ++j )
				{
					// qWatermarkColor.setAlpha( watermarkImg.pixelColor( i, j ).alpha() == 255 ? 127 : 0 );
					qWatermarkColor.setAlpha( watermarkImg.pixelColor( i, j ).alpha() );
					watermarkImg.setPixelColor( i, j, qWatermarkColor );
				}
			}

			QRect	 rect		  = QRect( QPoint( p_image.size().width() - watermarkImg.width(),
										   p_image.size().height() - watermarkImg.height() ),
								   watermarkImg.size() );
			QPainter imagePainter = QPainter( &p_image );
			// imagePainter.setRenderHint( QPainter::Antialiasing, true );
			// imagePainter.setRenderHint( QPainter::LosslessImageRendering, true );
			imagePainter.drawImage( rect, watermarkImg );
		}

		// TOREDO
		const void Snapshoter::_takeSnapshotRTCPU() const
		{
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
