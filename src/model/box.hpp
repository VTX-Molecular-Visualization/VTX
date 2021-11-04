#ifndef __VTX_MODEL_BOX__
#define __VTX_MODEL_BOX__

#include "base_model_3d.hpp"
#include "buffer/box.hpp"

namespace VTX::Model
{
	class Box : public BaseModel3D<Buffer::Box>
	{
		VTX_MODEL

	  public:
		inline const std::vector<Vec3f> & getCorners() const { return _corners; }
		inline std::vector<Vec3f> &		  getCorners() { return _corners; }

		inline const std::vector<uint> & getIndices() const { return _indices; }
		inline std::vector<uint> &		 getIndices() { return _indices; }

	  protected:
		void _init() override;
		void _fillBuffer() override;
		void _computeAABB() const override {}
		void _instantiate3DViews() override;

	  private:
		Math::AABB * const _aabb;
		std::vector<Vec3f> _corners;
		std::vector<uint>  _indices;

		Box()  = default;
		~Box() = default;
	};
} // namespace VTX::Model

#endif
