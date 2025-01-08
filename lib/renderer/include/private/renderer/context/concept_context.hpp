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
	enum struct E_API
	{
		OPENGL45,
		DEFAULT
	};

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
	using FunctionBuild
		= void ( * )( const RenderQueue &, const Links &, const Handle, const std::vector<BufferData> &, Instructions &,
	InstructionsDurationRanges & ); using FunctionResize			  = void ( * )( const RenderQueue &, const size_t,
	const size_t ); using FunctionSetOutput			  = void ( * )( const Handle ); using FunctionSetValue			  =
	void ( * )( const Key &, const void * const, const size_t ); using FunctionReserveData		  = void ( * )( const
	Key &, const size_t ); using FunctionSet				  = void ( * )( const Key &, const void * const, const
	size_t ); using FunctionSetSub			  = void ( * )( const Key &, const void * const, const size_t, const size_t
	); using FunctionGet				  = void ( * )( const Key &, void * const, const size_t ); using
	FunctionFillInfos			  = void ( * )( StructInfos & ); using FunctionMeasureTaskDuration = float ( * )( const
	Util::Chrono::Task & ); using FunctionCompileShaders	  = void ( * )(); using FunctionSnapshot = void ( * )(
	std::vector<uchar> &, const RenderQueue &, const Instructions &, const size_t, const size_t ); using
	FunctionGetTextureData = void ( * )( const Key &, std::any &, const size_t, const size_t, const E_CHAN_OUT ); using
	FunctionCompute		 = void ( * )( const ComputePass & );
	*/

	using FunctionBuild = std::function<
		void( const RenderQueue &, const Links &, const Handle, const std::vector<BufferData> &, Instructions &, InstructionsDurationRanges & )>;
	using FunctionResize			  = std::function<void( const RenderQueue &, size_t, size_t )>;
	using FunctionSetOutput			  = std::function<void( Handle )>;
	using FunctionSetValue			  = std::function<void( const Key &, const void * const, size_t )>;
	using FunctionReserveData		  = std::function<void( const Key &, size_t )>;
	using FunctionSet				  = std::function<void( const Key &, const void * const, size_t )>;
	using FunctionSetSub			  = std::function<void( const Key &, const void * const, size_t, size_t )>;
	using FunctionGet				  = std::function<void( const Key &, void * const, size_t )>;
	using FunctionFillInfos			  = std::function<void( StructInfos & )>;
	using FunctionMeasureTaskDuration = std::function<float( const Util::Chrono::Task & )>;
	using FunctionCompileShaders	  = std::function<void()>;
	using FunctionSnapshot
		= std::function<void( std::vector<uchar> &, const RenderQueue &, const Instructions &, size_t, size_t )>;
	using FunctionGetTextureData = std::function<void( const Key &, std::any &, size_t, size_t, E_CHAN_OUT )>;
	using FunctionCompute		 = std::function<void( const ComputePass & )>;

	class Context
	{
	  public:
		template<ConceptContextImpl C, typename... Args>
		void set( Args &&... p_args )
		{
			// Create or get context.
			C * context;
			if ( _contexts.has<C>() )
			{
				context = _contexts.get<C>();
			}
			else
			{
				context = _contexts.create<C>( std::forward<Args>( p_args )... );
			}

			// Connect functions.
			_build	= [ context ]( auto &&... args ) { context->build( std::forward<decltype( args )>( args )... ); };
			_resize = [ context ]( auto &&... args ) { context->resize( std::forward<decltype( args )>( args )... ); };
			_setOutput
				= [ context ]( auto &&... args ) { context->setOutput( std::forward<decltype( args )>( args )... ); };
			_setValue
				= [ context ]( auto &&... args ) { context->setValue( std::forward<decltype( args )>( args )... ); };
			_reserveData
				= [ context ]( auto &&... args ) { context->reserveData( std::forward<decltype( args )>( args )... ); };
			_set	= [ context ]( auto &&... args ) { context->set( std::forward<decltype( args )>( args )... ); };
			_setSub = [ context ]( auto &&... args ) { context->setSub( std::forward<decltype( args )>( args )... ); };
			_get	= [ context ]( auto &&... args ) { context->get( std::forward<decltype( args )>( args )... ); };
			_fillInfos
				= [ context ]( auto &&... args ) { context->fillInfos( std::forward<decltype( args )>( args )... ); };
			_measureTaskDuration = [ context ]( auto &&... args )
			{ return context->measureTaskDuration( std::forward<decltype( args )>( args )... ); };
			_compileShaders = [ context ]( auto &&... args )
			{ context->compileShaders( std::forward<decltype( args )>( args )... ); };
			_snapshot
				= [ context ]( auto &&... args ) { context->snapshot( std::forward<decltype( args )>( args )... ); };
			_getTextureData = [ context ]( auto &&... args )
			{ context->getTextureData( std::forward<decltype( args )>( args )... ); };
			_compute
				= [ context ]( auto &&... args ) { context->compute( std::forward<decltype( args )>( args )... ); };
		}

		void reset()
		{
			_build				 = nullptr;
			_resize				 = nullptr;
			_setOutput			 = nullptr;
			_setValue			 = nullptr;
			_reserveData		 = nullptr;
			_set				 = nullptr;
			_setSub				 = nullptr;
			_get				 = nullptr;
			_fillInfos			 = nullptr;
			_measureTaskDuration = nullptr;
			_compileShaders		 = nullptr;
			_snapshot			 = nullptr;
			_getTextureData		 = nullptr;
			_compute			 = nullptr;

			_contexts.clear();
		}

		template<typename... Args>
		inline void build( Args &&... args )
		{
			_build( std::forward<Args>( args )... );
		}

		template<typename... Args>
		inline void resize( Args &&... args )
		{
			_resize( std::forward<Args>( args )... );
		}

		template<typename... Args>
		inline void setOutput( Args &&... args )
		{
			_setOutput( std::forward<Args>( args )... );
		}

		// TODO: change parameter order.
		template<typename T>
		inline void setValue( const T & p_value, const Key & p_key, const size_t p_index )
		{
			_setValue( p_key, static_cast<const void * const>( &p_value ), p_index );
		}

		template<typename T>
		inline void reserveData( const size_t p_size, const Key & p_key, const T p_dummy = T() )
		{
			size_t size = sizeof( T ) * p_size;
			_reserveData( p_key, size );
		}

		template<typename T>
		inline void set( const std::vector<T> & p_data, const Key & p_key )
		{
			size_t size = sizeof( T ) * p_data.size();
			_set( p_key, static_cast<const void * const>( p_data.data() ), size );
		}

		template<typename T>
		inline void setSub(
			const std::vector<T> & p_data,
			const Key &			   p_key,
			const size_t		   p_offset = 0,
			const size_t		   p_size	= 0

		)
		{
			size_t size	  = sizeof( T ) * ( p_size ? p_size : p_data.size() );
			size_t offset = sizeof( T ) * p_offset;
			_setSub( p_key, static_cast<const void * const>( p_data.data() ), size, offset );
		}

		template<typename T>
		inline void get( std::vector<T> & p_data, const Key & p_key )
		{
			size_t size = sizeof( T ) * p_data.size();
			_get( p_key, static_cast<void * const>( p_data.data() ), size );
		}

		template<typename... Args>
		inline void fillInfos( Args &&... args ) const
		{
			_fillInfos( std::forward<Args>( args )... );
		}

		template<typename... Args>
		inline float measureTaskDuration( Args &&... args )
		{
			return _measureTaskDuration( std::forward<Args>( args )... );
		}

		template<typename... Args>
		inline void compileShaders( Args &&... args ) const
		{
			_compileShaders( std::forward<Args>( args )... );
		}

		template<typename... Args>
		inline void snapshot( Args &&... args )
		{
			_snapshot( std::forward<Args>( args )... );
		}

		template<typename T>
		inline T getTextureData( const Key & p_key, const size_t p_x, const size_t p_y, const E_CHAN_OUT p_channel )
			const
		{
			std::any textureData = std::make_any<Vec2i>();
			_getTextureData( p_key, textureData, p_x, p_y, p_channel );
			return std::any_cast<T>( textureData );
		}

		template<typename... Args>
		inline void compute( Args &&... args )
		{
			_compute( std::forward<Args>( args )... );
		}

		inline void clear() { _contexts.clear(); }

	  private:
		Util::Collection<std::unique_ptr<BaseContext>> _contexts;
		// std::unique_ptr<BaseContext> _impl;

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
