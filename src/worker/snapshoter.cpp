#include "snapshoter.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "define.hpp"
#ifdef CUDA_DEFINED
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#endif
#include "renderer/gl/gl.hpp"
#include "renderer/ray_tracing/ray_tracer.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"
#include <GL/gl3w.h>
#include <imgui/imgui.h>
#include <stb/stb_image_write.h>
#include <vector>

namespace VTX
{
	namespace Worker
	{
		bool Snapshoter::takeSnapshotGL( const Path & p_path ) const
		{
			const Renderer::GL & renderer = VTXApp::get().getRendererGL();

			const uint width  = renderer.getWidth();
			const uint height = renderer.getHeight();

			std::vector<uchar> image( width * height * 4 );
			glBindFramebuffer( GL_FRAMEBUFFER, renderer.getRenderedFBO() );
			glReadnPixels( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, GLsizei( image.size() ), image.data() );
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			// const GLuint & texture = renderer.getRenderedTexture();
			// std::vector<float> buffer( width * height * 4 );
			// glGetTextureImage( texture, 0, GL_RGBA, GL_FLOAT, GLsizei( buffer.size() ), &buffer );
			// std::vector<uchar> image( buffer.size() );
			// for ( uint i = 0; i < buffer.size(); ++i )
			//{
			//	image[ i ] = uchar( buffer[ i ] * 255 );
			//}

			stbi_flip_vertically_on_write( true );
			stbi_write_png_compression_level = 0;
			return stbi_write_png( p_path.string().c_str(), width, height, 4, image.data(), 0 );
			// stbi_write_jpg( p_path.c_str(), width, height, 3, buffer.data(), 100 );
			// return false;
		}

		bool Snapshoter::takeSnapshotRTCPU( const Path & p_path ) const
		{
			const Renderer::GL & renderer = VTXApp::get().getRendererGL();

			const uint width  = renderer.getWidth();
			const uint height = renderer.getHeight();

			Renderer::RayTracer * rt = new Renderer::RayTracer();
			rt->init( width, height );
			rt->renderFrame( VTXApp::get().getScene() );
			const std::vector<uchar> & pixels = rt->getPixels();
			stbi_flip_vertically_on_write( true );
			stbi_write_png_compression_level = 0;
			bool res = stbi_write_png( p_path.string().c_str(), width, height, 3, pixels.data(), 0 );
			delete rt;
			return res;
		}

		bool Snapshoter::takeSnapshotRTOptix( const Path & p_path ) const
		{
			const Renderer::GL & renderer = VTXApp::get().getRendererGL();

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
