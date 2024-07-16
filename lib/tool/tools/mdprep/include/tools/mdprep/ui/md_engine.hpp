#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE__

#include <memory>
#include <util/concepts.hpp>

namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonInformation;
}
namespace VTX::Tool::Mdprep::ui
{

	class MdEngineFieldPlacer;
	class MdEngineSpecificFieldPlacer;
	enum class E_FIELD_SECTION;

	template<typename T>
	concept InfoProvider = requires( const T & t, Gateway::EngineSpecificCommonInformation & i ) {
		{ get( t, i ) };
	};
	template<typename T>
	concept FieldProvider = requires( T t, MdEngineFieldPlacer & p_out ) {
		{ t.get( p_out ) };
	};
	template<typename T>
	concept SpecificFieldProvider
		= requires( T t, const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) {
			  { t.get( p_section, p_out ) };
		  };

	// Class responsible for framing md engine specifcities
	class MdEngine
	{
	  public:
		template<typename T>
			requires( not VTX::SameUnalteredType<MdEngine, T> )
		MdEngine( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
			static_assert(
				InfoProvider<T>,
				"You need to implement 'void get( const MdEngine &, Gateway::EngineSpecificCommonInformation & )' "
				"function."
			);
			static_assert(
				FieldProvider<T>,
				"You need to implement 'void get( MdEngineFieldPlacer & p_out ) noexcept' "
				"class method."
			);
			static_assert(
				SpecificFieldProvider<T>,
				"You need to implement 'void get( const E_FIELD_SECTION &, MdEngineSpecificFieldPlacer & ) noexcept' "
				"class method."
			);
		}

		inline void get( MdEngineFieldPlacer & p_out ) noexcept
		{
			if ( _ptr )
				_ptr->_get( p_out );
		}
		inline void get( const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) noexcept
		{
			if ( _ptr )
				_ptr->_get( p_section, p_out );
		}

		friend inline void get( const MdEngine & p_engine, Gateway::EngineSpecificCommonInformation & p_info ) noexcept
		{
			if ( p_engine._ptr )
				p_engine._ptr->_get( p_info );
		}

	  private:
		struct _interface
		{
			virtual void _get( Gateway::EngineSpecificCommonInformation & ) const noexcept		 = 0;
			virtual void _get( const E_FIELD_SECTION &, MdEngineSpecificFieldPlacer & ) noexcept = 0;
			virtual void _get( MdEngineFieldPlacer & p_out ) noexcept							 = 0;
		};

		std::unique_ptr<_interface> _ptr = nullptr;

		template<typename T>
		struct _wrapper : public _interface
		{
			T _obj;
			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}
			virtual void _get( Gateway::EngineSpecificCommonInformation & p_ ) const noexcept override
			{
				ui::get( _obj, p_ );
			}
			virtual void _get( const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) noexcept
				override
			{
				_obj.get( p_section, p_out );
			}
			virtual void _get( MdEngineFieldPlacer & p_out ) noexcept override { _obj.get( p_out ); }
		};
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
