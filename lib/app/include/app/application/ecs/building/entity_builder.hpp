#ifndef __VTX_APP_APPLICATION_ECS_BUILDING_ENTITY_BUILDER__
#define __VTX_APP_APPLICATION_ECS_BUILDING_ENTITY_BUILDER__

#include "app/core/ecs/base_entity.hpp"
#include <array>
#include <functional>
#include <util/variant.hpp>
#include <vector>

namespace VTX::App::Application::ECS::Building
{
	enum class PASS_ENUM : int
	{
		ADD_COMPONENT = 0,
		SETUP,
		POST_SETUP,

		COUNT
	};

	using Step			   = std::function<void( const Core::ECS::BaseEntity, const Util::VariantMap & )>;
	using BuildPass		   = std::vector<Step>;
	using BuildInstruction = std::array<BuildPass, int( PASS_ENUM::COUNT )>;

	class EntityBuilder
	{
	  public:
		EntityBuilder() {}
		EntityBuilder( const BuildInstruction & p_instruction ) : _instruction( p_instruction ) {}

		Util::VariantMap &		 getData() { return _extraData; }
		const Util::VariantMap & getData() const { return _extraData; }

		const Core::ECS::BaseEntity getEntity() const { return _entity; }

		void build();

	  protected:
		Util::VariantMap	  _extraData   = Util::VariantMap();
		BuildInstruction	  _instruction = BuildInstruction();
		Core::ECS::BaseEntity _entity	   = Core::ECS::INVALID_ENTITY;
	};

} // namespace VTX::App::Application::ECS::Building
#endif
