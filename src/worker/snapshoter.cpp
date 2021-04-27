#include "snapshoter.hpp"
#ifdef CUDA_DEFINED
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#endif
#include "renderer/gl/gl.hpp"
#include "renderer/ray_tracing/ray_tracer.hpp"
#include "ui/main_window.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"
#include <vector>

namespace VTX
{
	namespace Worker
	{
		const bool Snapshoter::_takeSnapshotGL() const
		{
			if ( std::filesystem::exists( _path ) )
			{
				emit logError( "File already exists" );
				return 0;
			}

			bool result = _image.save( QString( _path.string().c_str() ), "png" );
			if ( result )
			{
				emit logInfo( "Snapshot taken: " + _path.filename().string() );
			}
			else
			{
				emit logError( "Snapshot failed" );
			}

			return result;
		}

		// TOREDO
		const bool Snapshoter::_takeSnapshotRTCPU() const
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
			return false;
		}

		// TOREDO
		const bool Snapshoter::_takeSnapshotRTOptix() const
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
			return res;
#else
			emit logWarning( "Optix unavailable!" );
			return false;
#endif
		}

	} // namespace Worker
} // namespace VTX
