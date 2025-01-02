#ifndef __VTX_APP_CORE_SETTINGS_BASE_SETTING__
#define __VTX_APP_CORE_SETTINGS_BASE_SETTING__

#include <concepts>
#include <memory>
#include <optional>
#include <util/json/basic_json.hpp>

namespace VTX::App::Core::Settings
{
	class BaseSetting
	{
	  public:
		virtual std::unique_ptr<BaseSetting> clone() const = 0;

		virtual Util::JSon::BasicJSon serialize() const									  = 0;
		virtual void				  deserialize( const Util::JSon::BasicJSon & p_json ) = 0;
		virtual void				  reset()											  = 0;

		virtual bool operator==( const BaseSetting & p_other ) const = 0;
		virtual bool operator!=( const BaseSetting & p_other ) const = 0;
	};

	template<typename T>
	class Setting final : public BaseSetting
	{
	  public:
		Setting() {}
		Setting( const T & p_value ) : _value( p_value ) {}
		Setting( const T & p_value, const T & p_defaultValue ) : _value( p_value ), _defaultValue( p_defaultValue ) {}

		std::unique_ptr<BaseSetting> clone() const override { return std::make_unique<Setting<T>>( *this ); }

		const T & get() const { return _value; }
		T &		  getRef() { return _value; }

		void set( const T & p_value ) { _value = p_value; }

		Util::JSon::BasicJSon serialize() const override
		{
			// return Core::Serialization::serialize<T>( _value );
			return {};
		}
		void deserialize( const Util::JSon::BasicJSon & p_json ) override
		{
			// Core::Serialization::deserialize<T>( p_json, _value );
		}

		void reset() override
		{
			if ( _defaultValue )
				_value = *_defaultValue;
		}

		bool operator==( const BaseSetting & p_other ) const override
		{
			const Setting<T> * const castedOther = dynamic_cast<const Setting<T> *>( &p_other );

			if ( castedOther == nullptr )
				return false;

			return _value == castedOther->_value;
		}
		bool operator!=( const BaseSetting & p_other ) const override
		{
			const Setting<T> * const castedOther = dynamic_cast<const Setting<T> *>( &p_other );

			if ( castedOther == nullptr )
				return true;

			return _value != castedOther->_value;
		}

	  private:
		T				 _value;
		std::optional<T> _defaultValue;
	};
} // namespace VTX::App::Core::Settings
#endif
