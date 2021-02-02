#include "snapshoter.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "define.hpp"
#ifdef CUDA_DEFINED
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#endif
#include "renderer/gl/gl.hpp"
#include "renderer/ray_tracing/ray_tracer.hpp"
#include "ui/main_window.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"
#include <stb/stb_image_write.h>
#include <vector>

namespace VTX
{
	namespace Worker
	{
		bool Snapshoter::takeSnapshotGL( const FilePath & p_path )
		{
			QImage image = VTXApp::get().getMainWindow().getOpenGLWidget().grabFramebuffer();
			return image.save( QString( p_path.string().c_str() ), "png" );
		}

		bool Snapshoter::takeSnapshotRTCPU( const FilePath & p_path ) const
		{
			Renderer::GL::GL & renderer = VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL();

			const uint width  = renderer.getWidth();
			const uint height = renderer.getHeight();

			Renderer::RayTracer * rt = new Renderer::RayTracer( renderer.gl() );
			rt->init( width, height, renderer.getOutputFbo() );
			rt->renderFrame( VTXApp::get().getScene() );
			const std::vector<uchar> & pixels = rt->getPixels();
			stbi_flip_vertically_on_write( true );
			stbi_write_png_compression_level = 0;
			bool res = stbi_write_png( p_path.string().c_str(), width, height, 3, pixels.data(), 0 );
			delete rt;
			return res;
		}

		bool Snapshoter::takeSnapshotRTOptix( const FilePath & p_path ) const
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
			delete ort;
			return res;
#else
			VTX_WARNING( "Optix unavailable!" );
			return false;
#endif
		}

	} // namespace Worker
} // namespace VTX
