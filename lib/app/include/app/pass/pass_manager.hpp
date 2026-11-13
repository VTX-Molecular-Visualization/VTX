#ifndef VTX_APP_PASS_PASS_MANAGER_HPP
#define VTX_APP_PASS_PASS_MANAGER_HPP

#include "app/pending_updates.hpp"
#include <concepts>
#include <forward_list>
#include <util/collection.hpp>
#include <util/event_hub.hpp>
#include <util/exceptions.hpp>

namespace VTX::App::Pass
{
	/**
	 * @brief For polymorphism.
	 */
	struct IPass
	{
		/**
		 * @brief Constructor.
		 */
		virtual ~IPass() = default;
	};

	/**
	 * @brief Defines the update function.
	 */
	template<typename T>
	concept ConceptPass
		= std::derived_from<T, IPass> && requires( T & p_pass, const float p_delta, const float p_elapsed ) {
			  { p_pass.update( p_delta, p_elapsed ) } -> std::same_as<void>;
			  { T::UPDATE_TRIGGERS } -> std::convertible_to<E_PENDING_UPDATE>;
		  };

	/**
	 * @brief Handles classes that process data each frame.
	 */
	class PassManager
	{
	  public:
		/**
		 * @brief Update function.
		 */
		using UpdateDelegate = Util::EventHub::Delegate<void( const float, const float )>;

		/**
		 * @brief Constructors.
		 */
		PassManager()								   = default;
		PassManager( const PassManager & )			   = delete;
		PassManager & operator=( const PassManager & ) = delete;
		PassManager( PassManager && )				   = delete;
		PassManager & operator=( PassManager && )	   = delete;
		~PassManager()								   = default;

		/**
		 * @brief Instantiates a pass.
		 */
		template<ConceptPass T, typename... Args>
		T * addPass( Args &&... p_args )
		{
			assert( not _passes.has<T>() );

			// Create pass.
			T * const p = _passes.create<T>( std::forward<Args>( p_args )... );
			// Register update delegate.
			UpdateDelegate d;
			d.template connect<&T::update>( p );
			_delegates.push_back( { d, T::UPDATE_TRIGGERS } );

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
				_delegates, [ this ]( const Entry & p_entry ) { return p_entry.delegate.data() == _passes.get<T>(); }
			);

			// Delete pass.
			_passes.remove<T>();
		}

		/**
		 * @brief Removes pass if exist.
		 */
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
		{ return _passes.has<T>(); }

		/**
		 * @brief Updates all passes.
		 */
		void update( const float p_delta, const float p_elapsed )
		{ update( E_PENDING_UPDATE::ALL, p_delta, p_elapsed ); }

		/**
		 * @brief Updates passes based on pending updates.
		 */
		void update( const E_PENDING_UPDATE p_pending, const float p_delta, const float p_elapsed )
		{
			Util::ExceptionCollector errors;

			// Update delegates.
			for ( const auto & entry : _delegates )
			{
				if ( not Util::Enum::hasAnyBit( p_pending, entry.triggers ) )
				{
					continue;
				}

				errors.run( "Pass update failed", [ & ]() { entry.delegate( p_delta, p_elapsed ); } );
			}

			errors.rethrow();
		}

	  private:
		/**
		 * @brief Pass entry.
		 */
		struct Entry
		{
			UpdateDelegate	 delegate;
			E_PENDING_UPDATE triggers;
		};

		/**
		 * @brief All passes.
		 */
		Util::Collection<std::unique_ptr<IPass>> _passes;

		/**
		 * @brief All update delegates.
		 */
		std::vector<Entry> _delegates;
	};
} // namespace VTX::App::Pass

#endif
