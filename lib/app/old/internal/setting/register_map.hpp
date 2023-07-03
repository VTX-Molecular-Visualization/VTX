#ifndef __VTX_APP_OLD_INTERNAL_SETTING_REGISTER_MAP__
#define __VTX_APP_OLD_INTERNAL_SETTING_REGISTER_MAP__

#include <string>

namespace VTX::App::Old::Internal::Setting
{
	class RegisterMap
	{
	  public:
		static std::string getStringValue( const std::string & p_key, const std::string & p_default = "" );
		static void		   setStringValue( const std::string & p_key, const std::string & p_value );
		static bool		   containKey( const std::string & p_key );
	};
} // namespace VTX::App::Old::Internal::Setting
#endif
