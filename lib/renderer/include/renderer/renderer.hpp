#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "context/opengl_45.hpp"
#include "render_graph.hpp"
#include "scheduler/depth_first_search.hpp"
#include <util/logger.hpp>

namespace VTX::Renderer
{
	class Renderer
	{
	  public:
		using RenderGraphOpenGL45 = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;

		Renderer(
			const size_t	 p_width,
			const size_t	 p_height,
			const FilePath & p_shaderPath,
			void *			 p_loader = nullptr
		) :
			_width( p_width ),
			_height( p_height ), _shaderPath( p_shaderPath ), _loader( p_loader )
		{
			_renderGraph = std::make_unique<RenderGraphOpenGL45>();

			Attachment imageGeometry { E_FORMAT::RGBA32UI };
			Attachment imageColor { E_FORMAT::RGBA16F };
			Attachment imagePicking { E_FORMAT::RG32UI };
			Attachment imageDepth { E_FORMAT::DEPTH_COMPONENT32F };

			/*
			// Geometric.
			Pass * const geo = _renderGraph->addPass(
				{ "Geometric",
				  Inputs {},
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "Geometry", imageGeometry } },
							{ E_CHANNEL_OUTPUT::COLOR_1, { "Color", imageColor } },
							{ E_CHANNEL_OUTPUT::COLOR_2, { "Picking", imagePicking } },
							{ E_CHANNEL_OUTPUT::DEPTH, { "Depth", imageDepth } } },
				  Programs { { "Sphere", "sphere", Uniforms {} }, { "Cylinder", "cylinder", Uniforms {} } } } );

			// Depth.
			Pass * const depth = _renderGraph->addPass(
				{ "Linearize depth",
				  Inputs { { E_CHANNEL_INPUT::_0, { "Depth", imageDepth } } },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", Attachment { E_FORMAT::R32F } } } },
				  Programs {
					  { "LinearizeDepth", std::vector<FilePath> { "default.vert", "linearize_depth.frag" } } } } );

			// Shading.
			Pass * const shading = _renderGraph->addPass(

				{ "Shading",
				  Inputs { { E_CHANNEL_INPUT::_0, { "Geometry", imageGeometry } },
						   { E_CHANNEL_INPUT::_1, { "Color", imageColor } },
						   { E_CHANNEL_INPUT::_2, { "Blur", Attachment { E_FORMAT::R16F } } } },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageColor } } },
				  Programs { { "Shading",
							   std::vector<FilePath> { "default.vert", "shading.frag" },
							   Uniforms { { "Specular factor", E_TYPE::FLOAT },
										  { "Background color", E_TYPE::VEC3F } } } } } );

			// FXAA.
			Pass * const fxaa
				= _renderGraph->addPass( { "FXAA",
										   Inputs { { E_CHANNEL_INPUT::_0, { "Image", imageColor } } },
										   Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageColor } } },
										   Programs { { "FXAA",
														std::vector<FilePath> { "default.vert", "fxaa.frag" },
														Uniforms { { "Color", E_TYPE::VEC3F } } } } } );
			*/

			// Debug.
			Pass * const debug = _renderGraph->addPass(
				{ "Debug",
				  Inputs {},
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageColor } } },
				  Programs {
					  { "Debug",
						std::vector<FilePath> { "default.vert", "debug.frag" },
						Uniforms { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_GREEN } },
								   { "Color2", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_BLUE } },
								   { "Test", E_TYPE::FLOAT, StructUniformValue<float> { 5646.f } },
								   { "Factor",
									 E_TYPE::FLOAT,
									 StructUniformValue<float> {
										 5.f, StructUniformValue<float>::MinMax { 0.f, 10.f } } } } } } }
			);

			// Links.
			//_renderGraph->addLink( geo, depth, E_CHANNEL::DEPTH, E_CHANNEL::COLOR_0 );
			//_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			//_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_1, E_CHANNEL_INPUT::_1 );
			//_renderGraph->addLink( shading, fxaa, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			//_renderGraph->setOutput( &fxaa->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );
			_renderGraph->setOutput( &debug->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );
		}

		template<typename T>
		inline void setUniform( const T & p_value, const std::string & p_uniform, const std::string & p_program = "" )
		{
			_renderGraph->setUniform<T>( p_value, p_uniform, p_program );
		}

		template<typename T>
		inline void getUniform( T & p_value, const Uniform & p_uniform, const Program & p_program )
		{
			return _renderGraph->getUniform<T>( p_value, p_uniform, p_program );
		}

		inline void resize( const size_t p_width, const size_t p_height ) { _renderGraph->resize( p_width, p_height ); }

		inline void build( const uint p_output = 0 )
		{
			_renderGraph->setup( _loader, _width, _height, _shaderPath, p_output );
		}

		inline void render() { _renderGraph->render(); }

		// Debug purposes only.
		inline RenderGraphOpenGL45 & getRenderGraph() { return *_renderGraph; }

	  private:
		void * _loader = nullptr;

		size_t								 _width;
		size_t								 _height;
		FilePath							 _shaderPath;
		std::unique_ptr<RenderGraphOpenGL45> _renderGraph;
	};
} // namespace VTX::Renderer

#endif
