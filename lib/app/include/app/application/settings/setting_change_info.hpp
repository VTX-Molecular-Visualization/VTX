#ifndef __VTX_APP_APPLICATION_SETTINGS_SETTINGS_CHANGE_EVENT_INFO__
#define __VTX_APP_APPLICATION_SETTINGS_SETTINGS_CHANGE_EVENT_INFO__

#include <string>

namespace VTX::App::Application::Settings
{
	class BaseSettingChangeInfo
	{
	  public:
		BaseSettingChangeInfo( const std::string & p_key ) : key( p_key ) {};
		const std::string key;
	};

	template<typename T>
	class SettingChangeInfo : public BaseSettingChangeInfo
	{
	  public:
		SettingChangeInfo( const std::string & p_key, const T & p_oldValue, const T & p_newValue ) :
			BaseSettingChangeInfo( p_key ), oldValue( p_oldValue ), newValue( p_newValue ) {};

		const T & oldValue;
		const T & newValue;
	};

} // namespace VTX::App::Application::Settings

#endif
