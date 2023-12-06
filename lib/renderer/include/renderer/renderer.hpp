#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "context/opengl_45.hpp"
#include "render_graph.hpp"
#include "scheduler/custom_search.hpp"
#include "struct_proxy_mesh.hpp"
#include "struct_proxy_molecule.hpp"
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer
{
	class Renderer
	{
	  public:
		using RenderGraphOpenGL45 = RenderGraph<Context::OpenGL45, Scheduler::CustomSearch>;
		using CallbackReady		  = std::function<void()>;

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

			/*
			Data dataMeshes { { { "Positions", E_TYPE::FLOAT, 3 },
								{ "Normales", E_TYPE::FLOAT, 3 },
								{ "Colors", E_TYPE::FLOAT, 4 },
								{ "Visibilities", E_TYPE::UINT, 1 },
								{ "Selections", E_TYPE::UINT, 1 },
								{ "Ids", E_TYPE::UINT, 1 } } };
*/

			// Geometric.
			Pass * const geo = _renderGraph->addPass(
				{ "Geometric",
				  Inputs {
					  { E_CHANNEL_INPUT::_0, { "Molecules", dataMolecules } },
					  //{ E_CHANNEL_INPUT::_1, { "Meshes", dataMeshes } }
				  },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "Geometry", imageRGBA32UI } },
							{ E_CHANNEL_OUTPUT::COLOR_1, { "Color", imageRGBA16F } },
							{ E_CHANNEL_OUTPUT::COLOR_2, { "Picking", imageRG32UI } },
							{ E_CHANNEL_OUTPUT::DEPTH, { "Depth", imageD32F } } },
				  Programs {
					  { "Sphere", "sphere", Uniforms {}, Draw { "Molecules", E_PRIMITIVE::POINTS, &_sizeAtoms } },
					  { "Cylinder",
						"cylinder",
						Uniforms {},
						Draw { "Molecules", E_PRIMITIVE::LINES, &_sizeBonds, true } } },
				  { E_SETTING::CLEAR } }
			);

			// Depth.
			Pass * const depth = _renderGraph->addPass(
				{ "Linearize depth",
				  Inputs { { E_CHANNEL_INPUT::_0, { "Depth", imageD32F } } },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", Attachment { E_FORMAT::R32F } } } },
				  Programs { { "LinearizeDepth", std::vector<FilePath> { "default.vert", "linearize_depth.frag" } } } }
			);

			// Shading.
			Pass * const shading = _renderGraph->addPass(
				{ "Shading",
				  Inputs { { E_CHANNEL_INPUT::_0, { "Geometry", imageRGBA32UI } },
						   { E_CHANNEL_INPUT::_1, { "Color", imageRGBA16F } },
						   { E_CHANNEL_INPUT::_2, { "Blur", Attachment { E_FORMAT::R16F } } } },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
				  Programs {
					  { "Shading",
						std::vector<FilePath> { "default.vert", "shading.frag" },
						Uniforms {
							{ "Background color",
							  E_TYPE::COLOR4,
							  StructUniformValue<Util::Color::Rgba> { COLOR_BLACK } },
							{ "Light color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
							{ "Fog color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
							{ "Mode",
							  E_TYPE::INT,
							  StructUniformValue<int> { int( E_SHADING::FLAT_COLOR ),
														StructUniformValue<int>::MinMax { int( E_SHADING::DIFFUSE ),
																						  int( E_SHADING::COUNT ) } } },
							{ "Specular factor",
							  E_TYPE::FLOAT,
							  StructUniformValue<float> { 0.4f, StructUniformValue<float>::MinMax { 0.f, 1.f } } },
							{ "Toon steps",
							  E_TYPE::UINT,
							  StructUniformValue<uint> { 1, StructUniformValue<uint>::MinMax { 1, 15 } } },
							{ "Fog near",
							  E_TYPE::FLOAT,
							  StructUniformValue<float> { 30.f, StructUniformValue<float>::MinMax { 0.f, 1000.f } } },
							{ "Fog far",
							  E_TYPE::FLOAT,
							  StructUniformValue<float> { 80.f, StructUniformValue<float>::MinMax { 0.f, 1000.f } } },
							{ "Fog density",
							  E_TYPE::FLOAT,
							  StructUniformValue<float> { 0.f, StructUniformValue<float>::MinMax { 0.f, 1.f } } },
						} } } }

			);

			// FXAA.
			Pass * const fxaa = _renderGraph->addPass(
				{ "FXAA",
				  Inputs { { E_CHANNEL_INPUT::_0, { "Image", imageRGBA16F } } },
				  Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
				  Programs { { "FXAA", std::vector<FilePath> { "default.vert", "fxaa.frag" } } } }
			);

			// Links.
			_renderGraph->addLink( geo, depth, E_CHANNEL_OUTPUT::DEPTH, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_1, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( shading, fxaa, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->setOutput( &fxaa->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );

			// Shared uniforms.
			_renderGraph->addUniforms(
				{ { "Matrix model", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  { "Matrix normal", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  { "Matrix view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  { "Matrix projection", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  // { _near * _far, _far, _far - _near, _near }
				  { "Camera clip infos", E_TYPE::VEC4F, StructUniformValue<Vec4f> { VEC4F_ZERO } },
				  // TODO: check why not compiling with bool.
				  { "Is perspective", E_TYPE::INT, StructUniformValue<int> { 0 } } }
			);

			//  Debug pass.
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

			//_renderGraph->addLink( geo, debug, E_CHANNEL_OUTPUT::COLOR_1, E_CHANNEL_INPUT::_0 );
			//_renderGraph->setOutput( &debug->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );

			// build();
		}

		template<typename T>
		inline void setUniform( const T & p_value, const std::string & p_key )
		{
			_renderGraph->setUniform<T>( p_value, p_key );
		}

		template<typename T>
		inline void getUniform( T & p_value, const std::string & p_key )
		{
			_renderGraph->getUniform<T>( p_value, p_key );
		}

		inline void resize( const size_t p_width, const size_t p_height ) { _renderGraph->resize( p_width, p_height ); }

		inline void build( const uint p_output = 0 )
		{
			_instructions.clear();

			VTX_INFO(
				"Renderer graph setup total time: {}",
				Util::CHRONO_CPU(
					[ & ]() { _renderGraph->setup( _loader, _width, _height, _shaderPath, _instructions, p_output ); }
				)
			);

			for ( const StructProxyMolecule & proxy : _molecules )
			{
				_setData( proxy );
			}

			_onReady();
		}

		inline void render( const float p_time )
		{
			for ( Instruction & instruction : _instructions )
			{
				instruction();
			}
		}

		inline void setCallbackReady( const CallbackReady & p_cb ) { _callbackReady = p_cb; }

		inline void setMatrixView( const Mat4f & p_view ) { setUniform( p_view, "Matrix view" ); }

		inline void setMatrixProjection( const Mat4f & p_proj ) { setUniform( p_proj, "Matrix projection" ); }

		inline void setCameraClipInfos( const float p_near, const float p_far )
		{
			setUniform( Vec4f( p_near * p_far, p_far, p_far - p_near, p_near ), "Camera clip infos" );
		}

		inline void addMolecule( const StructProxyMolecule & p_proxy )
		{
			_molecules.push_back( p_proxy );

			if ( _renderGraph->isBuilt() )
			{
				_setData( p_proxy );
			}
		}

		// Debug purposes only.
		inline RenderGraphOpenGL45 & getRenderGraph() { return *_renderGraph; }

	  private:
		void * _loader = nullptr;

		size_t								 _width;
		size_t								 _height;
		FilePath							 _shaderPath;
		std::unique_ptr<RenderGraphOpenGL45> _renderGraph;
		Instructions						 _instructions;
		CallbackReady						 _callbackReady;

		std::vector<StructProxyMolecule> _molecules;

		size_t _sizeAtoms = 0;
		size_t _sizeBonds = 0;

		void _setData( const StructProxyMolecule & p_proxy )
		{
			_renderGraph->setData( *p_proxy.atomPositions, "MoleculesPositions" );
			_renderGraph->setData( *p_proxy.atomColors, "MoleculesColors" );
			_renderGraph->setData( *p_proxy.atomRadii, "MoleculesRadii" );
			_renderGraph->setData( *p_proxy.atomVisibilities, "MoleculesVisibilities" );
			_renderGraph->setData( *p_proxy.atomSelections, "MoleculesSelections" );
			_renderGraph->setData( *p_proxy.atomIds, "MoleculesIds" );
			_renderGraph->setData( *p_proxy.bonds, "MoleculesEbo" );

			_sizeAtoms = p_proxy.atomPositions->size();
			_sizeBonds = p_proxy.bonds->size();
		}

		inline void _onReady()
		{
			if ( _callbackReady )
			{
				_callbackReady();
			}
		}
	};
} // namespace VTX::Renderer
#endif
