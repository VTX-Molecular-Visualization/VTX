#ifndef __VTX_APP_HELPER_CHEMISTRY__
#define __VTX_APP_HELPER_CHEMISTRY__

#include "app/component/chemistry/_fwd.hpp"
#include <string>
#include <vector>

namespace VTX::App::Helper::Chemistry
{
	using namespace App::Component::Chemistry;

	std::vector<Chain *> findChainsByName( const Molecule & p_molecule, const std::string & p_chainName );
} // namespace VTX::App::Helper::Chemistry

#endif
