#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_ENGINE_JOB_MANAGER__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_ENGINE_JOB_MANAGER__

#include "tool/mdprep/gateway/shared.hpp"
#include <functional>
#include <memory>
#include <util/concepts.hpp>
namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;

	// Class responsible for taking care of the task related to an abstract MD engine from the UI perspective
	class EngineJobManager
	{
	  public:
		EngineJobManager() = default;
		template<typename T>
			requires( not VTX::SameUnalteredType<EngineJobManager, T> )
		EngineJobManager( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
			static_assert(
				requires( T t, const Gateway::MdParameters & p_1, Gateway::CheckReportCallback p_2 ) {
					{ t.checkInputs( p_1, p_2 ) };
				},
				"You must implement 'void checkInputs( const Gateway::MdParameters & p_1, Gateway::CheckReportCallback "
				"p_2 ) const noexcept' class method."
			);
			static_assert(
				requires( T t, const MdParameters & p_1, JobUpdateCallback p_2 ) {
					{ t.startPreparation( p_1, p_2 ) };
				},
				"You must implement 'void startPreparation( const MdParameters & p_1, JobUpdateCallback p_2 ) "
				"noexcept' class method."
			);
			static_assert(
				requires( T t ) {
					{ t.isResultAvailable() } -> std::same_as<bool>;
				}, "You must implement 'bool isResultAvailable() const noexcept' class method."
			);
			static_assert(
				requires( T t ) {
					{ t.lastResult() } -> std::same_as<Gateway::CheckReport>;
				}, "You must implement 'Gateway::CheckReport lastResult() const noexcept' class method."
			);
		}

		// Check weither the current inputs are reasonnable and send results via callback
		inline void checkInputs( const MdParameters & p_1, CheckReportCallback p_2 ) noexcept
		{
			if ( _ptr )
				_ptr->checkInputs( p_1, std::move( p_2 ) );
		}

		/**
		 * @brief Returns the last checkInputs result. Doesn't restart a check.
		 * @return Returns default constructed Gateway::CheckReport if no results are available
		 */
		inline Gateway::CheckReport lastResult() const noexcept
		{
			if ( _ptr )
				return _ptr->lastResult();
			return Gateway::CheckReport();
		}

		/**
		 * @brief Tells if a check result is available.
		 * @return true if there is.
		 */
		inline bool isResultAvailable() const noexcept
		{
			if ( _ptr )
				return _ptr->isResultAvailable();
			return false;
		}

		// Start preparation of the system and feed the callback with job progression
		inline void startPreparation( const MdParameters & p_1, JobUpdateCallback p_2 ) noexcept
		{
			if ( _ptr )
				_ptr->startPreparation( p_1, std::move( p_2 ) );
		}

	  private:
		struct _interface
		{
			~_interface()																			  = default;
			virtual void checkInputs( const MdParameters & p_1, CheckReportCallback p_3 ) noexcept	  = 0;
			virtual void startPreparation( const MdParameters & p_1, JobUpdateCallback p_3 ) noexcept = 0;

			virtual bool				 isResultAvailable() const noexcept = 0;
			virtual Gateway::CheckReport lastResult() const noexcept		= 0;
		};
		template<typename T>
		struct _wrapper final : public _interface
		{
			T _obj;

			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}
			virtual void checkInputs( const MdParameters & p_1, CheckReportCallback p_2 ) noexcept override
			{
				_obj.checkInputs( p_1, std::move( p_2 ) );
			}
			virtual void startPreparation( const MdParameters & p_1, JobUpdateCallback p_2 ) noexcept override
			{
				_obj.startPreparation( p_1, std::move( p_2 ) );
			}
			virtual bool isResultAvailable() const noexcept override { return _obj.isResultAvailable(); }
			virtual Gateway::CheckReport lastResult() const noexcept override { return _obj.lastResult(); }
		};
		std::unique_ptr<_interface> _ptr = nullptr;
	};
} // namespace VTX::Tool::Mdprep::Gateway

#endif
