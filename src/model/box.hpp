#ifndef __VTX_MODEL_BOX__
#define __VTX_MODEL_BOX__

#include "base_model_3d.hpp"
#include "buffer/box.hpp"
#include "generic/base_scene_item.hpp"

namespace VTX::Model
{
	class Box : public BaseModel3D<Buffer::Box>, public Generic::BaseSceneItem
	{
		VTX_MODEL

	  public:
		// BaseSceneItem
		const Model::ID & getModelID() const override { return getId(); }

		inline const std::vector<Vec3f> & getCorners() const { return _corners; }
		inline std::vector<Vec3f> &		  getCorners() { return _corners; }

		inline const std::vector<uint> & getIndices() const { return _indices; }
		inline std::vector<uint> &		 getIndices() { return _indices; }

		inline Math::AABB & getAABB() { return _aabb; }

		void refresh();

	  protected:
		void _fillBuffer() override;
		void _instantiate3DViews() override;

	  private:
		std::vector<Vec3f> _corners;
		std::vector<uint>  _indices;

		Box( const Math::AABB & p_aabb );
		~Box() = default;
	};
} // namespace VTX::Model

#endif
