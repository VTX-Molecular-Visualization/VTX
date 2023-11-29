#ifndef __VTX_APP_CORE_SERIALIZATION_UPGRADE_UTILITY__
#define __VTX_APP_CORE_SERIALIZATION_UPGRADE_UTILITY__

#include <string>
#include <util/json/json.hpp>

namespace VTX::App::Core::Serialization::UpgradeUtility
{
	using JSonObject = ::VTX::Util::JSon::Object;

	void renameField( JSonObject & p_jsonObj, const std::string & p_oldName, const std::string & p_newName );

} // namespace VTX::App::Core::Serialization::UpgradeUtility

#endif
