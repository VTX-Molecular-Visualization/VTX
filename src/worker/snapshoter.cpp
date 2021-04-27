#include "snapshoter.hpp"
#ifdef CUDA_DEFINED
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#endif
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
		const void Snapshoter::_takeSnapshotGL()
		{
			if ( std::filesystem::exists( _path ) )
			{
				VTX_ERROR( "File already exists" );
			}

			QSvgRenderer watermarkSvg( QString( ":/sprite/logo_vect.svg" ) );
			QSize		 watermarkSize = watermarkSvg.defaultSize();
			// QSize		 imageSize	   = _image.size();

			// Compute ratio.

			QImage watermarkImg( watermarkSize, QImage::Format_ARGB32 );
			// img.fill( 0x00000000 );
			QPainter watermarkPainter = QPainter( &watermarkImg );
			watermarkSvg.render( &watermarkPainter );

			// Compute watermark color.
			// watermarkImg.invertPixels( QImage::InvertRgba );
			Color::Rgb watermakColor
				= VTX_SETTING().backgroundColor.brightness() > 0.5f ? Color::Rgb::BLACK : Color::Rgb::WHITE;
			// watermakColor.oppose();

			QColor qWatermarkColor = watermakColor.toQColor();

			/*
		   if ( VTX_SETTING().backgroundColor.brightness() < 0.5f )
		   {
			   watermarkImg.invertPixels( QImage::InvertRgb );
		   }
		   */

			// Apply the color.

			for ( int i = 0; i < watermarkImg.width(); ++i )
			{
				for ( int j = 0; j < watermarkImg.height(); ++j )
				{
					qWatermarkColor.setAlpha( watermarkImg.pixelColor( i, j ).alpha() == 255 ? 255 : 0 );
					watermarkImg.setPixelColor( i, j, qWatermarkColor );
				}
			}

			// TODO: force max quality.
			QImage render = VTXApp::get().getMainWindow().getOpenGLWidget().grabFramebuffer();
			// TODO: reset quality.

			QPainter imagePainter = QPainter( &render );
			// imagePainter.drawImage( render.rect(), watermarkImg );

			if ( render.save( QString( _path.string().c_str() ), "png" ) )
			{
				VTX_INFO( "Snapshot taken: " + _path.filename().string() );
			}
			else
			{
				VTX_ERROR( "Snapshot failed" );
			}
		}

		void Snapshoter::_addWatermark() { QImage watermark( ":/sprite/logo_vect.svg" ); }

		// TOREDO
		const void Snapshoter::_takeSnapshotRTCPU() const
		{
			Renderer::GL::GL & renderer = VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL();

			const uint width  = renderer.getWidth();
			const uint height = renderer.getHeight();

			Renderer::RayTracer * rt = new Renderer::RayTracer( renderer.gl() );
			rt->init( width, height, renderer.getOutputFbo() );
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
