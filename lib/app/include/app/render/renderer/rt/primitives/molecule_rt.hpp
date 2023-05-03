#ifndef __VTX_APP_RENDER_RENDERER_RT_MOLECULE_RT__
#define __VTX_APP_RENDER_RENDERER_RT_MOLECULE_RT__

#include "../bvh.hpp"
#include "app/component/chemistry/molecule.hpp"

namespace VTX::App::Render::Renderer::RT::Primitive
{
	class MoleculeRT : public BaseObject3D
	{
	  public:
		MoleculeRT( const App::Component::Chemistry::Molecule * p_molecule );
		~MoleculeRT()
		{
			for ( Material::BaseMaterial * mtl : _materials )
			{
				delete mtl;
			}
			_materials.clear();
		}

		bool intersect( const Ray &	   p_ray,
						const float	   p_tMin,
						const float	   p_tMax,
						Intersection & p_intersection ) const override
		{
			return _bvh.intersect( p_ray, p_tMin, p_tMax, p_intersection );
		}

		bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const override
		{
			return _bvh.intersectAny( p_ray, p_tMin, p_tMax );
		}

	  private:
		void _computeAABB() override
		{
			if ( _bvh.isBuilt() )
				_aabb = _bvh.getAABB();
		}

	  private:
		BVH									  _bvh;
		std::vector<Material::BaseMaterial *> _materials;
	};
} // namespace VTX::App::Render::Renderer::RT::Primitive
#endif
