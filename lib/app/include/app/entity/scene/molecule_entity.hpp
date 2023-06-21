#ifndef __VTX_APP_ENTITY_APPLICATION_SCENE_MOLECULE_ENTITY__
#define __VTX_APP_ENTITY_APPLICATION_SCENE_MOLECULE_ENTITY__

#include "app/component/chemistry/_fwd.hpp"
#include "app/core/ecs/base_entity.hpp"
#include <util/variant.hpp>

namespace VTX::App::Entity::Scene
{
	class MoleculeEntityBuilder
	{
	  public:
		static void addComponent( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData );
		static void setup( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData );

	  private:
		static void _load( Component::Chemistry::Molecule & moleculeComponent, const VariantMap & p_extraData );
	};

} // namespace VTX::App::Entity::Scene
#endif
