#ifndef VTX_APP_FIXTURE_HPP
#define VTX_APP_FIXTURE_HPP

#include "action/io.hpp"
#include "pending_updates.hpp"
#include "services.hpp"
#include "vtx_app.hpp"
#include <chrono>
#include <concepts>
#include <future>
#include <stdexcept>
#include <util/action_manager.hpp>
#include <util/filesystem.hpp>
#include <utility>

namespace VTX::App
{
	class Fixture
	{
	  public:
		/**
		 * @brief Timeout for predicates waiting.
		 */
		static constexpr std::chrono::milliseconds DEFAULT_TIMEOUT { 5000 };
		static constexpr std::chrono::milliseconds SCRIPT_TIMEOUT { std::chrono::minutes( 2 ) };

		/**
		 * @brief Default app args.
		 */
		static Arguments getDefaultArguments()
		{
			static const char * argv[] = { "" };

			Arguments args;
			args.argc		= 1;
			args.argv		= argv;
			args.noGraphics = true;
			args.noUpdates	= true;
			args.noPython	= true;
			args.noGui		= true;
			args.debug		= true;
			return args;
		}

		/**
		 * @brief Constructors.
		 */
		Fixture() : Fixture( getDefaultArguments() ) {}

		explicit Fixture( Arguments p_args )
		{
			if ( p_args.argv == nullptr )
			{
				static const char * argv[] = { "" };
				p_args.argc				   = 1;
				p_args.argv				   = argv;
			}

			_app = std::make_unique<VTXApp>( std::move( p_args ) );
			_app->start();
		}

		~Fixture() { _app.reset(); }

		/**
		 * @brief System loading.
		 */
		void loadSystem( const FilePath & p_path )
		{
			ACTION().execute<Action::IO::LoadSystem>( p_path );
			if ( not runUntilIdle() )
			{
				throw std::runtime_error( "Application did not become idle after loading " + p_path.string() );
			}
		}

		void loadSystem( const std::string_view p_filename = "1AGA.mmtf" )
		{ loadSystem( Util::Filesystem::getExecutableDir() / "data" / p_filename ); }

		/**
		 * @brief Wait for a predicate is true.
		 */
		template<std::predicate P>
		bool runUntil( const P & p_predicate, const std::chrono::milliseconds p_timeout = DEFAULT_TIMEOUT )
		{ return _app->runUntil( p_predicate, p_timeout ); }

		bool runUntilIdle( const std::chrono::milliseconds p_timeout = DEFAULT_TIMEOUT )
		{ return _app->runUntilIdle( p_timeout ); }

		template<typename T>
		bool runUntilReady(
			const std::future<T> &			p_future,
			const std::chrono::milliseconds p_timeout = DEFAULT_TIMEOUT
		)
		{ return _app->runUntilReady( p_future, p_timeout ); }

		/**
		 * @brief Get app pointer.
		 */
		VTXApp * const get() { return _app.get(); }

	  private:
		/**
		 * @brief VTYX App.
		 */
		std::unique_ptr<VTXApp> _app;
	};
} // namespace VTX::App

#endif
