/*
Adapted from PBRTv3 BVHAccel : https://github.com/mmp/pbrt-v3
*/

#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_BVH__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_BVH__

#include "primitives/base_primitive.hpp"
#include <atomic>
#include <vector>

namespace VTX::App::Old::Render::Renderer::RT
{
	// Structures for BVH construction
	struct BVHBuildNode;
	struct BVHPrimInfo;
	struct MortonPrim;

	struct LBVHNode
	{
		App::Old::Component::Object3D::Helper::AABB _aabb;
		union
		{
			uint _primsOffset = 0;	 // if leaf
			uint _secondChildOffset; // if inner node
		};
		uint16_t _splitAxis = 0;
		// TODO: uint16 is sufficient ?
		uint16_t _nbPrims = 0;
	};

	class BVH
	{
	  public:
		enum class SplitMethod
		{
			SAH,
			HLBVH
		};
		BVH()  = default;
		~BVH() = default;

		const bool									   isBuilt() const { return _isBuilt; }
		const App::Old::Component::Object3D::Helper::AABB & getAABB() const { return _nodes[ 0 ]._aabb; }

		void build( const std::vector<Primitive::BasePrimitive *> & p_prims,
					const uint										p_maxPrimsLeaf,
					const SplitMethod								p_splitMethod );

		// returns nearest intersection if exists
		bool intersect( const Ray &	   p_ray,
						const float	   p_tMin,
						const float	   p_tMax,
						Intersection & p_intersection ) const;

		// return true if any intersection is found
		bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const;

	  private:
		BVHBuildNode * _buildSAHRecursive( std::vector<BVHPrimInfo> &				 p_primsInfo,
										   const uint								 p_begin,
										   const uint								 p_end,
										   uint &									 p_totalNodes,
										   std::vector<Primitive::BasePrimitive *> & p_outPrims );

		BVHBuildNode * _buildHLBVH( const std::vector<BVHPrimInfo> &		  p_primsInfo,
									uint &									  p_totalNodes,
									std::vector<Primitive::BasePrimitive *> & p_outPrims );

		BVHBuildNode * _emitLBVHRecursive( BVHBuildNode *&							 p_buildNodes,
										   const std::vector<BVHPrimInfo> &			 p_primsInfo,
										   MortonPrim *								 p_mortonPrims,
										   uint										 p_nbPrims,
										   uint &									 p_totalNodes,
										   std::vector<Primitive::BasePrimitive *> & p_outPrims,
										   std::atomic<uint> &						 p_outPrimsOffset,
										   uint										 bitIndex ) const;

		BVHBuildNode * _buildUpperSAHRecursive( std::vector<BVHBuildNode *> & p_treeletRoots,
												uint						  p_begin,
												uint						  p_end,
												uint &						  totalNodes ) const;

		uint _flattenRecursive( const BVHBuildNode * const p_buildNode, uint & p_offset );

	  private:
		std::vector<Primitive::BasePrimitive *> _primitives;
		uint									_maxPrimLeaf = 1;
		SplitMethod								_splitMethod = SplitMethod::SAH;

		std::vector<LBVHNode> _nodes;

		bool _isBuilt = false;
	};

} // namespace VTX::App::Old::Render::Renderer::RT

#endif
