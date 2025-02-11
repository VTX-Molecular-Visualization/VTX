#ifndef __VTX_APP_ENTITY_APPLICATION_SCENE_SYSTEM_ENTITY__
#define __VTX_APP_ENTITY_APPLICATION_SCENE_SYSTEM_ENTITY__

#include "app/component/chemistry/_fwd.hpp"
#include "app/core/ecs/base_entity.hpp"
#include <util/variant.hpp>

namespace VTX::App::Entity
{
	class System : public Core::ECS::BaseEntity
	{
	  public:
		System( const FilePath & p_path, const std::string * const p_buffer = nullptr ) :
			_path( p_path ), _buffer( p_buffer )
		{
		}

		void setup() override;

	  private:
		const FilePath			  _path;
		const std::string * const _buffer;
	};
} // namespace VTX::App::Entity

#endif
