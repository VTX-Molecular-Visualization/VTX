#ifndef __VTX_APP_COMPONENT_IO_MOLECULE_METADATA__
#define __VTX_APP_COMPONENT_IO_MOLECULE_METADATA__

#include "app/application/ecs/component_registration.hpp"
#include <set>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Component::IO
{
	struct MoleculeMetadata
	{
	  private:
		inline static Application::ECS::Registration<MoleculeMetadata> registration { "IO::MoleculeMetadataComponent" };

	  public:
		FilePath	path				= FilePath();
		std::string pdbIDCode			= "0000";
		bool		structureHasChanged = false;

		std::set<uint>		  solventAtomIds					 = std::set<uint>();
		std::set<uint>		  ionAtomIds						 = std::set<uint>();
		std::set<std::string> solventResidueSymbols				 = std::set<std::string>();
		std::set<std::string> ionResidueSymbols					 = std::set<std::string>();
		bool				  isSecondaryStructureLoadedFromFile = false;
	};
} // namespace VTX::App::Component::IO
#endif
