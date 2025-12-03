#ifndef __VTX_APP_SETTINGS_MANAGER__
#define __VTX_APP_SETTINGS_MANAGER__

#include <memory>
#include <optional>
#include <string>
#include <util/collection.hpp>

namespace VTX::App::Settings
{
	/**
	 * @brief Interface for polymorphism.
	 */
	struct ISetting
	{
		virtual ~ISetting()	 = default;
		virtual void reset() = 0;
	};

	/**
	 * @brief Setting structure.
	 */
	template<typename T>
	struct Setting : public ISetting
	{
		T				 value;
		T				 defaultValue;
		std::optional<T> min;
		std::optional<T> max;
		void			 reset() override { value = defaultValue; }
	};

	template<typename T>
	using SettingRef = const T &;

	template<typename T>
	using SettingPtr = const T * const;

	/**
	 * @brief Manage all application settings (except UI).
	 */
	class SettingsManager
	{
	  public:
		/**
		 * @brief Constructors.
		 */
		SettingsManager()					  = default;
		SettingsManager( SettingsManager && ) = default;

		/**
		 * @brief Register a new setting.
		 */
		template<typename T>
		void add(
			const std::string_view	 p_key,
			const T &				 p_defaultValue,
			const std::optional<T> & p_min = std::nullopt,
			const std::optional<T> & p_max = std::nullopt
		)
		{
			Hash hash = Util::hash( p_key );

			assert( not _settings.has( hash ) );

			auto * setting		  = _settings.createWithHash<Setting<T>>( hash );
			setting->value		  = p_defaultValue;
			setting->defaultValue = p_defaultValue;
			setting->min		  = p_min;
			setting->max		  = p_max;
		}

		/**
		 * @brief Register a new setting from object.
		 */
		template<typename T>
		void add( const std::string_view p_key, const Setting<T> & p_setting )
		{
			Hash hash = Util::hash( p_key );

			assert( not _settings.has( hash ) );

			auto * setting = _settings.createWithHash<Setting<T>>( hash );
			*setting	   = p_setting;
		}

		/**
		 * @brief Register a new setting with default value.
		 */
		template<typename T>
		void add( const std::string_view p_key )
		{
			Hash hash = Util::hash( p_key );

			assert( not _settings.has( hash ) );

			_settings.createWithHash<Setting<T>>( hash );
		}

		/**
		 * @brief Get a setting by its key.
		 */
		template<typename T>
		const Setting<T> * const get( const std::string_view p_key ) const
		{
			Hash hash = Util::hash( p_key );

			assert( _settings.has( hash ) );

			return _settings.get<Setting<T>>( hash );
		}

		/**
		 * @brief Get the value of a setting by its key.
		 */
		template<typename T>
		const T & getValue( const std::string_view p_key ) const
		{
			assert( _settings.has( Util::hash( p_key ) ) );

			return get<T>( p_key )->value;
		}

		/**
		 * @brief Get the value ptr of a setting by its key.
		 */
		template<typename T>
		const T * const getValuePtr( const std::string_view p_key ) const
		{
			assert( _settings.has( Util::hash( p_key ) ) );

			return &( get<T>( p_key )->value );
		}

		/**
		 * @brief Set the value of a setting by its key.
		 */
		template<typename T>
		void setValue( const std::string_view p_key, const T & p_value )
		{
			Hash hash = Util::hash( p_key );

			assert( _settings.has( hash ) );

			const Setting<T> & setting	= *get<T>( p_key );
			const T			   oldValue = setting.value;
			T				   value	= p_value;

			// If type comparable, clamp value to min/max.
			if constexpr ( std::totally_ordered<T> )
			{
				if ( setting.min.has_value() )
				{
					value = std::max( value, setting.min.value() );
				}

				if ( setting.max.has_value() )
				{
					value = std::min( value, setting.max.value() );
				}
			}

			_settings.get<Setting<T>>( hash )->value = value;
			// TODO: notify change.
		}

		/**
		 * @brief Reset all settings to their default value.
		 */
		void reset()
		{
			for ( const auto & baseSetting : _settings )
			{
				baseSetting.second->reset();
			}
		}

		/**
		 * @brief Reset a setting to its default value.
		 */
		void reset( const std::string_view p_key )
		{
			Hash hash = Util::hash( p_key );

			assert( _settings.has( hash ) );

			auto * baseSetting = _settings.get<ISetting>( hash );
			baseSetting->reset();
		}

	  private:
		/**
		 * @brief Store all settings.
		 */
		Util::Collection<std::unique_ptr<ISetting>> _settings;
	};

} // namespace VTX::App::Settings

#endif
