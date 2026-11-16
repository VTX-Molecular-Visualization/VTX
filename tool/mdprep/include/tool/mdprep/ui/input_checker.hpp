#ifndef VTX_TOOL_MDPREP_UI_INPUT_CHECKER_HPP
#define VTX_TOOL_MDPREP_UI_INPUT_CHECKER_HPP

#include "tool/mdprep/gateway/shared.hpp"
#include <memory>
#include <util/concepts.hpp>

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	// Class responsible for checking inputs synchonously and providing feedback eventually
	class InputChecker
	{
	  public:
		InputChecker() = default;

		void checkInputs( const Gateway::MdParameters & p_1 ) const
		{
			if ( _ptr )
			{
				_ptr->checkInputs( p_1 );
			}
		}

		bool isResultAvailable() const noexcept
		{
			if ( _ptr )
			{
				return _ptr->isResultAvailable();
			}
			return false;
		}

		Gateway::CheckReport lastResult() const
		{
			if ( _ptr )
			{
				return _ptr->lastResult();
			}
			return Gateway::CheckReport();
		}

	  private:
		struct _interface
		{
			virtual ~_interface() = default;

			virtual void checkInputs( const Gateway::MdParameters & p_1 ) const = 0;

			virtual bool				 isResultAvailable() const noexcept = 0;
			virtual Gateway::CheckReport lastResult() const					= 0;
		};

		std::unique_ptr<_interface> _ptr = nullptr;

		template<typename T>
		struct _wrapper final : public _interface
		{
			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}

			void checkInputs( const Gateway::MdParameters & p_1 ) const override { _obj.checkInputs( p_1 ); }

			bool isResultAvailable() const noexcept override { return _obj.isResultAvailable(); }

			Gateway::CheckReport lastResult() const override { return _obj.lastResult(); }

		  private:
			T _obj;
		};

	  public:
		template<typename T>
			requires( not VTX::SameUnalteredType<InputChecker, T> )
		InputChecker( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
			static_assert(
				requires( T t, const Gateway::MdParameters & p_1 ) {
					{ t.checkInputs( p_1 ) };
				},
				"You must implement 'void checkInputs( const Gateway::MdParameters & p_1, Gateway::CheckReportCallback "
				"p_2 ) const noexcept' class method." );
			static_assert(
				requires( T t ) {
					{ t.isResultAvailable() } -> std::same_as<bool>;
				}, "You must implement 'bool isResultAvailable() const noexcept' class method." );
			static_assert(
				requires( T t ) {
					{ t.lastResult() } -> std::same_as<Gateway::CheckReport>;
				}, "You must implement 'Gateway::CheckReport lastResult() const' class method." );
		}
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
