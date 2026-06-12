#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE__

#include <memory>
#include <util/concepts.hpp>

namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonInformation;
	class EngineJobManager;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{

	class MdEngineFieldPlacer;
	class MdEngineSpecificFieldPlacer;
	enum class E_FIELD_SECTION;

	class MdEngine;
	void get( const MdEngine & p_engine, Gateway::EngineSpecificCommonInformation & p_info ) noexcept;

	// Class responsible for framing md engine specifcities
	class MdEngine
	{
	  public:
		MdEngine() = default;

		inline void get( MdEngineFieldPlacer & p_out ) noexcept
		{
			if ( _ptr )
			{
				_ptr->_get( p_out );
			}
		}

		inline void get( const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) noexcept
		{
			if ( _ptr )
			{
				_ptr->_get( p_section, p_out );
			}
		}

		inline void get( Gateway::EngineJobManager & p_out ) noexcept
		{
			if ( _ptr )
			{
				_ptr->_get( p_out );
			}
		}

		friend void get( const MdEngine & p_engine, Gateway::EngineSpecificCommonInformation & p_info ) noexcept;

	  private:
		struct _interface
		{
			virtual void _get( Gateway::EngineSpecificCommonInformation & ) const noexcept		 = 0;
			virtual void _get( const E_FIELD_SECTION &, MdEngineSpecificFieldPlacer & ) noexcept = 0;
			virtual void _get( MdEngineFieldPlacer & p_out ) noexcept							 = 0;
			virtual void _get( Gateway::EngineJobManager & p_out ) noexcept						 = 0;
		};

		std::unique_ptr<_interface> _ptr = nullptr;

		template<typename T>
		struct _wrapper : public _interface
		{
			T _obj;

			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}

			virtual void _get( Gateway::EngineSpecificCommonInformation & p_ ) const noexcept override
			{ ui::get( _obj, p_ ); }

			virtual void _get(
				const E_FIELD_SECTION &		  p_section,
				MdEngineSpecificFieldPlacer & p_out
			) noexcept override
			{ _obj.get( p_section, p_out ); }

			virtual void _get( MdEngineFieldPlacer & p_out ) noexcept override { _obj.get( p_out ); }

			virtual void _get( Gateway::EngineJobManager & p_out ) noexcept override { _obj.get( p_out ); }
		};

	  public:
		template<typename T>
			requires( not VTX::SameUnalteredType<MdEngine, T> )
		MdEngine( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
			static_assert(
				requires( const T & t, Gateway::EngineSpecificCommonInformation & i ) {
					{ ui::get( t, i ) };
				},
				"You need to implement 'void get( const T &, Gateway::EngineSpecificCommonInformation & "
				")' function."
			);
			static_assert(
				requires( T t, MdEngineFieldPlacer & p_out ) {
					{ t.get( p_out ) };
				},
				"You need to implement 'void get( MdEngineFieldPlacer & p_out ) noexcept' "
				"class method."
			);
			static_assert(
				requires( T t, const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) {
					{ t.get( p_section, p_out ) };
				},
				"You need to implement 'void get( const E_FIELD_SECTION &, MdEngineSpecificFieldPlacer & ) noexcept' "
				"class method."
			);
			static_assert(
				requires( T t, Gateway::EngineJobManager & p_out ) {
					{ t.get( p_out ) };
				},
				"You need to implement 'void get( Gateway::EngineJobManager & p_out ) noexcept' "
				"class method. Resulting object should need access to engine specific data implicitly."
			);
		}
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
