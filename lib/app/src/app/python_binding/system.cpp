#include "app/python_binding/system.hpp"
#include "app/helper/system.hpp"
#include <python_binding/binding/entity_caster.hpp>

namespace VTX::App::PythonBinding
{
	void SystemBinder::bind( Module & p_vtxModule )
	{
		p_vtxModule.def(
			"getSystemIdByName",
			&Helper::System::getSystemByName,
			"Return a system ID that matches given name (case insensitive)."
		);
		p_vtxModule.def(
			"getSystemIdByPdb",
			&Helper::System::getSystemByPdb,
			"Return a system ID that matches given RCSB PDB ID code (case insensitive)."
		);
		p_vtxModule.def(
			"getSystemIdByFileName",
			&Helper::System::getSystemByFileName,
			"Return a system ID that matches given file name (case insensitive)."
		);
	}
} // namespace VTX::App::PythonBinding
