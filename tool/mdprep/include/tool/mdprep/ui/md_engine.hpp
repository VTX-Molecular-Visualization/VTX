#ifndef VTX_TOOL_MDPREP_UI_MD_ENGINE_HPP
#define VTX_TOOL_MDPREP_UI_MD_ENGINE_HPP

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
	enum class E_FIELD_SECTION : uint8_t;

	class MdEngine;
	void get( const MdEngine & p_engine, Gateway::EngineSpecificCommonInformation & p_info );

	// Class responsible for framing md engine specifcities
	class MdEngine
	{
	  public:
		MdEngine() = default;

		void get( MdEngineFieldPlacer & p_out )
		{
			if ( _ptr )
			{
				_ptr->_get( p_out );
			}
		}

		void get( const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out )
		{
			if ( _ptr )
			{
				_ptr->_get( p_section, p_out );
			}
		}

		void get( Gateway::EngineJobManager & p_out )
		{
			if ( _ptr )
			{
				_ptr->_get( p_out );
			}
		}

		friend void get( const MdEngine & p_engine, Gateway::EngineSpecificCommonInformation & p_info );

	  private:
		struct _interface
		{
			virtual ~_interface()														= default;
			virtual void _get( Gateway::EngineSpecificCommonInformation & ) const		= 0;
			virtual void _get( const E_FIELD_SECTION &, MdEngineSpecificFieldPlacer & ) = 0;
			virtual void _get( MdEngineFieldPlacer & p_out )							= 0;
			virtual void _get( Gateway::EngineJobManager & p_out )						= 0;
		};

		std::unique_ptr<_interface> _ptr = nullptr;

		template<typename T>
		struct _wrapper : public _interface
		{
			T _obj;

			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}

			void _get( Gateway::EngineSpecificCommonInformation & p_ ) const override { ui::get( _obj, p_ ); }

			void _get( const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) override
			{ _obj.get( p_section, p_out ); }

			void _get( MdEngineFieldPlacer & p_out ) override { _obj.get( p_out ); }

			void _get( Gateway::EngineJobManager & p_out ) override { _obj.get( p_out ); }
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
				"You need to implement 'void get( MdEngineFieldPlacer & p_out )' "
				"class method."
			);
			static_assert(
				requires( T t, const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) {
					{ t.get( p_section, p_out ) };
				},
				"You need to implement 'void get( const E_FIELD_SECTION &, MdEngineSpecificFieldPlacer & )' "
				"class method."
			);
			static_assert(
				requires( T t, Gateway::EngineJobManager & p_out ) {
					{ t.get( p_out ) };
				},
				"You need to implement 'void get( Gateway::EngineJobManager & p_out )' "
				"class method. Resulting object should need access to engine specific data implicitly."
			);
		}
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
