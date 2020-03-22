#ifndef __VTX_RENDERER_RAY_TRACER_MOLECULE_BALL_AND_STICK__
#define __VTX_RENDERER_RAY_TRACER_MOLECULE_BALL_AND_STICK__

#ifdef _MSC_VER
#pragma once
#endif

#include "../bvh.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Renderer
	{
		class MoleculeBallAndStick : public BaseObject3D
		{
		  public:
			MoleculeBallAndStick( Model::Molecule * p_molecule );
			~MoleculeBallAndStick()
			{
				for ( Renderer::BaseMaterial * mtl : _materials )
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
				if ( _bvh.isBuilt() ) _aabb = _bvh.getAABB();
			}

		  private:
			BVH									  _bvh;
			std::vector<Renderer::BaseMaterial *> _materials;
		};
	} // namespace Renderer
} // namespace VTX
#endif
