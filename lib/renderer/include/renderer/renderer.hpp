#ifndef __VTX_RENDERER_RENDERER__
#define __VTX_RENDERER_RENDERER__

#include "context/opengl_45.hpp"
#include "render_graph.hpp"
#include "scheduler/depth_first_search.hpp"
#include "struct_proxy_mesh.hpp"
#include "struct_proxy_molecule.hpp"
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer
{
	class Renderer
	{
	  public:
		using RenderGraphOpenGL45 = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;
		using CallbackClean		  = std::function<void()>;
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
			// Graph.
			_renderGraph = std::make_unique<RenderGraphOpenGL45>();

			// Passes.
			Pass * const geo	 = _renderGraph->addPass( descPassGeometric );
			Pass * const depth	 = _renderGraph->addPass( descPassDepth );
			Pass * const ssao	 = _renderGraph->addPass( descPassSSAO );
			Pass * const blurX	 = _renderGraph->addPass( descPassBlur );
			Pass * const blurY	 = _renderGraph->addPass( descPassBlur );
			Pass * const shading = _renderGraph->addPass( descPassShading );
			Pass * const outline = _renderGraph->addPass( descPassOutline );
			Pass * const fxaa	 = _renderGraph->addPass( desPassFXAA );
			// Pass * const crt	 = _renderGraph->addPass( descPassCRT );

			// Setup values.
			geo->programs[ 0 ].draw.value().count	 = &_sizeAtoms;
			geo->programs[ 1 ].draw.value().count	 = &_sizeBonds;
			blurX->name								 = "BlurX";
			blurY->name								 = "BlurY";
			blurY->programs[ 0 ].uniforms[ 0 ].value = StructUniformValue<Vec2i> { Vec2i( 0, 1 ) };

			// Links.
			_renderGraph->addLink( geo, depth, E_CHANNEL_OUTPUT::DEPTH, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( geo, ssao, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, ssao, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_2 );
			_renderGraph->addLink( ssao, blurX, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, blurX, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( blurX, blurY, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, blurY, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( geo, shading, E_CHANNEL_OUTPUT::COLOR_1, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( blurY, shading, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_2 );
			_renderGraph->addLink( shading, outline, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->addLink( depth, outline, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_1 );
			_renderGraph->addLink( outline, fxaa, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			_renderGraph->setOutput( &fxaa->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );
			//_renderGraph->addLink( fxaa, crt, E_CHANNEL_OUTPUT::COLOR_0, E_CHANNEL_INPUT::_0 );
			//_renderGraph->setOutput( &crt->outputs[ E_CHANNEL_OUTPUT::COLOR_0 ] );

			// Shared uniforms.
			_renderGraph->addUniforms(
				{ { "Matrix model", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  { "Matrix normal", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  { "Matrix view", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  { "Matrix projection", E_TYPE::MAT4F, StructUniformValue<Mat4f> { MAT4F_ID } },
				  // { _near * _far, _far, _far - _near, _near }
				  { "Camera clip infos", E_TYPE::VEC4F, StructUniformValue<Vec4f> { VEC4F_ZERO } },
				  { "Mouse position", E_TYPE::VEC2I, StructUniformValue<Vec2i> { { 0, 0 } } },
				  { "Is perspective", E_TYPE::UINT, StructUniformValue<uint> { 1 } } }
			);

			// create array of 256 color white.
			std::array<Util::Color::Rgba, 256> colorLayout;
			std::generate( colorLayout.begin(), colorLayout.end(), [] { return Util::Color::Rgba::random(); } );

			_renderGraph->addUniforms( { { "Color layout",
										   E_TYPE::COLOR4_256,
										   StructUniformValue<std::array<Util::Color::Rgba, 256>> { colorLayout } } } );
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

		inline void resize( const size_t p_width, const size_t p_height, const uint p_output = 0 )
		{
			_width	= p_width;
			_height = p_height;

			if ( _renderGraph->isBuilt() )
			{
				_renderGraph->resize( p_width, p_height );
			}

			VTX_DEBUG( "resize: {} {}", p_width, p_height );
		}

		inline void setOutput( const uint p_output )
		{
			if ( _renderGraph->isBuilt() )
			{
				_renderGraph->setOutput( p_output );
			}
		}

		inline void build( const uint p_output = 0, void * p_loader = nullptr )
		{
			clean();

			VTX_INFO(
				"Renderer graph setup total time: {}",
				Util::CHRONO_CPU(
					[ & ]()
					{
						if ( _renderGraph->setup(
								 p_loader ? p_loader : _loader,
								 _width,
								 _height,
								 _shaderPath,
								 _instructions,
								 _instructionsDurationRanges,
								 p_output
							 ) )
						{
							for ( const StructProxyMolecule & proxy : _proxiesMolecules )
							{
								_setData( proxy );
							}

							_renderGraph->fillInfos( _infos );

							_onReady();
						}
					}
				)
			);
		}

		inline void clean()
		{
			_instructions.clear();
			_instructionsDurationRanges.clear();
			_renderGraph->clean();
			_infos = StructInfos();

			_onClean();
		}

		inline void render( const float p_time )
		{
			if ( _logDurations )
			{
				for ( InstructionsDurationRange & instructionDurationRange : _instructionsDurationRanges )
				{
					instructionDurationRange.duration = _renderGraph->measureTaskDuration(

						[ this, &instructionDurationRange ]()
						{
							for ( size_t i = instructionDurationRange.first; i <= instructionDurationRange.last; ++i )
							{
								_instructions[ i ]();
							}
						}
					);
				}
			}
			else
			{
				for ( const Instruction & instruction : _instructions )
				{
					instruction();
				}
			}
		}

		inline void setCallbackClean( const CallbackClean & p_cb ) { _callbackClean = p_cb; }

		inline void setCallbackReady( const CallbackReady & p_cb ) { _callbackReady = p_cb; }

		inline void setMatrixView( const Mat4f & p_view ) { setUniform( p_view, "Matrix view" ); }

		inline void setMatrixProjection( const Mat4f & p_proj ) { setUniform( p_proj, "Matrix projection" ); }

		inline void setCameraClipInfos( const float p_near, const float p_far )
		{
			setUniform( Vec4f( p_near * p_far, p_far, p_far - p_near, p_near ), "Camera clip infos" );
		}

		inline void setMousePosition( const Vec2i & p_position )
		{
			setUniform( Vec2i { p_position.x, _height - p_position.y }, "Mouse position" );
		}

		inline void addMolecule( const StructProxyMolecule & p_proxy )
		{
			_proxiesMolecules.push_back( p_proxy );

			if ( _renderGraph->isBuilt() )
			{
				_setData( p_proxy );
			}
		}

		inline void setColorLayout( const std::array<Util::Color::Rgba, 256> p_layout )
		{
			setUniform( p_layout, "Color layout" );
		}

		inline const size_t getWidth() const { return _width; }
		inline const size_t getHeight() const { return _height; }
		inline const size_t getAtomCount() const { return _sizeAtoms; }
		inline const size_t getBondCount() const { return _sizeBonds; }

		inline const StructInfos & getInfos() const { return _infos; }

		inline const bool isLogDurations() const { return _logDurations; }
		inline void		  setLogDurations( const bool p_value ) { _logDurations = p_value; }
		inline void		  compileShaders() const { _renderGraph->compileShaders(); }

		// Debug purposes only.
		inline RenderGraphOpenGL45 &			  getRenderGraph() { return *_renderGraph; }
		inline const InstructionsDurationRanges & getInstructionsDurationRanges() const
		{
			return _instructionsDurationRanges;
		}

	  private:
		void * _loader = nullptr;

		size_t								 _width;
		size_t								 _height;
		FilePath							 _shaderPath;
		std::unique_ptr<RenderGraphOpenGL45> _renderGraph;
		Instructions						 _instructions;
		InstructionsDurationRanges			 _instructionsDurationRanges;
		StructInfos							 _infos;
		bool								 _logDurations = false;

		CallbackClean _callbackClean;
		CallbackReady _callbackReady;

		std::vector<StructProxyMolecule> _proxiesMolecules;

		size_t _sizeAtoms = 0;
		size_t _sizeBonds = 0;

		void _setData( const StructProxyMolecule & p_proxy )
		{
			assert( p_proxy.atomPositions->size() == p_proxy.atomColors->size() );
			assert( p_proxy.atomPositions->size() == p_proxy.atomRadii->size() );
			assert( p_proxy.atomPositions->size() == p_proxy.atomVisibilities->size() );
			assert( p_proxy.atomPositions->size() == p_proxy.atomSelections->size() );
			assert( p_proxy.atomPositions->size() == p_proxy.atomIds->size() );

			_renderGraph->setData( *p_proxy.atomPositions, "MoleculesPositions" );
			_renderGraph->setData( *p_proxy.atomColors, "MoleculesColors" );
			_renderGraph->setData( *p_proxy.atomRadii, "MoleculesRadii" );
			_renderGraph->setData( *p_proxy.atomIds, "MoleculesIds" );
			_renderGraph->setData( *p_proxy.bonds, "MoleculesEbo" );

			std::vector<uchar> atomFlags( p_proxy.atomPositions->size() );

			enum E_ATOM_FLAGS
			{
				VISIBILITY = 0,
				SELECTION  = 1
			};

			for ( size_t i = 0; i < atomFlags.size(); ++i )
			{
				uchar flag = 0;
				flag |= ( *p_proxy.atomVisibilities )[ i ] << E_ATOM_FLAGS::VISIBILITY;
				flag |= ( *p_proxy.atomSelections )[ i ] << E_ATOM_FLAGS::SELECTION;
				atomFlags[ i ] = flag;
			}

			_renderGraph->setData( atomFlags, "MoleculesFlags" );

			_sizeAtoms = p_proxy.atomPositions->size();
			_sizeBonds = p_proxy.bonds->size();
		}

		inline void _onClean()
		{
			if ( _callbackClean )
			{
				_callbackClean();
			}
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
