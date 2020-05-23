#include "snapshoter.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "define.hpp"
#ifdef CUDA_DEFINED
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#include "renderer/ray_tracing/ray_tracer.hpp"
#endif
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

			uint			   size = width * height * 3 * sizeof( char );
			std::vector<uchar> image( width * height * 3 );
			glBindFramebuffer( GL_FRAMEBUFFER, renderer.getRenderedFBO() );
			glReadnPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, size, image.data() );
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
			return stbi_write_png( p_path.string().c_str(), width, height, 3, image.data(), 0 );
			// stbi_write_jpg( p_path.c_str(), width, height, 3, buffer.data(), 100 );
			// return false;
		}

		bool Snapshoter::takeSnapshotRT( const Path & p_path ) const
		{
			ImGuiIO & io	 = ImGui::GetIO();
			uint	  width	 = (uint)io.DisplaySize.x;
			uint	  height = (uint)io.DisplaySize.y;

#ifdef CUDA_DEFINED
#ifdef OPTIX_DEFINED
			Renderer::OptixRayTracer * ort = new Renderer::OptixRayTracer();
			ort->init( width, height );
			ort->renderFrame( VTXApp::get().getScene() );
			const std::vector<uchar4> & pixels = ort->getPixels();
			stbi_write_png_compression_level   = 0;
			bool res = stbi_write_png( p_path.string().c_str(), width, height, 4, pixels.data(), 0 );
			delete ort;
			return res;
#else
			Renderer::RayTracer * rt = new Renderer::RayTracer();
			rt->init( width, height );
			rt->renderFrame( VTXApp::get().getScene() );
			const std::vector<uchar> & pixels = rt->getPixels();
			stbi_write_png_compression_level  = 0;
			bool res = stbi_write_png( p_path.string().c_str(), width, height, 3, pixels.data(), 0 );
			delete rt;
			return res;
#endif
#else
			return false;
#endif
		}

	} // namespace Worker
} // namespace VTX
