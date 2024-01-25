#include "app/core/serialization/upgrade_utility.hpp"

namespace VTX::App::Core::Serialization::UpgradeUtility
{
	void renameField( JSonObject & p_jsonObj, const std::string & p_oldName, const std::string & p_newName )
	{
		if ( p_jsonObj.contains( p_oldName ) )
			p_jsonObj.appendField( p_newName, p_jsonObj[ p_oldName ] );
	}

} // namespace VTX::App::Core::Serialization::UpgradeUtility
