#ifndef __VTX_RENDERER_SYSTEM_BUILD_PIPELINE__
#define __VTX_RENDERER_SYSTEM_BUILD_PIPELINE__

#include <span>
#include <tuple>
#include <utility>

namespace VTX::Renderer
{
	/**
	 * @brief Static pipeline for system/geometry build phases.
	 */
	template<typename... B>
	class SystemBuildPipeline
	{
	  public:
		/**
		 * @brief Constructors.
		 */
		SystemBuildPipeline() = default;

		SystemBuildPipeline( B... p_builders ) : _builders( std::move( p_builders )... ) {}

		/**
		 * @brief Call given phase for all builders.
		 */
		template<typename C>
		void clear( C & p_context )
		{
			_forEachBuilder( [ & ]<typename B>( B & p_builder ) { _clear( p_builder, p_context ); } );
		}

		template<typename C, typename S>
		void registerSystems( C & p_context, std::span<const S> p_systems )
		{
			_forEachBuilder( [ & ]<typename B>( B & p_builder )
							 { _registerSystems( p_builder, p_context, p_systems ); } );
		}

		template<typename C>
		void allocateInputs( C & p_context )
		{
			_forEachBuilder( [ & ]<typename B>( B & p_builder ) { _allocateInputs( p_builder, p_context ); } );
		}

		template<typename C, typename S>
		void uploadInputs( C & p_context, std::span<const S> p_systems )
		{
			_forEachBuilder( [ & ]<typename B>( B & p_builder ) { _uploadInputs( p_builder, p_context, p_systems ); } );
		}

		template<typename C>
		void buildDerived( C & p_context )
		{
			_forEachBuilder( [ & ]<typename B>( B & p_builder ) { _buildDerived( p_builder, p_context ); } );
		}

		template<typename C>
		void allocateOutputs( C & p_context )
		{
			_forEachBuilder( [ & ]<typename B>( B & p_builder ) { _allocateOutputs( p_builder, p_context ); } );
		}

		template<typename C, typename S>
		void writeOutputs( C & p_context, std::span<const S> p_systems )
		{
			_forEachBuilder( [ & ]<typename B>( B & p_builder ) { _writeOutputs( p_builder, p_context, p_systems ); } );
		}

		template<typename C>
		void buildDrawRanges( C & p_context )
		{
			_forEachBuilder( [ & ]<typename B>( B & p_builder ) { _buildDrawRanges( p_builder, p_context ); } );
		}

		inline std::tuple<B...> & builders() noexcept { return _builders; }

		inline const std::tuple<B...> & builders() const noexcept { return _builders; }

	  private:
		/**
		 * @brief All builders.
		 */
		std::tuple<B...> _builders;

		/**
		 * @brief Apply for each builder.
		 */
		template<typename Func>
		void _forEachBuilder( Func && p_func )
		{
			std::apply( [ & ]( auto &... p_builders ) { ( p_func( p_builders ), ... ); }, _builders );
		}

		/**
		 * @brief Call builder impl. if function exists.
		 */
		template<typename BD, typename C>
		static void _clear( BD & p_builder, C & p_context )
		{
			if constexpr ( requires { p_builder.clear( p_context ); } )
			{
				p_builder.clear( p_context );
			}
		}

		template<typename BD, typename C, typename S>
		static void _registerSystems( BD & p_builder, C & p_context, std::span<const S> p_systems )
		{
			if constexpr ( requires { p_builder.registerSystems( p_context, p_systems ); } )
			{
				p_builder.registerSystems( p_context, p_systems );
			}
			else if constexpr ( requires( const S & p_system ) { p_builder.registerSystem( p_context, p_system ); } )
			{
				for ( const S & system : p_systems )
				{
					p_builder.registerSystem( p_context, system );
				}
			}
		}

		template<typename BD, typename C>
		static void _allocateInputs( BD & p_builder, C & p_context )
		{
			if constexpr ( requires { p_builder.allocateInputs( p_context ); } )
			{
				p_builder.allocateInputs( p_context );
			}
		}

		template<typename BD, typename C, typename S>
		static void _uploadInputs( BD & p_builder, C & p_context, std::span<const S> p_systems )
		{
			if constexpr ( requires { p_builder.uploadInputs( p_context, p_systems ); } )
			{
				p_builder.uploadInputs( p_context, p_systems );
			}
			else if constexpr ( requires( const S & p_system ) { p_builder.uploadInput( p_context, p_system ); } )
			{
				for ( const S & system : p_systems )
				{
					p_builder.uploadInput( p_context, system );
				}
			}
		}

		template<typename BD, typename C>
		static void _buildDerived( BD & p_builder, C & p_context )
		{
			if constexpr ( requires { p_builder.buildDerived( p_context ); } )
			{
				p_builder.buildDerived( p_context );
			}
		}

		template<typename BD, typename C>
		static void _allocateOutputs( BD & p_builder, C & p_context )
		{
			if constexpr ( requires { p_builder.allocateOutputs( p_context ); } )
			{
				p_builder.allocateOutputs( p_context );
			}
		}

		template<typename BD, typename C, typename S>
		static void _writeOutputs( BD & p_builder, C & p_context, std::span<const S> p_systems )
		{
			if constexpr ( requires { p_builder.writeOutputs( p_context, p_systems ); } )
			{
				p_builder.writeOutputs( p_context, p_systems );
			}
			else if constexpr ( requires( const S & p_system ) { p_builder.writeOutput( p_context, p_system ); } )
			{
				for ( const S & system : p_systems )
				{
					p_builder.writeOutput( p_context, system );
				}
			}
		}

		template<typename BD, typename C>
		static void _buildDrawRanges( BD & p_builder, C & p_context )
		{
			if constexpr ( requires { p_builder.buildDrawRanges( p_context ); } )
			{
				p_builder.buildDrawRanges( p_context );
			}
		}
	};
} // namespace VTX::Renderer

#endif
