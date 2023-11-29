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

			Attachment imageRGBA32UI { E_FORMAT::RGBA32UI };
			Attachment imageRGBA16F { E_FORMAT::RGBA16F };
			Attachment imageRG32UI { E_FORMAT::RG32UI };
			Attachment imageD32F { E_FORMAT::DEPTH_COMPONENT32F };

			Data dataMolecules { { { "Positions", E_TYPE::FLOAT, 3 },
								   { "Colors", E_TYPE::FLOAT, 4 },
								   { "Radii", E_TYPE::FLOAT, 1 },
								   { "Visibilities", E_TYPE::UINT, 1 },
								   { "Selections", E_TYPE::UINT, 1 },
								   { "Ids", E_TYPE::UINT, 1 } } };

			Data dataMeshes { { { "Positions", E_TYPE::FLOAT, 3 },
								{ "Normales", E_TYPE::FLOAT, 3 },
								{ "Colors", E_TYPE::FLOAT, 4 },
								{ "Visibilities", E_TYPE::UINT, 1 },
								{ "Selections", E_TYPE::UINT, 1 },
								{ "Ids", E_TYPE::UINT, 1 } } };

			// Geometric.
			Pass * const geo = _renderGraph->addPass(
				{ "Geometric",
				  Inputs { { E_CHANNEL_INPUT::_0, { "Molecules", dataMolecules } },
						   { E_CHANNEL_INPUT::_1, { "Meshes", dataMeshes } } },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "Geometry", imageRGBA32UI } },
							{ E_CHANNEL_OUTPUT::COLOR_1, { "Color", imageRGBA16F } },
							{ E_CHANNEL_OUTPUT::COLOR_2, { "Picking", imageRG32UI } },
							{ E_CHANNEL_OUTPUT::DEPTH, { "Depth", imageD32F } } },
				  Programs { { "Sphere", "sphere", Uniforms {}, Draw { "Molecules", E_PRIMITIVE::POINTS } },
							 { "Cylinder", "cylinder", Uniforms {}, Draw { "Molecules", E_PRIMITIVE::LINES, true } } } }
			);

			/*
			// Depth.
			Pass * const depth = _renderGraph->addPass(
				{ "Linearize depth",
				  Inputs { { E_CHANNEL_INPUT::_0, { "Depth", imageD32F } } },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", Attachment { E_FORMAT::R32F } } } },
				  Programs {
					  { "LinearizeDepth", std::vector<FilePath> { "default.vert", "linearize_depth.frag" } } } } );

			// Shading.
			Pass * const shading = _renderGraph->addPass(

				{ "Shading",
				  Inputs { { E_CHANNEL_INPUT::_0, { "Geometry", imageRGBA32UI } },
						   { E_CHANNEL_INPUT::_1, { "Color", imageRGBA16F } },
						   { E_CHANNEL_INPUT::_2, { "Blur", Attachment { E_FORMAT::R16F } } } },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
				  Programs { { "Shading",
							   std::vector<FilePath> { "default.vert", "shading.frag" },
							   Uniforms { { "Specular factor", E_TYPE::FLOAT },
										  { "Background color", E_TYPE::VEC3F } } } } } );

			// FXAA.
			Pass * const fxaa
				= _renderGraph->addPass( { "FXAA",
										   Inputs { { E_CHANNEL_INPUT::_0, { "Image", imageRGBA16F } } },
										   Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
										   Programs { { "FXAA",
														std::vector<FilePath> { "default.vert", "fxaa.frag" },
														Uniforms { { "Color", E_TYPE::VEC3F } } } } } );
			*/

			// Debug.
			Pass * const debug = _renderGraph->addPass(
				{ "Debug",
				  Inputs { { E_CHANNEL_INPUT::_0, { "", imageRGBA16F } } },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
				  Programs {
					  { "Debug",
						std::vector<FilePath> { "default.vert", "debug.frag" },
						Uniforms { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_YELLOW } },
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

			_renderGraph->addLink( geo, debug, E_CHANNEL_OUTPUT::COLOR_1, E_CHANNEL_INPUT::_0 );

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
