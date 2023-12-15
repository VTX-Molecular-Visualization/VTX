#ifndef __VTX_APP_APPLICATION_SETTING__
#define __VTX_APP_APPLICATION_SETTING__

#include "app/core/serialization/serialization.hpp"
#include "app/vtx_app.hpp"
#include <optional>
#include <util/json/basic_json.hpp>

namespace VTX::App::Application
{
	class BaseSetting
	{
	  public:
		virtual Util::JSon::BasicJSon serialize() const									  = 0;
		virtual void				  deserialize( const Util::JSon::BasicJSon & p_json ) = 0;
		virtual void				  reset()											  = 0;
	};

	template<typename T>
	class Setting : public BaseSetting
	{
	  public:
		Setting() {}
		Setting( const T & p_value ) : _value( p_value ) {}
		Setting( const T & p_value, const T & p_defaultValue ) : _value( p_value ), _defaultValue( p_defaultValue ) {}

		const T & get() const { return _value; }
		T &		  getRef() { return _value; }

		void set( const T & p_value ) { _value = p_value; }

		Util::JSon::BasicJSon serialize() const override { return SERIALIZER().serialize( _value ); }
		void				  deserialize( const Util::JSon::BasicJSon & p_json ) override
		{
			SERIALIZER().deserialize( p_json, _value );
		}

		void reset() override
		{
			if ( _defaultValue )
				_value = *_defaultValue;
		}

	  private:
		T				 _value;
		std::optional<T> _defaultValue;
	};

} // namespace VTX::App::Application
#endif
