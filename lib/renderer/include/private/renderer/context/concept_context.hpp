#ifndef __VTX_RENDERER_CONTEXT_CONCEPT__
#define __VTX_RENDERER_CONTEXT_CONCEPT__

#include "renderer/descriptors.hpp"
#include "renderer/struct_infos.hpp"
#include <any>
#include <concepts>
#include <util/chrono.hpp>
#include <util/collection.hpp>

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
			  const void * const			  p_value,
			  const void * const			  p_data,
			  void * const					  p_dataOut,
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
			  const ComputePass &			  p_computePass
		  ) {
				 {
					 p_context.build(
						 p_renderQueue, p_links, p_output, p_globalData, p_instructions, p_instructionsDurationRanges
					 )
				 } -> std::same_as<void>;
				 { p_context.resize( p_renderQueue, p_width, p_height ) } -> std::same_as<void>;
				 { p_context.setOutput( p_output ) } -> std::same_as<void>;
				 { p_context.setValue( p_key, p_value, p_index ) } -> std::same_as<void>;
				 { p_context.reserveData( p_key, p_size ) } -> std::same_as<void>;
				 { p_context.set( p_key, p_data, p_size ) } -> std::same_as<void>;
				 { p_context.setSub( p_key, p_data, p_size, p_offset ) } -> std::same_as<void>;
				 { p_context.get( p_key, p_dataOut, p_size ) } -> std::same_as<void>;
				 { p_context.fillInfos( p_infos ) } -> std::same_as<void>;
				 { p_context.measureTaskDuration( p_task ) } -> std::same_as<float>;
				 { p_context.compileShaders() } -> std::same_as<void>;
				 {
					 p_context.snapshot( p_image, p_renderQueue, p_instructions, p_width, p_height )
				 } -> std::same_as<void>;
				 { p_context.getTextureData( p_key, p_textureData, p_x, p_y, p_channel ) } -> std::same_as<void>;
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

	using FunctionBuild
		= void ( * )( const RenderQueue &, const Links &, const Handle, const std::vector<BufferData> &, Instructions &, InstructionsDurationRanges & );
	using FunctionResize	  = void ( * )( const RenderQueue &, const size_t, const size_t );
	using FunctionSetOutput	  = void ( * )( const Handle );
	using FunctionSetValue	  = void ( * )( const std::any &, const Key &, const size_t );
	using FunctionReserveData = void ( * )( const size_t, const Key &, const std::any & );
	using FunctionSet		  = void ( * )( const std::vector<std::any> &, const Key & );
	using FunctionSetSub
		= void ( * )( const std::vector<std::any> &, const Key &, const size_t, const bool, const size_t );
	using FunctionGet				  = void ( * )( std::vector<std::any> &, const Key & );
	using FunctionFillInfos			  = void ( * )( StructInfos & );
	using FunctionMeasureTaskDuration = float ( * )( const Util::Chrono::Task & );
	using FunctionCompileShaders	  = void ( * )();
	using FunctionSnapshot
		= void ( * )( std::vector<uchar> &, const RenderQueue &, const Instructions &, const size_t, const size_t );
	using FunctionGetTextureData = void ( * )( std::any &, const size_t, const size_t, const Key &, const E_CHAN_OUT );
	using FunctionCompute		 = void ( * )( const ComputePass & );

	/*
	using FunctionBuild = std::function<
		void( const RenderQueue &, const Links &, const Handle, const std::vector<BufferData> &, Instructions &,
	InstructionsDurationRanges & )>; using FunctionResize	  = std::function<void( const RenderQueue &, const size_t,
	const size_t )>; using FunctionSetOutput	  = std::function<void( const Handle )>; using FunctionSetValue	  =
	std::function<void( const std::any &, const Key &, const size_t )>; using FunctionReserveData = std::function<void(
	const size_t, const Key &, const std::any & )>; using FunctionSet		  = std::function<void( const
	std::vector<std::any> &, const Key & )>; using FunctionSetSub = std::function<void( const std::vector<std::any> &,
	const Key &, const size_t, const bool, const size_t )>; using FunctionGet				  = std::function<void(
	std::vector<std::any> &, const Key & )>; using FunctionFillInfos			  = std::function<void( StructInfos &
	)>; using FunctionMeasureTaskDuration = std::function<float( const Util::Chrono::Task & )>; using
	FunctionCompileShaders	  = std::function<void()>; using FunctionSnapshot			  = std::function< void(
	std::vector<uchar> &, const RenderQueue &, const Instructions &, const size_t, const size_t )>; using
	FunctionGetTextureData = std::function<void( std::any &, const size_t, const size_t, const Key &, const E_CHAN_OUT
	)>; using FunctionCompute = std::function<void( const ComputePass & )>;
	*/

	class Context
	{
	  public:
		// Function pointer to the build function.
		template<ConceptContextImpl C>
		void set()
		{
			// std::unique_ptr<C> context = std::make_unique<C>();
			//_impl					   = std::move( context );

			//_impl = std::make_unique<C>();

			// Connect function pointer.
			//_build = &context->build;

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

		inline void build(
			const RenderQueue &				p_renderQueue,
			const Links &					p_links,
			const Handle					p_output,
			const std::vector<BufferData> & p_globalData,
			Instructions &					p_instructions,
			InstructionsDurationRanges &	p_instructionsDurationRanges
		)
		{
			//_build( p_renderQueue, p_links, p_output, p_globalData, p_instructions, p_instructionsDurationRanges );
		}

		inline void resize( const RenderQueue & p_renderQueue, const size_t p_width, const size_t p_height )
		{
			//_resize( p_renderQueue, p_width, p_height );
		}

		inline void setOutput( const Handle p_output )
		{
			//_setOutput( p_output );
		}

		template<typename T>
		inline void setValue( const T & p_value, const Key & p_key, const size_t p_index )
		{
			_setValue( p_value, p_key, p_index );
		}

		template<typename T>
		inline void reserveData( const size_t p_size, const Key & p_key, const T p_dummy = T() )
		{
			//_reserveData( p_size, p_key, p_dummy );
		}

		template<typename T>
		inline void set( const std::vector<T> & p_data, const Key & p_key )
		{
			//_set( p_data, p_key );
		}

		template<typename T>
		inline void setSub(
			const std::vector<T> & p_data,
			const Key &			   p_key,
			const size_t		   p_offset		  = 0,
			const bool			   p_offsetSource = false,
			const size_t		   p_size		  = 0
		)
		{
			//_setSub( p_data, p_key, p_offset, p_offsetSource, p_size );
		}

		template<typename T>
		inline void get( std::vector<T> & p_data, const Key & p_key )
		{
			//_get( p_data, p_key );
		}

		inline void fillInfos( StructInfos & p_infos )
		{
			//_fillInfos( p_infos );
		}

		inline float measureTaskDuration( const Util::Chrono::Task & p_task )
		{
			// return _measureTaskDuration( p_task );
			return 0.f;
		}

		inline void compileShaders()
		{
			//_compileShaders();
		}

		inline void snapshot(
			std::vector<uchar> & p_image,
			const RenderQueue &	 p_renderQueue,
			const Instructions & p_instructions,
			const size_t		 p_width,
			const size_t		 p_height
		)

		{
			//_snapshot( p_image, p_renderQueue, p_instructions, p_width, p_height );
		}

		template<typename T>
		inline void getTextureData(
			T &				 p_textureData,
			const size_t	 p_x,
			const size_t	 p_y,
			const Key &		 p_pass,
			const E_CHAN_OUT p_channel
		)

		{
			//_getTextureData( p_textureData, p_x, p_y, p_pass, p_channel );
		}

		inline void compute( const ComputePass & p_computePass )
		{
			//_compute( p_computePass );
		}

	  private:
		Util::Collection<std::unique_ptr<BaseContext>> _contexts;

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
