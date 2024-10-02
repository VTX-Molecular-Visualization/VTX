#ifndef __VTX_APP_COMPONENT_IO_MOLECULE_METADATA__
#define __VTX_APP_COMPONENT_IO_MOLECULE_METADATA__

#include "app/application/system/ecs_system.hpp"
#include <set>
#include <string>
#include <util/generic/base_serializable.hpp>
#include <util/serializer.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::IO
{
	class MoleculeMetadata : public Util::Generic::BaseSerializable
	{
	  private:
		inline static Application::System::ECSSystem::ComponentStaticIDRegistration<MoleculeMetadata> registration {
			"IO::MoleculeMetadataComponent"
		};

	  public:
		FilePath	path				= FilePath();
		std::string pdbIDCode			= "0000";
		bool		structureHasChanged = false;

		std::set<uint>		  solventAtomIds					 = std::set<uint>();
		std::set<uint>		  ionAtomIds						 = std::set<uint>();
		std::set<std::string> solventResidueSymbols				 = std::set<std::string>();
		std::set<std::string> ionResidueSymbols					 = std::set<std::string>();
		bool				  isSecondaryStructureLoadedFromFile = false;

		Util::JSon::BasicJSon serialize() const override
		{
			const Component::Chemistry::Molecule & moleculeComponent
				= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( MAIN_REGISTRY().getEntity( *this ) );

			return { { "PATH", Util::Serializer::serialize( path ) },
					 { "PDB_ID", Util::Serializer::serialize( pdbIDCode ) },
					 { "SECONDARY_STRUCTURE_FROM_FILE",
					   Util::Serializer::serialize( isSecondaryStructureLoadedFromFile ) },
					 { "VISIBILITY", serialize( moleculeComponent.getAtomVisibilities() ) } };
		}

		void deserialize( const Util::JSon::BasicJSon & p_json ) override {}
	};
} // namespace VTX::App::Component::IO
#endif
