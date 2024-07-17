#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_INPUT_CHECKER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_INPUT_CHECKER__

#include "tools/mdprep/gateway/shared.hpp"
#include <memory>
#include <util/concepts.hpp>

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	// Class responsible for checking inputs asynchonously and providing feedback eventually
	class InputChecker
	{
	  public:
		InputChecker() = default;
		template<typename T>
			requires( not VTX::SameUnalteredType<InputChecker, T> )
		InputChecker( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
			static_assert(
				requires( T t, const Gateway::MdParameters & p_1, Gateway::CheckReportCallback p_2 ) {
					{ t.checkInputs( p_1, p_2 ) };
				},
				"You must implement 'void checkInputs( const Gateway::MdParameters & p_1, Gateway::CheckReportCallback "
				"p_2 ) const noexcept' class method."
			);
		}

		inline void checkInputs( const Gateway::MdParameters & p_1, Gateway::CheckReportCallback p_2 ) const noexcept
		{
			if ( _ptr )
				_ptr->checkInputs( p_1, std::move( p_2 ) );
		}

	  private:
		struct _interface
		{
			virtual ~_interface() = default;

			virtual void checkInputs( const Gateway::MdParameters & p_1, Gateway::CheckReportCallback p_2 )
				const noexcept
				= 0;
		};
		std::unique_ptr<_interface> _ptr = nullptr;

		template<typename T>
		struct _wrapper final : public _interface
		{
			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}

			virtual void checkInputs( const Gateway::MdParameters & p_1, Gateway::CheckReportCallback p_2 )
				const noexcept override
			{
				_obj.checkInputs( p_1, std::move( p_2 ) );
			}

		  private:
			T _obj;
		};
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
