#ifndef __VTX_RENDERER_CONTEXT_CONCEPT__
#define __VTX_RENDERER_CONTEXT_CONCEPT__

#include "renderer/descriptors.hpp"
#include "renderer/struct_infos.hpp"
#include <any>
#include <concepts>
#include <util/chrono.hpp>

namespace VTX::Renderer::Context
{
	using Key  = std::string;
	using Keys = std::vector<Key>;

	/**
	 * @brief Shared attributes for all render contexts.
	 */
	struct BaseContext
	{
		size_t	 width;
		size_t	 height;
		FilePath shaderPath;
	};

	/**
	 * @brief The context is the graphic API implementation.
	 */
	template<typename C>
	concept ConceptContextImpl
		= std::is_base_of<BaseContext, C>::value
		  && requires(
			  C								  p_context,
			  const size_t					  p_width,
			  const size_t					  p_height,
			  const size_t					  p_x,
			  const size_t					  p_y,
			  const RenderQueue &			  p_renderQueue,
			  const Links &					  p_links,
			  const Handle					  p_output,
			  const std::vector<BufferData> & p_globalData,
			  const Key &					  p_key,
			  const std::any &				  p_value,
			  const std::vector<std::any> &	  p_data,
			  std::vector<std::any> &		  p_dataOut,
			  const size_t					  p_size,
			  const size_t					  p_index,
			  const size_t					  p_offset,
			  const Util::Chrono::Task &	  p_task,
			  Instructions &				  p_instructions,
			  InstructionsDurationRanges &	  p_instructionsDurationRanges,
			  StructInfos &					  p_infos,
			  std::vector<uchar> &			  p_image,
			  const std::string &			  p_pass,
			  const E_CHAN_OUT				  p_channel,
			  std::any &					  p_textureData,
			  const ComputePass &			  p_computePass,
			  const bool					  p_bool
		  ) {
				 {
					 p_context.build(
						 p_renderQueue, p_links, p_output, p_globalData, p_instructions, p_instructionsDurationRanges
					 )
				 } -> std::same_as<void>;
				 { p_context.resize( p_renderQueue, p_width, p_height ) } -> std::same_as<void>;
				 { p_context.setOutput( p_output ) } -> std::same_as<void>;
				 { p_context.setValue( p_value, p_key, p_index ) } -> std::same_as<void>;
				 { p_context.reserveData( p_size, p_key, p_value ) } -> std::same_as<void>;
				 { p_context.set( p_data, p_key ) } -> std::same_as<void>;
				 { p_context.setSub( p_data, p_key, p_offset, p_bool, p_size ) } -> std::same_as<void>;
				 { p_context.get( p_dataOut, p_key ) } -> std::same_as<void>;
				 { p_context.fillInfos( p_infos ) } -> std::same_as<void>;
				 { p_context.measureTaskDuration( p_task ) } -> std::same_as<float>;
				 { p_context.compileShaders() } -> std::same_as<void>;
				 {
					 p_context.snapshot( p_image, p_renderQueue, p_instructions, p_width, p_height )
				 } -> std::same_as<void>;
				 { p_context.getTextureData( p_textureData, p_x, p_y, p_pass, p_channel ) } -> std::same_as<void>;
				 { p_context.compute( p_computePass ) } -> std::same_as<void>;
			 };

	/*
	enum struct E_FUNCTION
	{
		BUILD,
		RESIZE,
		SET_OUTPUT,
		SET_VALUE,
		RESERVE_DATA,
		SET,
		SET_SUB,
		GET,
		FILL_INFOS,
		MEASURE_TASK_DURATION,
		COMPILE_SHADERS,
		SNAPSHOT,
		GET_TEXTURE_DATA,
		COMPUTE
	};
	*/

	/*
	using FunctionBuild
		= void ( * )( const RenderQueue &, const Links &, const Handle, const std::vector<BufferData> &, Instructions &,
	InstructionsDurationRanges & ); using FunctionResize	  = void ( * )( const RenderQueue &, const size_t, const
	size_t ); using FunctionSetOutput	  = void ( * )( const Handle ); using FunctionSetValue	  = void ( * )( const
	std::any &, const Key &, const size_t ); using FunctionReserveData = void ( * )( const size_t, const Key &, const
	std::any & ); using FunctionSet		  = void ( * )( const std::vector<std::any> &, const Key & ); using
	FunctionSetSub = void ( * )( const std::vector<std::any> &, const Key &, const size_t, const bool, const size_t );
	using FunctionGet				  = void ( * )( std::vector<std::any> &, const Key & );
	using FunctionFillInfos			  = void ( * )( StructInfos & );
	using FunctionMeasureTaskDuration = float ( * )( const Util::Chrono::Task & );
	using FunctionCompileShaders	  = void ( * )();
	using FunctionSnapshot
		= void ( * )( std::vector<uchar> &, const RenderQueue &, const Instructions &, const size_t, const size_t );
	using FunctionGetTextureData = void ( * )( std::any &, const size_t, const size_t, const Key &, const E_CHAN_OUT );
	using FunctionCompute		 = void ( * )( const ComputePass & );
	*/

	using FunctionBuild = std::function<
		void( const RenderQueue &, const Links &, const Handle, const std::vector<BufferData> &, Instructions &, InstructionsDurationRanges & )>;
	using FunctionResize	  = std::function<void( const RenderQueue &, const size_t, const size_t )>;
	using FunctionSetOutput	  = std::function<void( const Handle )>;
	using FunctionSetValue	  = std::function<void( const std::any &, const Key &, const size_t )>;
	using FunctionReserveData = std::function<void( const size_t, const Key &, const std::any & )>;
	using FunctionSet		  = std::function<void( const std::vector<std::any> &, const Key & )>;
	using FunctionSetSub
		= std::function<void( const std::vector<std::any> &, const Key &, const size_t, const bool, const size_t )>;
	using FunctionGet				  = std::function<void( std::vector<std::any> &, const Key & )>;
	using FunctionFillInfos			  = std::function<void( StructInfos & )>;
	using FunctionMeasureTaskDuration = std::function<float( const Util::Chrono::Task & )>;
	using FunctionCompileShaders	  = std::function<void()>;
	using FunctionSnapshot			  = std::function<
				   void( std::vector<uchar> &, const RenderQueue &, const Instructions &, const size_t, const size_t )>;
	using FunctionGetTextureData
		= std::function<void( std::any &, const size_t, const size_t, const Key &, const E_CHAN_OUT )>;
	using FunctionCompute = std::function<void( const ComputePass & )>;

	class Context
	{
	  public:
		// Function pointer to the build function.
		template<ConceptContextImpl C>
		void set()
		{
			std::unique_ptr<C> context = std::make_unique<C>();
			//_impl					   = std::move( context );

			//_build =
		}

		/*
		template<E_FUNCTION F, typename... Args>
		void run( Args... p_args )
		{
			if constexpr ( F == E_FUNCTION::BUILD )
			{
				_build( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::RESIZE )
			{
				_resize( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::SET_OUTPUT )
			{
				_setOutput( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::SET_VALUE )
			{
				_setValue( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::RESERVE_DATA )
			{
				_reserveData( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::SET )
			{
				_set( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::SET_SUB )
			{
				_setSub( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::GET )
			{
				_get( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::FILL_INFOS )
			{
				_fillInfos( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::MEASURE_TASK_DURATION )
			{
				_measureTaskDuration( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::COMPILE_SHADERS )
			{
				_compileShaders( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::SNAPSHOT )
			{
				_snapshot( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::GET_TEXTURE_DATA )
			{
				_getTextureData( p_args... );
			}
			else if constexpr ( F == E_FUNCTION::COMPUTE )
			{
				_compute( p_args... );
			}
			else
			{
				static_assert( std::is_same_v<F, void>, "Unknown function" );
			}
		}
		*/

		void build(
			const RenderQueue &				p_renderQueue,
			const Links &					p_links,
			const Handle					p_output,
			const std::vector<BufferData> & p_globalData,
			Instructions &					p_instructions,
			InstructionsDurationRanges &	p_instructionsDurationRanges
		)
		{
			_build( p_renderQueue, p_links, p_output, p_globalData, p_instructions, p_instructionsDurationRanges );
		}

		void resize( const RenderQueue & p_renderQueue, const size_t p_width, const size_t p_height )
		{
			_resize( p_renderQueue, p_width, p_height );
		}

		void setOutput( const Handle p_output ) { _setOutput( p_output ); }

		void setValue( const std::any & p_value, const Key & p_key, const size_t p_index )
		{
			_setValue( p_value, p_key, p_index );
		}

		void reserveData( const size_t p_size, const Key & p_key, const std::any & p_value )
		{
			_reserveData( p_size, p_key, p_value );
		}

		void set( const std::vector<std::any> & p_data, const Key & p_key ) { _set( p_data, p_key ); }

	  private:
		std::unique_ptr<BaseContext> _impl;

		FunctionBuild				_build;
		FunctionResize				_resize;
		FunctionSetOutput			_setOutput;
		FunctionSetValue			_setValue;
		FunctionReserveData			_reserveData;
		FunctionSet					_set;
		FunctionSetSub				_setSub;
		FunctionGet					_get;
		FunctionFillInfos			_fillInfos;
		FunctionMeasureTaskDuration _measureTaskDuration;
		FunctionCompileShaders		_compileShaders;
		FunctionSnapshot			_snapshot;
		FunctionGetTextureData		_getTextureData;
		FunctionCompute				_compute;
	};
} // namespace VTX::Renderer::Context

#endif
