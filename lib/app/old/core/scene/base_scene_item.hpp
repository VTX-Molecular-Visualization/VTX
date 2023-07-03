#ifndef __VTX_APP_OLD_CORE_SCENE_BASE_SCENE_ITEM__
#define __VTX_APP_OLD_CORE_SCENE_BASE_SCENE_ITEM__

#include "app/old/core/model/base_model.hpp"

namespace VTX::App::Old::Core::Scene
{
	class BaseSceneItem
	{
	  public:
		virtual ~BaseSceneItem() = default;

		virtual const App::Old::Core::Model::ID & getModelID() const = 0;

		inline int	getPersistentSceneID() const { return _persistentId; }
		inline void setPersistentSceneID( const int p_id ) { _persistentId = p_id; }
		inline bool hasPersistentSceneID() const { return _persistentId != -1; }

	  private:
		int _persistentId = -1;
	};
} // namespace VTX::App::Old::Core::Scene
#endif
