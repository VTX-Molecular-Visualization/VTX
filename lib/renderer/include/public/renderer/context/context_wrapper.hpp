#ifndef __VTX_RENDERER_CONTEXT_WRAPPER__
#define __VTX_RENDERER_CONTEXT_WRAPPER__

#include "concept.hpp"
#include "renderer/binary_buffer.hpp"
#include <span>

namespace VTX::Renderer::Context
{
	using FunctionBuild				  = std::function<void(
		  const RenderQueue &,
		  const Links &,
		  const std::vector<BufferData> &,
		  Instructions &,
		  InstructionsDurationRanges &
	  )>;
	using FunctionResize			  = std::function<void( const RenderQueue &, size_t, size_t )>;
	using FunctionSetOutput			  = std::function<void( Handle )>;
	using FunctionSetValue			  = std::function<void( const Key &, const void * const, size_t )>;
	using FunctionReserveData		  = std::function<void( const Key &, size_t )>;
	using FunctionSet				  = std::function<void( const Key &, const void * const, size_t )>;
	using FunctionSetSub			  = std::function<void( const Key &, const void * const, size_t, size_t )>;
	using FunctionGet				  = std::function<void( const Key &, void * const, size_t )>;
	using FunctionFillInfos			  = std::function<void( StructInfos & )>;
	using FunctionMeasureTaskDuration = std::function<float( const std::function<void()> & )>;
	using FunctionCompileShaders	  = std::function<void()>;
	using FunctionSnapshot
		= std::function<void( std::vector<uchar> &, const RenderQueue &, const Instructions &, size_t, size_t )>;
	using FunctionGetTextureData = std::function<void( const Key &, void * const, size_t, size_t, E_CHAN_OUT )>;
	using FunctionCompute		 = std::function<void( const ComputePass & )>;

	struct ContextInterface
	{
		FunctionBuild				build;
		FunctionResize				resize;
		FunctionSetOutput			setOutput;
		FunctionSetValue			setValue;
		FunctionReserveData			reserveData;
		FunctionSet					set;
		FunctionSetSub				setSub;
		FunctionGet					get;
		FunctionFillInfos			fillInfos;
		FunctionMeasureTaskDuration measureTaskDuration;
		FunctionCompileShaders		compileShaders;
		FunctionSnapshot			snapshot;
		FunctionGetTextureData		getTextureData;
		FunctionCompute				compute;

		void clear()
		{
			build				= nullptr;
			resize				= nullptr;
			setOutput			= nullptr;
			setValue			= nullptr;
			reserveData			= nullptr;
			set					= nullptr;
			setSub				= nullptr;
			get					= nullptr;
			fillInfos			= nullptr;
			measureTaskDuration = nullptr;
			compileShaders		= nullptr;
			snapshot			= nullptr;
			getTextureData		= nullptr;
			compute				= nullptr;
		}
	};

	class ContextWrapper
	{
	  public:
		template<ConceptContextImpl C, typename... Args>
		void set( Args &&... p_args )
		{
			C * context;
			if ( _contexts.has<C>() )
				context = _contexts.get<C>();
			else
				context = _contexts.create<C>( std::forward<Args>( p_args )... );

#define BIND_CONTEXT_FN( name ) \
	_impl.name = [ context ]( auto &&... args ) { return context->name( std::forward<decltype( args )>( args )... ); }

			BIND_CONTEXT_FN( build );
			BIND_CONTEXT_FN( resize );
			BIND_CONTEXT_FN( setOutput );
			BIND_CONTEXT_FN( setValue );
			BIND_CONTEXT_FN( reserveData );
			BIND_CONTEXT_FN( set );
			BIND_CONTEXT_FN( setSub );
			BIND_CONTEXT_FN( get );
			BIND_CONTEXT_FN( fillInfos );
			BIND_CONTEXT_FN( measureTaskDuration );
			BIND_CONTEXT_FN( compileShaders );
			BIND_CONTEXT_FN( snapshot );
			BIND_CONTEXT_FN( getTextureData );
			BIND_CONTEXT_FN( compute );

#undef BIND_CONTEXT_FN
		}

		void reset()
		{
			_impl.clear();
			_contexts.clear();
		}

		template<ConceptContextImpl C>
		bool hasContext() const
		{
			return _contexts.has<C>();
		}

		bool hasContext() const { return not _contexts.empty(); }

		template<typename... Args>
		inline void build( Args &&... p_args )
		{
			_impl.build( std::forward<Args>( p_args )... );
		}
		template<typename... Args>
		inline void resize( Args &&... p_args )
		{
			_impl.resize( std::forward<Args>( p_args )... );
		}
		template<typename... Args>
		inline void setOutput( Args &&... p_args )
		{
			_impl.setOutput( std::forward<Args>( p_args )... );
		}

		template<typename T>
		inline void setValue( const T & p_value, const Key & p_key, size_t p_index )
		{
			_impl.setValue( p_key, static_cast<const void * const>( &p_value ), p_index );
		}

		template<typename T>
		inline void reserveData( size_t p_count, const Key & p_key, const T p_dummy = T() )
		{
			_impl.reserveData( p_key, sizeof( T ) * p_count );
		}

		template<typename T>
		inline void set( const std::span<const T> p_data, const Key & p_key )
		{
			_impl.set( p_key, p_data.data(), sizeof( T ) * p_data.size() );
		}

		/*
		inline void set( const BinaryBuffer & p_buffer, const Key & p_key )
		{
			_impl.set( p_key, p_buffer.data(), p_buffer.size() );
		}
		*/

		template<std::ranges::contiguous_range R>
		void set( const R & p_data, const Key & p_key )
		{
			using T = std::remove_cv_t<std::ranges::range_value_t<R>>;
			set<T>( std::span<const T>( std::data( p_data ), std::size( p_data ) ), p_key );

			//_impl.set( p_key, p_data.data(), sizeof( T ) * p_data.size() );
		}

		template<typename T>
		inline void setSub(
			const std::span<const T> p_data,
			const Key &				 p_key,
			size_t					 p_offset	 = 0,
			size_t					 p_offsetSrc = 0,
			size_t					 p_size		 = 0
		)
		{
			size_t byteSize	  = sizeof( T ) * ( p_size ? p_size : p_data.size() );
			size_t byteOffset = sizeof( T ) * p_offset;
			_impl.setSub( p_key, p_data.data() + p_offsetSrc, byteSize, byteOffset );
		}

		/*
		inline void setSub( const BinaryBuffer & p_buffer, const Key & p_key, size_t p_index = 0 )
		{
			_impl.setSub( p_key, p_buffer.data(), p_buffer.size(), p_buffer.size() * p_index );
		}
		*/

		template<std::ranges::contiguous_range R>
		void setSub( const R & r, const Key & key, size_t p_offset = 0, size_t p_offsetSrc = 0, size_t p_size = 0 )
		{
			using T = std::remove_cv_t<std::ranges::range_value_t<R>>;
			setSub<T>( std::span<const T>( std::data( r ), std::size( r ) ), key, p_offset, p_offsetSrc, p_size );
		}

		template<typename T>
		inline void get( std::vector<T> & p_data, const Key & p_key )
		{
			_impl.get( p_key, p_data.data(), sizeof( T ) * p_data.size() );
		}

		template<typename... Args>
		inline void fillInfos( Args &&... p_args ) const
		{
			_impl.fillInfos( std::forward<Args>( p_args )... );
		}

		template<typename... Args>
		inline float measureTaskDuration( Args &&... p_args )
		{
			return _impl.measureTaskDuration( std::forward<Args>( p_args )... );
		}

		template<typename... Args>
		inline void compileShaders( Args &&... p_args ) const
		{
			_impl.compileShaders( std::forward<Args>( p_args )... );
		}

		template<typename... Args>
		inline void snapshot( Args &&... p_args )
		{
			_impl.snapshot( std::forward<Args>( p_args )... );
		}

		template<typename T>
		inline T getTextureData( const Key & p_key, size_t p_x, size_t p_y, E_CHAN_OUT p_channel ) const
		{
			T result;
			_impl.getTextureData( p_key, (void * const)&result, p_x, p_y, p_channel );
			return result;
		}

		template<typename... Args>
		inline void compute( Args &&... p_args )
		{
			_impl.compute( std::forward<Args>( p_args )... );
		}

		inline void clear() { _contexts.clear(); }

	  private:
		Util::Collection<std::unique_ptr<BaseContext>> _contexts;
		ContextInterface							   _impl;
	};
} // namespace VTX::Renderer::Context

#endif
