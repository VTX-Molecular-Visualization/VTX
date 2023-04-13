/*
Adapted from PBRTv3 BVHAccel : https://github.com/mmp/pbrt-v3
*/

#ifndef __VTX_RENDERER_RAY_TRACER__
#define __VTX_RENDERER_RAY_TRACER__

#ifdef _MSC_VER
#pragma once
#endif

#include "bvh.hpp"
#include <util/math.hpp>
#include <algorithm>
#include <thread>
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX
{
	namespace Renderer
	{
		uint leafNodes		 = 0;
		uint totalLeafNodes	 = 0;
		uint totalPrimitives = 0;
		uint interiorNodes	 = 0;

		struct BVHBuildNode
		{
			BVHBuildNode() = default;
			~BVHBuildNode()
			{
				if ( _child0 != nullptr )
					delete _child0;
				if ( _child1 != nullptr )
					delete _child1;
			}
			// Constructor for leaves
			BVHBuildNode( const uint p_idFirstPrim, const uint p_nbPrims, const Object3D::Helper::AABB & p_aabb ) :
				_idFirstPrim( p_idFirstPrim ), _nbPrims( p_nbPrims ), _aabb( p_aabb )
			{
				++leafNodes;
				++totalLeafNodes;
				totalPrimitives += p_nbPrims;
			}
			// Constructor for inner nodes
			BVHBuildNode( const uint p_splitAxis, BVHBuildNode * p_child0, BVHBuildNode * p_child1 ) :
				_splitAxis( p_splitAxis ), _child0( p_child0 ), _child1( p_child1 )
			{
				_aabb = Object3D::Helper::AABB::join( p_child0->_aabb, p_child1->_aabb );
				++interiorNodes;
			}

			Object3D::Helper::AABB _aabb;
			BVHBuildNode *		   _child0		= nullptr;
			BVHBuildNode *		   _child1		= nullptr;
			uint				   _splitAxis	= 0;
			uint				   _idFirstPrim = 0;
			uint				   _nbPrims		= 0;
		};

		struct BVHPrimInfo
		{
			BVHPrimInfo() {}
			BVHPrimInfo( const uint p_idPrimitive, const Object3D::Helper::AABB & p_aabb ) :
				_idPrimitive( p_idPrimitive ), _aabb( p_aabb ), _centroid( _aabb.centroid() )
			{
			}
			uint				   _idPrimitive = 0;
			Object3D::Helper::AABB _aabb;
			Vec3f				   _centroid = VEC3F_ZERO;
		};

		struct BinInfo
		{
			uint				   _count = 0;
			Object3D::Helper::AABB _aabb;
		};

		struct MortonPrim
		{
			int		 _idPrimitive;
			uint32_t _code;
		};

		struct LBVHTreelet
		{
			LBVHTreelet( const uint p_idBegin, const uint p_nbPrimitives, BVHBuildNode * p_buildNodes ) :
				_idBegin( p_idBegin ), _nbPrims( p_nbPrimitives ), _buildNodes( p_buildNodes )
			{
			}
			uint		   _idBegin, _nbPrims;
			BVHBuildNode * _buildNodes;
		};

		void BVH::build( const std::vector<BasePrimitive *> & p_prims,
						 const uint							  p_maxPrimsLeaf,
						 const SplitMethod					  p_splitMethod )
		{
			VTX_INFO( "Building " + std::string( p_splitMethod == SplitMethod::SAH ? "BVH with SAH" : "HLBVH" ) );
			if ( p_prims.empty() )
			{
				VTX_ERROR( "Cannot build BVH without primitives !" );
				return;
			}

			// copy input primitive
			_primitives	 = p_prims;
			_maxPrimLeaf = p_maxPrimsLeaf;
			_splitMethod = p_splitMethod;

			Util::Chrono chrono;
			chrono.start();
			std::vector<BVHPrimInfo> primsInfo( _primitives.size() );
			for ( uint i = 0; i < _primitives.size(); ++i )
			{
				primsInfo[ i ] = { i, _primitives[ i ]->getAABB() };
			}

			std::vector<BasePrimitive *> outPrims;
			outPrims.reserve( p_prims.size() );

			BVHBuildNode * root		  = nullptr;
			uint		   totalNodes = 0;
			if ( _splitMethod == SplitMethod::HLBVH )
			{
				root = _buildHLBVH( primsInfo, totalNodes, outPrims );
			}
			else
			{
				root = _buildSAHRecursive( primsInfo, 0, uint( _primitives.size() ), totalNodes, outPrims );
			}
			_primitives.swap( outPrims );
			primsInfo.clear();

			// Flatten BVH tree to get LBVH
			_nodes.resize( totalNodes );
			uint offset = 0;
			_flattenRecursive( root, offset );
			assert( totalNodes == offset );

			// Clean build nodes
			if ( root != nullptr )
				delete root;

			chrono.stop();
			_isBuilt = true;
			VTX_INFO( "BVH built in " + std::to_string( chrono.elapsedTime() ) );
		}

		bool BVH::intersect( const Ray &	p_ray,
							 const float	p_tMin,
							 const float	p_tMax,
							 Intersection & p_intersection ) const
		{
			if ( _nodes.empty() )
				return false;

			const Vec3f & rayDir = p_ray.getDirection();
			const Vec3f & rayPos = p_ray.getOrigin();

			const Vec3f invDir = 1.f / rayDir;

			const Vec3i isDirNeg = { invDir.x < 0.f, invDir.y < 0.f, invDir.z < 0.f };

			float tMin = p_tMin;
			float tMax = p_tMax;

			// Follow ray through BVH nodes to find primitive intersections
			int	 toVisitOffset = 0, currentNodeIndex = 0;
			int	 nodesToVisit[ 64 ];
			bool hit = false;
			while ( true )
			{
				const LBVHNode & node = _nodes[ currentNodeIndex ];
				// Check ray against BVH node
				if ( node._aabb.intersect( rayPos, invDir, isDirNeg, tMin, tMax ) )
				{
					if ( node._nbPrims > 0 )
					{
						// Intersect ray with primitives in leaf BVH node
						for ( int i = 0; i < node._nbPrims; ++i )
						{
							if ( _primitives[ node._primsOffset + i ]->intersect( p_ray, tMin, tMax, p_intersection ) )
							{
								tMax = p_intersection._distance;
								hit	 = true;
							}
						}
						if ( toVisitOffset == 0 )
							break;
						currentNodeIndex = nodesToVisit[ --toVisitOffset ];
					}
					else
					{
						// Put far BVH node on _nodesToVisit_ stack, advance to near
						// node
						if ( isDirNeg[ node._splitAxis ] )
						{
							nodesToVisit[ toVisitOffset++ ] = currentNodeIndex + 1;
							currentNodeIndex				= node._secondChildOffset;
						}
						else
						{
							nodesToVisit[ toVisitOffset++ ] = node._secondChildOffset;
							currentNodeIndex				= currentNodeIndex + 1;
						}
					}
				}
				else
				{
					if ( toVisitOffset == 0 )
						break;
					currentNodeIndex = nodesToVisit[ --toVisitOffset ];
				}
			}
			return hit;
		}

		bool BVH::intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
		{
			if ( _nodes.empty() )
				return false;

			const Vec3f & rayDir = p_ray.getDirection();
			const Vec3f & rayPos = p_ray.getOrigin();

			const Vec3f invDir = 1.f / rayDir;

			const Vec3i isDirNeg = { invDir.x < 0.f, invDir.y < 0.f, invDir.z < 0.f };

			float tMin = p_tMin;
			float tMax = p_tMax;

			// Follow ray through BVH nodes to find primitive intersections
			int toVisitOffset = 0, currentNodeIndex = 0;
			int nodesToVisit[ 64 ];
			while ( true )
			{
				const LBVHNode & node = _nodes[ currentNodeIndex ];
				// Check ray against BVH node
				if ( node._aabb.intersect( rayPos, invDir, isDirNeg, tMin, tMax ) )
				{
					if ( node._nbPrims > 0 )
					{
						// Intersect ray with primitives in leaf BVH node
						for ( int i = 0; i < node._nbPrims; ++i )
						{
							// TODO: remove Intersection() -> make intersectAny for primitives
							Intersection inter = Intersection();
							if ( _primitives[ node._primsOffset + i ]->intersect( p_ray, tMin, tMax, inter ) )
							{
								return true;
							}
						}
						if ( toVisitOffset == 0 )
							break;
						currentNodeIndex = nodesToVisit[ --toVisitOffset ];
					}
					else
					{
						// Put far BVH node on _nodesToVisit_ stack, advance to near
						// node
						if ( isDirNeg[ node._splitAxis ] )
						{
							nodesToVisit[ toVisitOffset++ ] = currentNodeIndex + 1;
							currentNodeIndex				= node._secondChildOffset;
						}
						else
						{
							nodesToVisit[ toVisitOffset++ ] = node._secondChildOffset;
							currentNodeIndex				= currentNodeIndex + 1;
						}
					}
				}
				else
				{
					if ( toVisitOffset == 0 )
						break;
					currentNodeIndex = nodesToVisit[ --toVisitOffset ];
				}
			}
			return false;
		}

		// Build using approximate SAH
		BVHBuildNode * BVH::_buildSAHRecursive( std::vector<BVHPrimInfo> &	   p_primsInfo,
												const uint					   p_begin,
												const uint					   p_end,
												uint &						   p_totalNodes,
												std::vector<BasePrimitive *> & p_outPrims )
		{
			assert( p_begin != p_end );

			p_totalNodes++;

			Object3D::Helper::AABB aabb;
			for ( uint i = p_begin; i < p_end; ++i )
			{
				aabb.extend( p_primsInfo[ i ]._aabb );
			}

			const uint nbPrimitives = p_end - p_begin;

			if ( nbPrimitives == 1 )
			{
				// Only one primitive, create a leaf
				const uint idFirstPrimitive = uint( p_outPrims.size() );
				p_outPrims.emplace_back( _primitives[ p_primsInfo[ p_begin ]._idPrimitive ] );
				return new BVHBuildNode( idFirstPrimitive, nbPrimitives, aabb );
			}
			else
			{
				// Compute the AABB of primitives centroids
				Object3D::Helper::AABB centroidsAABB;
				for ( uint i = p_begin; i < p_end; ++i )
				{
					centroidsAABB.extend( p_primsInfo[ i ]._centroid );
				}
				uint splitDim = centroidsAABB.largestAxis();

				if ( centroidsAABB.getMax()[ splitDim ] == centroidsAABB.getMin()[ splitDim ] )
				{
					// Create leaf _BVHBuildNode_
					uint idFirstPrimitive = uint( p_outPrims.size() );
					for ( uint i = p_begin; i < p_end; ++i )
					{
						int idPrim = p_primsInfo[ i ]._idPrimitive;
						p_outPrims.emplace_back( _primitives[ idPrim ] );
					}
					return new BVHBuildNode( idFirstPrimitive, nbPrimitives, aabb );
				}
				else if ( nbPrimitives <= 2 )
				{
					const uint idSplit = ( p_begin + p_end ) / 2;
					std::nth_element( &p_primsInfo[ p_begin ],
									  &p_primsInfo[ idSplit ],
									  &p_primsInfo[ p_end - 1 ] + 1,
									  [ splitDim ]( const BVHPrimInfo & a, const BVHPrimInfo & b )
									  { return a._centroid[ splitDim ] < b._centroid[ splitDim ]; } );
					return new BVHBuildNode(
						splitDim,
						_buildSAHRecursive( p_primsInfo, p_begin, idSplit, p_totalNodes, p_outPrims ),
						_buildSAHRecursive( p_primsInfo, idSplit, p_end, p_totalNodes, p_outPrims ) );
				}
				else
				{
					// Init bins for SAH
					const uint nbBins = 12;
					BinInfo	   bins[ nbBins ];
					for ( uint i = p_begin; i < p_end; ++i )
					{
						uint b = uint( nbBins * centroidsAABB.offset( p_primsInfo[ i ]._centroid, splitDim ) );
						if ( b == nbBins )
							b--;
						assert( b >= 0 );
						assert( b < nbBins );
						bins[ b ]._count++;
						bins[ b ]._aabb.extend( p_primsInfo[ i ]._aabb );
					}

					// Compute SAH cost split for each bin
					float sahCostBins[ nbBins - 1 ];
					for ( uint i = 0; i < nbBins - 1; ++i )
					{
						Object3D::Helper::AABB aabb0;
						uint				   count0 = 0;
						for ( uint j = 0; j <= i; ++j )
						{
							aabb0.extend( bins[ j ]._aabb );
							count0 += bins[ j ]._count;
						}
						Object3D::Helper::AABB aabb1;
						uint				   count1 = 0;
						for ( uint j = i + 1; j < nbBins; ++j )
						{
							aabb1.extend( bins[ j ]._aabb );
							count1 += bins[ j ]._count;
						}
						const float area0 = aabb0.area();
						const float area1 = aabb1.area();
						const float area  = aabb.area();
						sahCostBins[ i ]  = 1 + ( count0 * aabb0.area() + count1 * aabb1.area() ) / aabb.area();
					}

					// Search minimal cost among previously computed costs
					float minSahCost	  = sahCostBins[ 0 ];
					uint  idMinSahCostBin = 0;
					for ( uint i = 1; i < nbBins - 1; ++i )
					{
						if ( sahCostBins[ i ] < minSahCost )
						{
							minSahCost		= sahCostBins[ i ];
							idMinSahCostBin = i;
						}
					}

					const float sahCostLeaf = float( nbPrimitives );
					if ( nbPrimitives > _maxPrimLeaf || minSahCost < sahCostLeaf )
					{
						// Split it !
						BVHPrimInfo * nodeSplit = std::partition(
							&p_primsInfo[ p_begin ],
							&p_primsInfo[ p_end - 1 ] + 1,
							[ & ]( const BVHPrimInfo & p )
							{
								uint b = uint( nbBins * centroidsAABB.offset( p._centroid, splitDim ) );
								if ( b == nbBins )
									b--;
								assert( b >= 0 );
								assert( b < nbBins );
								return b <= idMinSahCostBin;
							} );

						const uint idSplit = uint( nodeSplit - &p_primsInfo[ 0 ] );
						return new BVHBuildNode(
							splitDim,
							_buildSAHRecursive( p_primsInfo, p_begin, idSplit, p_totalNodes, p_outPrims ),
							_buildSAHRecursive( p_primsInfo, idSplit, p_end, p_totalNodes, p_outPrims ) );
					}
					else
					{
						// Create leaf
						const uint idFirstPrimitive = uint( p_outPrims.size() );
						for ( uint i = p_begin; i < p_end; ++i )
						{
							p_outPrims.emplace_back( _primitives[ p_primsInfo[ i ]._idPrimitive ] );
						}
						return new BVHBuildNode( idFirstPrimitive, nbPrimitives, aabb );
					}
				}
			}
		}

		static void radixSort( std::vector<MortonPrim> * v )
		{
			std::vector<MortonPrim> tempVector( v->size() );
			const uint				bitsPerPass = 6;
			const uint				nbBits		= 30;
			static_assert( ( nbBits % bitsPerPass ) == 0, "Radix sort bitsPerPass must evenly divide nBits" );
			const uint nbPasses = nbBits / bitsPerPass;

			for ( uint pass = 0; pass < nbPasses; ++pass )
			{
				// Perform one pass of radix sort, sorting _bitsPerPass_ bits
				int lowBit = pass * bitsPerPass;

				// Set in and out vector pointers for radix sort pass
				const std::vector<MortonPrim> & in	= ( pass & 1 ) ? tempVector : *v;
				std::vector<MortonPrim> &		out = ( pass & 1 ) ? *v : tempVector;

				// Count number of zero bits in array for current radix sort bit
				constexpr int nbBins			  = 1 << bitsPerPass;
				uint		  binsCount[ nbBins ] = { 0 };
				constexpr int bitMask			  = ( 1 << bitsPerPass ) - 1;
				for ( const MortonPrim & mp : in )
				{
					const uint bin = ( mp._code >> lowBit ) & bitMask;
					assert( bin >= 0 );
					assert( bin < nbBins );
					++binsCount[ bin ];
				}

				// Compute starting index in output array for each bin
				uint outIndex[ nbBins ];
				outIndex[ 0 ] = 0;
				for ( int i = 1; i < nbBins; ++i )
					outIndex[ i ] = outIndex[ i - 1 ] + binsCount[ i - 1 ];

				// Store sorted values in output array
				for ( const MortonPrim & mp : in )
				{
					int bin					 = ( mp._code >> lowBit ) & bitMask;
					out[ outIndex[ bin ]++ ] = mp;
				}
			}
			// Copy final result from _tempVector_, if needed
			if ( nbPasses & 1 )
				std::swap( *v, tempVector );
		}

		BVHBuildNode * BVH::_buildHLBVH( const std::vector<BVHPrimInfo> & p_primsInfo,
										 uint &							  p_totalNodes,
										 std::vector<BasePrimitive *> &	  p_outPrims )
		{
			// Compute the global AABB of primitives centroids
			Object3D::Helper::AABB aabb;
			for ( uint i = 0; i < uint( p_primsInfo.size() ); ++i )
			{
				aabb.extend( p_primsInfo[ i ]._aabb );
			}

			// Define primitives Morton indices
			std::vector<MortonPrim> mortonPrims( p_primsInfo.size() );
			const uint				mortonBits	= 10;
			const uint				mortonScale = 1 << mortonBits;

			// init parallel
			const uint				 nbThreads = std::thread::hardware_concurrency();
			std::vector<std::thread> threadPool;
			threadPool.reserve( nbThreads );

			for ( uint i = 0; i < nbThreads; ++i )
			{
				threadPool.emplace_back( std::thread(
					[ nbThreads, &mortonPrims, mortonScale, &p_primsInfo, &aabb, i ]()
					{
						uint id = i;
						while ( id < p_primsInfo.size() )
						{
							mortonPrims[ id ]._idPrimitive = p_primsInfo[ id ]._idPrimitive;
							const Vec3f centroidOffset	   = aabb.offset( p_primsInfo[ id ]._centroid );
							mortonPrims[ id ]._code
								= Util::Math::encodeMorton3( centroidOffset * float( mortonScale ) );
							id += nbThreads;
						}
					} ) );
			}
			for ( std::thread & t : threadPool )
			{
				t.join();
			}
			threadPool.clear();

			// Sort Morton primitives
			radixSort( &mortonPrims );

			// Build LBVH treelets in BVH leaves
			std::vector<LBVHTreelet> treelets;
			for ( uint begin = 0, end = 1; end <= uint( mortonPrims.size() ); ++end )
			{
				uint mask = 0x3ffc0000;
				if ( end == (int)mortonPrims.size()
					 || ( ( mortonPrims[ begin ]._code & mask ) != ( mortonPrims[ end ]._code & mask ) ) )
				{
					// Add entry to _treeletsToBuild_ for this treelet
					uint		   nbPrimitives = end - begin;
					uint		   maxBVHNodes	= 2 * nbPrimitives;
					BVHBuildNode * nodes		= new BVHBuildNode[ maxBVHNodes ];
					treelets.emplace_back( begin, nbPrimitives, nodes );
					begin = end;
				}
			}

			std::atomic<uint> atomicTotalNodes	  = 0;
			std::atomic<uint> atomicOutPrimOffset = 0;
			p_outPrims.resize( _primitives.size() );

			for ( uint i = 0; i < nbThreads; ++i )
			{
				threadPool.emplace_back( std::thread(
					[ this,
					  nbThreads,
					  &treelets,
					  &p_primsInfo,
					  &mortonPrims,
					  &p_outPrims,
					  &atomicOutPrimOffset,
					  &atomicTotalNodes,
					  i ]()
					{
						uint id = i;
						while ( id < treelets.size() )
						{
							uint		  nodesCreated	= 0;
							const uint	  firstBitIndex = 29 - 12;
							LBVHTreelet & treelet		= treelets[ id ];
							treelet._buildNodes			= _emitLBVHRecursive( treelet._buildNodes,
																	  p_primsInfo,
																	  &mortonPrims[ treelet._idBegin ],
																	  treelet._nbPrims,
																	  nodesCreated,
																	  p_outPrims,
																	  atomicOutPrimOffset,
																	  firstBitIndex );
							atomicTotalNodes += nodesCreated;
							id += nbThreads;
						}
					} ) );
			}
			for ( std::thread & t : threadPool )
			{
				t.join();
			}

			p_totalNodes = atomicTotalNodes;

			// Build a SAH BVH upper the LBVH treelets
			std::vector<BVHBuildNode *> builtTreelets( treelets.size() );
			for ( uint i = 0; i < uint( treelets.size() ); ++i )
			{
				builtTreelets[ i ] = treelets[ i ]._buildNodes;
			}

			return _buildUpperSAHRecursive( builtTreelets, 0, uint( builtTreelets.size() ), p_totalNodes );
		}

		BVHBuildNode * BVH::_emitLBVHRecursive( BVHBuildNode *&					 p_buildNodes,
												const std::vector<BVHPrimInfo> & p_primsInfo,
												MortonPrim *					 p_mortonPrims,
												uint							 p_nbPrims,
												uint &							 p_totalNodes,
												std::vector<BasePrimitive *> &	 p_outPrims,
												std::atomic<uint> &				 p_outPrimsOffset,
												uint							 bitIndex ) const
		{
			assert( p_nbPrims > 0 );
			if ( bitIndex == -1 || p_nbPrims < _maxPrimLeaf )
			{
				// Create and return leaf node of LBVH treelet
				p_totalNodes++;
				BVHBuildNode *		   node = p_buildNodes++;
				Object3D::Helper::AABB aabb;
				uint				   idFirstPrimitive = p_outPrimsOffset.fetch_add( p_nbPrims );
				for ( uint i = 0; i < p_nbPrims; ++i )
				{
					const uint idPrim				   = p_mortonPrims[ i ]._idPrimitive;
					p_outPrims[ idFirstPrimitive + i ] = _primitives[ idPrim ];
					aabb.extend( p_primsInfo[ idPrim ]._aabb );
				}
				return new BVHBuildNode( idFirstPrimitive, p_nbPrims, aabb );
			}
			else
			{
				uint mask = 1 << bitIndex;
				// Advance to next subtree level if there's no LBVH split for this bit
				if ( ( p_mortonPrims[ 0 ]._code & mask ) == ( p_mortonPrims[ p_nbPrims - 1 ]._code & mask ) )
					return _emitLBVHRecursive( p_buildNodes,
											   p_primsInfo,
											   p_mortonPrims,
											   p_nbPrims,
											   p_totalNodes,
											   p_outPrims,
											   p_outPrimsOffset,
											   bitIndex - 1 );

				// Find LBVH split point for this dimension
				uint searchBegin = 0;
				uint searchEnd	 = p_nbPrims - 1;
				while ( searchBegin + 1 != searchEnd )
				{
					assert( searchBegin != searchEnd );
					uint idSplit = ( searchBegin + searchEnd ) / 2;
					if ( ( p_mortonPrims[ searchBegin ]._code & mask ) == ( p_mortonPrims[ idSplit ]._code & mask ) )
					{
						searchBegin = idSplit;
					}
					else
					{
						assert( ( p_mortonPrims[ idSplit ]._code & mask )
								== ( p_mortonPrims[ searchEnd ]._code & mask ) );
						searchEnd = idSplit;
					}
				}
				uint splitOffset = searchEnd;
				assert( splitOffset <= p_nbPrims - 1 );
				assert( ( p_mortonPrims[ splitOffset - 1 ]._code & mask )
						!= ( p_mortonPrims[ splitOffset ]._code & mask ) );

				// Create and return interior LBVH node
				p_totalNodes++;
				BVHBuildNode * node		 = p_buildNodes++;
				BVHBuildNode * lbvh[ 2 ] = { _emitLBVHRecursive( p_buildNodes,
																 p_primsInfo,
																 p_mortonPrims,
																 splitOffset,
																 p_totalNodes,
																 p_outPrims,
																 p_outPrimsOffset,
																 bitIndex - 1 ),
											 _emitLBVHRecursive( p_buildNodes,
																 p_primsInfo,
																 &p_mortonPrims[ splitOffset ],
																 p_nbPrims - splitOffset,
																 p_totalNodes,
																 p_outPrims,
																 p_outPrimsOffset,
																 bitIndex - 1 ) };
				uint		   axis		 = bitIndex % 3;
				return new BVHBuildNode( axis, lbvh[ 0 ], lbvh[ 1 ] );
			}
		}

		BVHBuildNode * BVH::_buildUpperSAHRecursive( std::vector<BVHBuildNode *> & p_treeletRoots,
													 uint						   p_begin,
													 uint						   p_end,
													 uint &						   totalNodes ) const
		{
			assert( p_begin < p_end );
			uint nbNodes = p_end - p_begin;
			if ( nbNodes == 1 )
				return p_treeletRoots[ p_begin ];

			totalNodes++;

			// Compute AABB of each treelet
			Object3D::Helper::AABB aabb;
			for ( uint i = p_begin; i < p_end; ++i )
				aabb.extend( p_treeletRoots[ i ]->_aabb );

			// Compute the AABB of each treelet root (HLBVH node) centroid
			Object3D::Helper::AABB centroidsAABB;
			for ( uint i = p_begin; i < p_end; ++i )
			{
				centroidsAABB.extend( p_treeletRoots[ i ]->_aabb.centroid() );
			}

			uint splitDim = centroidsAABB.largestAxis();
			// FIXME: if this hits, what do we need to do?
			// Make sure the SAH split below does something... ?
			assert( centroidsAABB.getMax()[ splitDim ] != centroidsAABB.getMin()[ splitDim ] );

			// Init bins for SAH
			const uint nbBins = 12;
			BinInfo	   bins[ nbBins ];
			for ( uint i = p_begin; i < p_end; ++i )
			{
				const float splitCentroid = ( p_treeletRoots[ i ]->_aabb.getMin()[ splitDim ]
											  + p_treeletRoots[ i ]->_aabb.getMax()[ splitDim ] )
											* 0.5f;
				uint b = uint( nbBins
							   * ( ( splitCentroid - centroidsAABB.getMin()[ splitDim ] )
								   / ( centroidsAABB.getMax()[ splitDim ] - centroidsAABB.getMin()[ splitDim ] ) ) );
				if ( b == nbBins )
					b = nbBins - 1;
				assert( b >= 0 );
				assert( b < nbBins );
				bins[ b ]._count++;
				bins[ b ]._aabb.extend( p_treeletRoots[ i ]->_aabb );
			}

			// Compute SAH cost split for each bin
			float sahCostBins[ nbBins - 1 ];
			for ( uint i = 0; i < nbBins - 1; ++i )
			{
				Object3D::Helper::AABB aabb0;
				uint				   count0 = 0;
				for ( uint j = 0; j <= i; ++j )
				{
					aabb0.extend( bins[ j ]._aabb );
					count0 += bins[ j ]._count;
				}
				Object3D::Helper::AABB aabb1;
				uint				   count1 = 0;
				for ( uint j = i + 1; j < nbBins; ++j )
				{
					aabb1.extend( bins[ j ]._aabb );
					count1 += bins[ j ]._count;
				}
				sahCostBins[ i ] = .125f + ( count0 * aabb0.area() + count1 * aabb1.area() ) / aabb.area();
			}

			// Search minimal cost among previously computed costs
			float minSahCost	  = sahCostBins[ 0 ];
			uint  idMinSahCostBin = 0;
			for ( int i = 1; i < nbBins - 1; ++i )
			{
				if ( sahCostBins[ i ] < minSahCost )
				{
					minSahCost		= sahCostBins[ i ];
					idMinSahCostBin = i;
				}
			}

			// Split it !
			BVHBuildNode ** pmid = std::partition(
				&p_treeletRoots[ p_begin ],
				&p_treeletRoots[ p_end - 1 ] + 1,
				[ & ]( const BVHBuildNode * node )
				{
					const float centroid
						= ( node->_aabb.getMin()[ splitDim ] + node->_aabb.getMax()[ splitDim ] ) * 0.5f;
					uint b
						= uint( nbBins
								* ( ( centroid - centroidsAABB.getMin()[ splitDim ] )
									/ ( centroidsAABB.getMax()[ splitDim ] - centroidsAABB.getMin()[ splitDim ] ) ) );
					if ( b == nbBins )
						b = nbBins - 1;
					assert( b >= 0 );
					assert( b < nbBins );
					return b <= idMinSahCostBin;
				} );
			uint idSplit = uint( pmid - &p_treeletRoots[ 0 ] );
			assert( idSplit > p_begin );
			assert( idSplit < p_end );
			return new BVHBuildNode( splitDim,
									 _buildUpperSAHRecursive( p_treeletRoots, p_begin, idSplit, totalNodes ),
									 _buildUpperSAHRecursive( p_treeletRoots, idSplit, p_end, totalNodes ) );
		}

		uint BVH::_flattenRecursive( const BVHBuildNode * const p_buildNode, uint & p_offset )
		{
			LBVHNode & node = _nodes[ p_offset ];
			node._aabb		= p_buildNode->_aabb;
			uint myOffset	= p_offset++;
			if ( p_buildNode->_nbPrims > 0 )
			{
				assert( !p_buildNode->_child0 && !p_buildNode->_child1 );
				assert( p_buildNode->_nbPrims < 65536 ); // because _nbPrims is uint16_t
				node._primsOffset = p_buildNode->_idFirstPrim;
				node._nbPrims	  = p_buildNode->_nbPrims;
			}
			else
			{
				// Create interior flattened BVH node
				node._splitAxis = p_buildNode->_splitAxis;
				node._nbPrims	= 0;
				_flattenRecursive( p_buildNode->_child0, p_offset );
				node._secondChildOffset = _flattenRecursive( p_buildNode->_child1, p_offset );
			}
			return myOffset;
		}

	} // namespace Renderer
} // namespace VTX
#endif
