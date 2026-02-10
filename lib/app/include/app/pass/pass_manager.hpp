#ifndef __VTX_APP_PASS_MANAGER__
#define __VTX_APP_PASS_MANAGER__

#include <concepts>
#include <forward_list>
#include <util/collection.hpp>
#include <util/event_hub.hpp>

namespace VTX::App::Pass
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

		/**
		 * @brief Some useful flags.
		 */
		bool deleted = false;
		bool paused	 = false;
	};

	/**
	 * @brief Handles classes that process data each frame.
	 */
	class PassManager
	{
	  public:
		using UpdateDelegate = Util::EventHub::Delegate<void( float, float )>;

		PassManager()				  = default;
		PassManager( PassManager && ) = default;

		/**
		 * @brief Instantiates a pass.
		 */
		template<ConceptPass T, typename... Args>
		T * const addPass( Args &&... p_args )
		{
			assert( not _passes.has<T>() );

			// Create pass.
			T * const p = _passes.create<T>( std::forward<Args>( p_args )... );
			// Register update delegate.
			UpdateDelegate d;
			d.template connect<&T::update>( p );
			_delegates.push_front( std::move( d ) );

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
				_delegates, [ this ]( const UpdateDelegate & p_d ) { return p_d.data() == _passes.get<T>(); }
			);

			// Delete pass.
			_passes.remove<T>();
		}

		template<ConceptPass T>
		void tryRemovePass()
		{
			if ( _passes.has<T>() )
			{
				removePass<T>();
			}
		}

		/**
		 * @brief Checks if a pass exists.
		 */
		template<ConceptPass T>
		bool hasPass() const
		{
			return _passes.has<T>();
		}

		/**
		 * @brief Gets a pass.
		 */
		template<ConceptPass T>
		T * const getPass() const
		{
			assert( _passes.has<T>() );
			return _passes.get<T>();
		}

		template<ConceptPass T>
		T * const tryGetPass() const
		{
			return _passes.tryGet<T>();
		}

		/**
		 * @brief Updates all passes.
		 */
		inline void update( const float p_delta, const float p_elapsed )
		{
			// Update delegates.
			for ( const auto & delegate : _delegates )
			{
				// Check if pass is paused.
				const IPass * pass = static_cast<const IPass *>( delegate.data() );
				if ( pass->paused )
				{
					continue;
				}

				delegate( p_delta, p_elapsed );
			}

			// Remove passes flagged for deletion.
			for ( auto it = _passes.begin(); it != _passes.end(); )
			{
				if ( it->second->deleted )
				{
					it = _passes.erase( it );
				}
				else
				{
					++it;
				}
			}

			std::erase_if(
				_delegates, []( const UpdateDelegate & d ) { return static_cast<const IPass *>( d.data() )->deleted; }
			);
		}

	  private:
		Util::Collection<std::unique_ptr<IPass>> _passes;
		std::forward_list<UpdateDelegate>
			_delegates; // delegates collection needs to be a forward list so insertion/removal of element from it
						// doesn't invalidate pointers.
	};
} // namespace VTX::App::Pass

#endif
