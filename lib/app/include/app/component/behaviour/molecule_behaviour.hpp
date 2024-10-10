#ifndef __VTX_APP_COMPONENT_BEHAVIOUR_MOLECULE_BEHAVIOUR__
#define __VTX_APP_COMPONENT_BEHAVIOUR_MOLECULE_BEHAVIOUR__

#include "app/application/selection/picking_info.hpp"
#include "app/application/selection/selection_data.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/component/chemistry/_fwd.hpp"
#include "app/core/ecs/base_component.hpp"
#include "app/core/ecs/base_entity.hpp"
#include <memory>
#include <util/variant.hpp>

namespace VTX::App::Component::Behaviour
{
	class Molecule : public Core::ECS::BaseComponent
	{
	  private:
		// Register component class as component
		inline static const Application::System::ECSSystem::ComponentStaticIDRegistration<Molecule> _compReg {
			"Behaviour::Molecule"
		};

	  public:
		Molecule();
		~Molecule() = default;

		void init( const FilePath & p_path, const std::string * const p_buffer );

	  private:
		void _initSelectableComponent() const;
		void _initUIDComponent() const;
		void _initAABBComponent() const;
		void _initGpuProxyComponent() const;
		void _initTrajectoryComponent() const;
		void _initPickableComponent() const;

		void _loadMolecule( const FilePath & p_path, const std::string * const p_buffer ) const;
		std::unique_ptr<Application::Selection::SelectionData> _moleculePickingFunction( const Application::Selection::
																							 PickingInfo & ) const;

		Util::Math::AABB _computeMoleculeAABB() const;

		const Core::ECS::BaseEntity		 _entity;
		Component::Chemistry::Molecule & _moleculeComponent;
	};

} // namespace VTX::App::Component::Behaviour

#endif
