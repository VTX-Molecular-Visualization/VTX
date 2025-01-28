#ifndef __VTX_RENDERER_CONTEXT_WRAPPER__
#define __VTX_RENDERER_CONTEXT_WRAPPER__

#include "concept.hpp"
#include "renderer/binary_buffer.hpp"

namespace VTX::Renderer::Context
{

	/**
	 * @brief Function pointers to the context implementation.
	 */
	using FunctionBuild = std::function<
		void( const RenderQueue &, const Links &, const std::vector<BufferData> &, Instructions &, InstructionsDurationRanges & )>;
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

	/**
	 * @brief Type-erased context wrapper.
	 */
	class ContextWrapper
	{
	  public:
		/**
		 * @brief Set the current context implementation.
		 */
		template<ConceptContextImpl C, typename... Args>
		void set( Args &&... p_p_args )
		{
			// Create or get context.
			C * context;
			if ( _contexts.has<C>() )
			{
				context = _contexts.get<C>();
			}
			else
			{
				context = _contexts.create<C>( std::forward<Args>( p_p_args )... );
			}

			// Connect functions.
			_build
				= [ context ]( auto &&... p_args ) { context->build( std::forward<decltype( p_args )>( p_args )... ); };
			_resize = [ context ]( auto &&... p_args )
			{ context->resize( std::forward<decltype( p_args )>( p_args )... ); };
			_setOutput = [ context ]( auto &&... p_args )
			{ context->setOutput( std::forward<decltype( p_args )>( p_args )... ); };
			_setValue = [ context ]( auto &&... p_args )
			{ context->setValue( std::forward<decltype( p_args )>( p_args )... ); };
			_reserveData = [ context ]( auto &&... p_args )
			{ context->reserveData( std::forward<decltype( p_args )>( p_args )... ); };
			_set = [ context ]( auto &&... p_args ) { context->set( std::forward<decltype( p_args )>( p_args )... ); };
			_setSub = [ context ]( auto &&... p_args )
			{ context->setSub( std::forward<decltype( p_args )>( p_args )... ); };
			_get = [ context ]( auto &&... p_args ) { context->get( std::forward<decltype( p_args )>( p_args )... ); };
			_fillInfos = [ context ]( auto &&... p_args )
			{ context->fillInfos( std::forward<decltype( p_args )>( p_args )... ); };
			_measureTaskDuration = [ context ]( auto &&... p_args )
			{ return context->measureTaskDuration( std::forward<decltype( p_args )>( p_args )... ); };
			_compileShaders = [ context ]( auto &&... p_args )
			{ context->compileShaders( std::forward<decltype( p_args )>( p_args )... ); };
			_snapshot = [ context ]( auto &&... p_args )
			{ context->snapshot( std::forward<decltype( p_args )>( p_args )... ); };
			_getTextureData = [ context ]( auto &&... p_args )
			{ context->getTextureData( std::forward<decltype( p_args )>( p_args )... ); };
			_compute = [ context ]( auto &&... p_args )
			{ context->compute( std::forward<decltype( p_args )>( p_args )... ); };
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

		template<ConceptContextImpl C>
		bool hasContext() const
		{
			return _contexts.has<C>();
		}

		template<typename... Args>
		inline void build( Args &&... p_args )
		{
			_build( std::forward<Args>( p_args )... );
		}

		template<typename... Args>
		inline void resize( Args &&... p_args )
		{
			_resize( std::forward<Args>( p_args )... );
		}

		template<typename... Args>
		inline void setOutput( Args &&... p_args )
		{
			_setOutput( std::forward<Args>( p_args )... );
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

		inline void set( const BinaryBuffer & p_buffer, const Key & p_key )
		{
			_set( p_key, p_buffer.data(), p_buffer.size() );
		}

		template<typename T>
		inline void setSub(
			const void * p_data,
			const Key &	 p_key,
			const size_t p_offset = 0,
			const size_t p_size	  = 0

		)
		{
			size_t size	  = sizeof( T ) * p_size;
			size_t offset = sizeof( T ) * p_offset;
			_setSub( p_key, p_data, size, offset );
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

		inline void setSub(
			const BinaryBuffer & p_buffer,
			const Key &			 p_key,
			const size_t		 p_index = 0

		)
		{
			_setSub( p_key, p_buffer.data(), p_buffer.size(), p_buffer.size() * p_index );
		}

		template<typename T>
		inline void get( std::vector<T> & p_data, const Key & p_key )
		{
			size_t size = sizeof( T ) * p_data.size();
			_get( p_key, static_cast<void * const>( p_data.data() ), size );
		}

		template<typename... Args>
		inline void fillInfos( Args &&... p_args ) const
		{
			_fillInfos( std::forward<Args>( p_args )... );
		}

		template<typename... Args>
		inline float measureTaskDuration( Args &&... p_args )
		{
			return _measureTaskDuration( std::forward<Args>( p_args )... );
		}

		template<typename... Args>
		inline void compileShaders( Args &&... p_args ) const
		{
			_compileShaders( std::forward<Args>( p_args )... );
		}

		template<typename... Args>
		inline void snapshot( Args &&... p_args )
		{
			_snapshot( std::forward<Args>( p_args )... );
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
		inline void compute( Args &&... p_args )
		{
			_compute( std::forward<Args>( p_args )... );
		}

		inline void clear() { _contexts.clear(); }

	  private:
		/**
		 * @brief Created context implementations.
		 */
		Util::Collection<std::unique_ptr<BaseContext>> _contexts;

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
