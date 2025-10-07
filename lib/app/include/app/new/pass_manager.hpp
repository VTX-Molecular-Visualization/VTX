#ifndef __VTX_APP_PASS_MANAGER__
#define __VTX_APP_PASS_MANAGER__

#include <concepts>
#include <util/collection.hpp>
#include <util/event_hub.hpp>

namespace VTX::App
{
	/**
	 * @brief Defines the update function.
	 */
	template<typename T>
	concept ConceptPass = requires( T & p_pass, const float p_delta, const float p_elapsed ) {
		{ p_pass.update( p_delta, p_elapsed ) } -> std::same_as<void>;
	};

	/**
	 * @brief For static polymorphism.
	 */
	struct IPass
	{
		virtual ~IPass() = default;
	};

	/**
	 * @brief Handles classes that process data each frame.
	 */
	class PassManager
	{
	  public:
		using UpdateFunctionArgs = void( float, float );
		using UpdateDelegate	 = Util::EventHub::Delegate<UpdateFunctionArgs>;

		PassManager()				  = default;
		PassManager( PassManager && ) = default;

		/**
		 * @brief Instantiates a pass.
		 */
		template<ConceptPass T, typename... Args>
		T * const addPass( Args &&... args )
		{
			assert( not _passes.has<T>() );

			// Create pass.
			T * const p = _passes.create<T>( std::forward<Args>( args )... );
			// Register update delegate.
			UpdateDelegate d;
			d.template connect<&T::update>( p );
			_delegates.push_back( std::move( d ) );

			return p;
		}

		/**
		 * @brief Removes and deletes a pass.
		 */
		template<ConceptPass T>
		void removePass()
		{
			assert( _passes.has<T>() );

			// Remove update delegate.
			std::erase_if(
				_delegates, [ this ]( const UpdateDelegate & p_d ) { return p_d.target() == _passes.get<T>(); }
			);
			// Delete pass.
			_passes.remove<T>();
		}

		/**
		 * @brief Updates all passes.
		 */
		void update( const float p_delta, const float p_elapsed )
		{
			for ( const auto & delegate : _delegates )
			{
				delegate( p_delta, p_elapsed );
			}
		}

	  private:
		Util::Collection<std::unique_ptr<IPass>> _passes;
		std::vector<UpdateDelegate>				 _delegates;
	};
} // namespace VTX::App

#endif
