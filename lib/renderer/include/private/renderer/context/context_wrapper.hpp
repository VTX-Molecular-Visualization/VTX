#ifndef __VTX_RENDERER_CONTEXT_WRAPPER__
#define __VTX_RENDERER_CONTEXT_WRAPPER__

#include "concept.hpp"
#include "renderer/binary_buffer.hpp"

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
	using FunctionMeasureTaskDuration = std::function<float( const Util::Chrono::Task & )>;
	using FunctionCompileShaders	  = std::function<void()>;
	using FunctionSnapshot
		= std::function<void( std::vector<uchar> &, const RenderQueue &, const Instructions &, size_t, size_t )>;
	using FunctionGetTextureData = std::function<void( const Key &, std::any &, size_t, size_t, E_CHAN_OUT )>;
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
		void set( Args &&... p_p_args )
		{
			C * context;
			if ( _contexts.has<C>() )
				context = _contexts.get<C>();
			else
				context = _contexts.create<C>( std::forward<Args>( p_p_args )... );

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

		template<typename... Args>
		inline void build( Args &&... args )
		{
			_impl.build( std::forward<Args>( args )... );
		}
		template<typename... Args>
		inline void resize( Args &&... args )
		{
			_impl.resize( std::forward<Args>( args )... );
		}
		template<typename... Args>
		inline void setOutput( Args &&... args )
		{
			_impl.setOutput( std::forward<Args>( args )... );
		}

		template<typename T>
		inline void setValue( const T & value, const Key & key, size_t index )
		{
			_impl.setValue( key, static_cast<const void * const>( &value ), index );
		}

		template<typename T>
		inline void reserveData( size_t count, const Key & key, const T dummy = T() )
		{
			_impl.reserveData( key, sizeof( T ) * count );
		}

		template<typename T>
		inline void set( const std::vector<T> & data, const Key & key )
		{
			_impl.set( key, data.data(), sizeof( T ) * data.size() );
		}

		inline void set( const BinaryBuffer & buffer, const Key & key )
		{
			_impl.set( key, buffer.data(), buffer.size() );
		}

		template<typename T>
		inline void setSub(
			const std::vector<T> & data,
			const Key &			   key,
			size_t				   offset	 = 0,
			size_t				   offsetSrc = 0,
			size_t				   size		 = 0
		)
		{
			size_t byteSize	  = sizeof( T ) * ( size ? size : data.size() );
			size_t byteOffset = sizeof( T ) * offset;
			_impl.setSub( key, data.data() + offsetSrc, byteSize, byteOffset );
		}

		inline void setSub( const BinaryBuffer & buffer, const Key & key, size_t index = 0 )
		{
			_impl.setSub( key, buffer.data(), buffer.size(), buffer.size() * index );
		}

		template<typename T>
		inline void get( std::vector<T> & data, const Key & key )
		{
			_impl.get( key, data.data(), sizeof( T ) * data.size() );
		}

		template<typename... Args>
		inline void fillInfos( Args &&... args ) const
		{
			_impl.fillInfos( std::forward<Args>( args )... );
		}

		template<typename... Args>
		inline float measureTaskDuration( Args &&... args )
		{
			return _impl.measureTaskDuration( std::forward<Args>( args )... );
		}

		template<typename... Args>
		inline void compileShaders( Args &&... args ) const
		{
			_impl.compileShaders( std::forward<Args>( args )... );
		}

		template<typename... Args>
		inline void snapshot( Args &&... args )
		{
			_impl.snapshot( std::forward<Args>( args )... );
		}

		template<typename T>
		inline T getTextureData( const Key & key, size_t x, size_t y, E_CHAN_OUT channel ) const
		{
			std::any result;
			_impl.getTextureData( key, result, x, y, channel );
			return std::any_cast<T>( result );
		}

		template<typename... Args>
		inline void compute( Args &&... args )
		{
			_impl.compute( std::forward<Args>( args )... );
		}

		inline void clear() { _contexts.clear(); }

	  private:
		Util::Collection<std::unique_ptr<BaseContext>> _contexts;
		ContextInterface							   _impl;
	};
} // namespace VTX::Renderer::Context

#endif
