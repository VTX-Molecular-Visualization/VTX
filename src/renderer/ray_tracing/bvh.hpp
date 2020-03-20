/*
Adapted from PBRTv3 BVHAccel : https://github.com/mmp/pbrt-v3
*/

#ifndef __VTX_RENDERER_RAY_TRACER_BVH__
#define __VTX_RENDERER_RAY_TRACER_BVH__

#ifdef _MSC_VER
#pragma once
#endif

#include "primitives/base_primitive.hpp"
#include <atomic>
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		// Structures for BVH construction
		struct BVHBuildNode;
		struct BVHPrimInfo;
		struct MortonPrim;

		struct LBVHNode
		{
			Math::AABB _aabb;
			union
			{
				uint _primsOffset = 0;	 // if leaf
				uint _secondChildOffset; // if inner node
			};
			// TODO: uint16 is sufficient ?
			uint16_t _splitAxis;
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

			void build( const std::vector<BasePrimitive *> & p_prims,
						const uint							 p_maxPrimsLeaf,
						const SplitMethod					 p_splitMethod );

			// returns nearest intersection if exists
			bool intersect( const Ray &	   p_ray,
							const float	   p_tMin,
							const float	   p_tMax,
							Intersection & p_intersection ) const;

		  private:
			BVHBuildNode * _buildSAHRecursive( std::vector<BVHPrimInfo> &	  p_primsInfo,
											   const uint					  p_begin,
											   const uint					  p_end,
											   uint &						  p_totalNodes,
											   std::vector<BasePrimitive *> & p_outPrims );

			BVHBuildNode * _buildHLBVH( const std::vector<BVHPrimInfo> & p_primsInfo,
										uint &							 p_totalNodes,
										std::vector<BasePrimitive *> &	 p_outPrims );

			BVHBuildNode * _emitLBVHRecursive( BVHBuildNode *&					p_buildNodes,
											   const std::vector<BVHPrimInfo> & p_primsInfo,
											   MortonPrim *						p_mortonPrims,
											   uint								p_nbPrims,
											   uint &							p_totalNodes,
											   std::vector<BasePrimitive *> &	p_outPrims,
											   std::atomic<uint> &				p_outPrimsOffset,
											   uint								bitIndex ) const;

			BVHBuildNode * _buildUpperSAHRecursive( std::vector<BVHBuildNode *> & p_treeletRoots,
													uint						  p_begin,
													uint						  p_end,
													uint &						  totalNodes ) const;

			uint _flattenRecursive( const BVHBuildNode * const p_buildNode, uint & p_offset );

		  private:
			std::vector<BasePrimitive *> _primitives;
			uint						 _maxPrimLeaf = 1;
			SplitMethod					 _splitMethod = SplitMethod::SAH;

			std::vector<LBVHNode> _nodes;
		};

	} // namespace Renderer
} // namespace VTX

#endif
