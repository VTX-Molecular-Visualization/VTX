#include "ce_align.hpp"
#include "old_tool/analysis/rmsd.hpp"
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/model/residue.hpp>
#include <cmath>
#include <list>
#include <string>
#include <util/logger.hpp>

namespace VTX::Analysis::StructuralAlignmentMethod
{
	Struct::ResidueCenterOfMassDataSet CEAlign::_residuePositionsDataSet = Struct::ResidueCenterOfMassDataSet();

	CEAlign::CustomParameters::CustomParameters() :
		AlignmentParameters::AlignmentParameters( StructuralAlignment::AlignmentMethodEnum::CEAlign )
	{
		windowSize = Setting::CE_ALIGN_WIN_SIZE_DEFAULT;
		gapMax	   = Setting::CE_ALIGN_GAP_MAX_DEFAULT;
		maxPath	   = Setting::CE_ALIGN_MAX_PATH_DEFAULT;
		d0		   = Setting::CE_ALIGN_D0_DEFAULT;
		d1		   = Setting::CE_ALIGN_D1_DEFAULT;
	}

	StructuralAlignment::AlignmentResult CEAlign::compute(
		const Model::Molecule &							 p_staticMolecule,
		Model::Molecule &								 p_mobileMolecule,
		const StructuralAlignment::AlignmentParameters & p_parameters )
	{
		const CustomParameters & castedParameters = reinterpret_cast<const CustomParameters &>( p_parameters );

		// Ensure good value for windowSize
		CustomParameters correctedParameters = CustomParameters( castedParameters );

		const int minMolSize		   = p_staticMolecule.getResidueCount() < p_mobileMolecule.getResidueCount()
											 ? p_staticMolecule.getResidueCount()
											 : p_mobileMolecule.getResidueCount();
		correctedParameters.windowSize = std::min( castedParameters.windowSize, int( sqrt( minMolSize ) ) );

		std::vector<Vec3f> staticMoleculeResiduePositions
			= _generateResiduePositionsVector( p_staticMolecule, castedParameters );
		std::vector<Vec3f> mobileMoleculeResiduePositions
			= _generateResiduePositionsVector( p_mobileMolecule, castedParameters );

		// Calculate distance matrix
		const Matrix<float> distanceMatrixStaticMol = _computeDistanceMatrix( staticMoleculeResiduePositions );
		const Matrix<float> distanceMatrixMobileMol = _computeDistanceMatrix( mobileMoleculeResiduePositions );

		// Calculate CE similarities
		const Matrix<float> scoreMatrix
			= _computeScoreMatrix( distanceMatrixStaticMol, distanceMatrixMobileMol, correctedParameters );

		// Calculate Top N Paths
		const std::vector<Path> bestPaths
			= _findPath( scoreMatrix, distanceMatrixStaticMol, distanceMatrixMobileMol, correctedParameters );

		const BestPathResult bestPathResult = _pickBest(
			staticMoleculeResiduePositions, mobileMoleculeResiduePositions, bestPaths, correctedParameters );

		const double atomRmsdOnPath = _computeRMSDOnAtomOfPath( p_staticMolecule,
																p_mobileMolecule,
																bestPaths[ bestPathResult.pathIndex ],
																p_parameters.windowSize,
																bestPathResult.transformationMatrix );

		StructuralAlignment::AlignmentResult result
			= StructuralAlignment::AlignmentResult( &p_staticMolecule, &p_mobileMolecule );

		result.transformationMatrix = bestPathResult.transformationMatrix;
		result.alignedResiduesRMSD	= atomRmsdOnPath;
		result.alignedResidueCount	= bestPathResult.positionsCount;

		return result;
	}

	std::vector<Vec3f> CEAlign::_generateResiduePositionsVector( const Model::Molecule &  p_molecule,
																 const CustomParameters & p_parameters )
	{
		std::vector<Vec3f> residuePositionsVector = std::vector<Vec3f>();
		residuePositionsVector.resize( p_molecule.getResidueCount() );

		size_t index = 0;

		for ( uint iResidue = 0; iResidue < p_molecule.getResidueCount(); iResidue++ )
		{
			const Model::Residue * const residuePtr = p_molecule.getResidue( iResidue );

			bool considerResidue = residuePtr != nullptr;
			considerResidue		 = considerResidue && ( p_parameters.considerWater || !residuePtr->isWater() );
			considerResidue		 = considerResidue && ( p_parameters.considerHiddenResidue || residuePtr->isVisible() );

			if ( considerResidue )
			{
				residuePositionsVector[ index ] = _computeResidueCenterOfMass( *residuePtr );
				index++;
			}
		}

		residuePositionsVector.resize( index );

		return residuePositionsVector;
	}

	Vec3f CEAlign::_computeResidueCenterOfMass( const Model::Residue & p_residue )
	{
		// return _residuePositionsDataSet.getPositionInMolecule( p_residue );

		const Model::Molecule &						molecule = *( p_residue.getMoleculePtr() );
		const Model::Molecule::AtomPositionsFrame & atomPositions
			= molecule.getAtomPositionFrame( molecule.getFrame() );

		Vec3f res = VEC3F_ZERO;

		for ( uint i = p_residue.getIndexFirstAtom(); i < p_residue.getIndexFirstAtom() + p_residue.getAtomCount();
			  i++ )
		{
			res += atomPositions[ i ];
		}

		res /= p_residue.getAtomCount();

		return res;
	}

	Matrix<float> CEAlign::_computeDistanceMatrix( const std::vector<Vec3f> & p_positions )
	{
		Matrix<float> distanceMatrix = Matrix<float>( p_positions.size(), p_positions.size() );

		for ( size_t iRow = 0; iRow < p_positions.size(); iRow++ )
		{
			for ( size_t iCol = iRow; iCol < p_positions.size(); iCol++ )
			{
				const float dist = Util::Math::distance( p_positions[ iRow ], p_positions[ iCol ] );

				distanceMatrix.set( iRow, iCol, dist );
				distanceMatrix.set( iCol, iRow, dist );
			}
		}

		return distanceMatrix;
	}

	Matrix<float> CEAlign::_computeScoreMatrix( const Matrix<float> &	 p_distanceMatrix1,
												const Matrix<float> &	 p_distanceMatrix2,
												const CustomParameters & p_parameters )
	{
		Matrix<float> scoreMatrix
			= Matrix<float>( p_distanceMatrix1.getRowCount(), p_distanceMatrix2.getRowCount(), -1.f );

		const int windowSize = p_parameters.windowSize;

		const int sumSize = ( windowSize - 1.f ) * ( windowSize - 2.f ) / 2.f;

		const size_t windSizeMat1 = p_distanceMatrix1.getRowCount() - windowSize;
		const size_t windSizeMat2 = p_distanceMatrix2.getRowCount() - windowSize;

		for ( size_t iA = 0; iA <= windSizeMat1; iA++ )
		{
			for ( size_t iB = 0; iB <= windSizeMat2; iB++ )
			{
				float score = 0.f;

				for ( size_t row = 0; row < windowSize - 2; row++ )
				{
					for ( size_t col = row + 2; col < windowSize; col++ )
					{
						score += fabsf( p_distanceMatrix1.get( iA + row, iA + col )
										- p_distanceMatrix2.get( iB + row, iB + col ) );
					}
				}

				scoreMatrix.set( iA, iB, score / sumSize );
			}
		}

		return scoreMatrix;
	}

	std::vector<CEAlign::Path> CEAlign::_findPath( const Matrix<float> &	p_scoreMatrix,
												   const Matrix<float> &	p_distanceMatrixA,
												   const Matrix<float> &	p_distanceMatrixB,
												   const CustomParameters & p_parameters )
	{
		const float d0		= p_parameters.d0;
		const float d1		= p_parameters.d1;
		const int	winSize = p_parameters.windowSize;
		const int	gapMax	= p_parameters.gapMax;

		const size_t lengthA = p_distanceMatrixA.getRowCount();
		const size_t lengthB = p_distanceMatrixB.getRowCount();

		const size_t winSizeMinus1 = size_t( winSize - 1 );

		// Length of longest possible alignment
		const size_t longestAlignmentLength = ( lengthA < lengthB ) ? lengthA : lengthB;
		const int	 winSum					= ( winSize - 1 ) * ( winSize - 2 ) / 2;

		// Store best Path length to skip potential shorter paths.
		size_t bestPathLength = 0;
		size_t bestPathScore  = 0;

		// for storing the best N paths
		std::list<Path>	 pathResults  = std::list<Path>();
		std::list<float> scoreResults = std::list<float>();

		// winCache
		std::vector<size_t> winCache = std::vector<size_t>();
		winCache.resize( longestAlignmentLength );

		for ( size_t i = 0; i < longestAlignmentLength; i++ )
			winCache[ i ] = ( i + 1 ) * i * winSize / 2 + ( i + 1 ) * winSum;

		// allScoreBuffer
		// this 2D array keeps track of all partial gapped scores
		Matrix<float> allScoreBuffer = Matrix<float>( longestAlignmentLength, size_t( gapMax ) * 2 + 1, FLOAT_MAX );

		std::vector<size_t> tIndex = std::vector<size_t>();
		tIndex.resize( longestAlignmentLength, -1 );

		//======================================================================
		// Start the search through the CE matrix.
		size_t newBestPathLength = 0;
		float  newBestPathScore	 = 0;
		Path   newBestPath		 = Path();
		newBestPath.resize( longestAlignmentLength, { -1, -1 } );

		for ( size_t iA = 0; iA < lengthA; iA++ )
		{
			if ( iA > lengthA - winSize * ( bestPathLength - 1 ) )
				break;

			for ( size_t iB = 0; iB < lengthB; iB++ )
			{
				if ( p_scoreMatrix.get( iA, iB ) >= d0 )
					continue;

				if ( p_scoreMatrix.get( iA, iB ) == -1.0 )
					continue;

				if ( iB > lengthB - winSize * ( bestPathLength - 1 ) )
					break;

				// Restart curPath here.
				Path currentPath = Path();
				currentPath.resize( longestAlignmentLength, { -1, -1 } );
				currentPath[ 0 ] = { iA, iB };

				size_t curPathLength		= 1;
				tIndex[ curPathLength - 1 ] = 0;
				float curTotalScore			= 0.0;

				bool done = false;
				while ( !done )
				{
					float gapBestScore = FLOAT_MAX;
					int	  gapBestIndex = -1;

					// Check all possible gaps [1..gapMax] from here
					for ( int g = 0; g < ( gapMax * 2 ) + 1; g++ )
					{
						size_t jA = currentPath[ curPathLength - 1 ].first + winSize;
						size_t jB = currentPath[ curPathLength - 1 ].second + winSize;

						if ( g % 2 == 0 )
							jB += ( g + 1 ) / 2;
						else
							jA += ( g + 1 ) / 2;

						// Following are three heuristics to ensure high quality long paths and make sure we don't run
						// over the end of the S, matrix.

						// 1st: If jA and jB are at the end of the matrix
						if ( jA > lengthA - winSize || jB > lengthB - winSize )
						{
							// FIXME, was: jA > lenA-winSize-1 || jB > lenB-winSize-1
							continue;
						}
						// 2nd: If this gapped octapeptide is bad, ignore it.
						if ( p_scoreMatrix.get( jA, jB ) > d0 )
							continue;
						// 3rd: if too close to end, ignore it.
						if ( p_scoreMatrix.get( jA, jB ) == -1.0 )
							continue;

						float currentScore = 0.0;
						for ( int s = 0; s < curPathLength; s++ )
						{
							currentScore += abs( p_distanceMatrixA.get( currentPath[ s ].first, jA )
												 - p_distanceMatrixB.get( currentPath[ s ].second, jB ) );

							currentScore += abs(
								p_distanceMatrixA.get( currentPath[ s ].first + winSizeMinus1, jA + winSizeMinus1 )
								- p_distanceMatrixB.get( currentPath[ s ].second + winSizeMinus1,
														 jB + winSizeMinus1 ) );

							for ( int k = 1; k < winSizeMinus1; k++ )
							{
								currentScore += abs(
									p_distanceMatrixA.get( currentPath[ s ].first + k, jA + winSizeMinus1 - k )
									- p_distanceMatrixB.get( currentPath[ s ].second + k, jB + winSizeMinus1 - k ) );
							}
						}

						currentScore /= float( winSize ) * float( curPathLength );

						if ( currentScore >= d1 )
						{
							continue;
						}
						else if ( currentScore < gapBestScore )
						{
							currentPath[ curPathLength ].first	= jA;
							currentPath[ curPathLength ].second = jB;
							gapBestScore						= currentScore;
							gapBestIndex						= g;
							allScoreBuffer.set( curPathLength - 1, g, currentScore );
						}
					}

					// DONE GAPPING:

					// calculate curTotalScore
					curTotalScore = 0.0;
					size_t jGap, gA, gB;
					float  score1 = 0.0, score2 = 0.0;

					if ( gapBestIndex != -1 )
					{
						jGap = ( gapBestIndex + 1 ) / 2;
						if ( ( gapBestIndex + 1 ) % 2 == 0 )
						{
							gA = currentPath[ curPathLength - 1 ].first + winSize + jGap;
							gB = currentPath[ curPathLength - 1 ].second + winSize;
						}
						else
						{
							gA = currentPath[ curPathLength - 1 ].first + winSize;
							gB = currentPath[ curPathLength - 1 ].second + winSize + jGap;
						}

						// perfect
						score1 = ( allScoreBuffer.get( curPathLength - 1, gapBestIndex ) * winSize * curPathLength
								   + p_scoreMatrix.get( gA, gB ) * winSum )
								 / ( winSize * curPathLength + winSum );

						// perfect
						score2 = ( ( curPathLength > 1
										 ? ( allScoreBuffer.get( curPathLength - 2, tIndex[ curPathLength - 1 ] ) )
										 : p_scoreMatrix.get( iA, iB ) )
									   * winCache[ curPathLength - 1 ]
								   + score1 * ( winCache[ curPathLength ] - winCache[ curPathLength - 1 ] ) )
								 / winCache[ curPathLength ];

						curTotalScore = score2;

						// heuristic -- path is getting sloppy, stop looking
						if ( curTotalScore > d1 )
						{
							done		 = 1;
							gapBestIndex = -1;
							break;
						}
						else
						{
							allScoreBuffer.set( curPathLength - 1, gapBestIndex, curTotalScore );
							tIndex[ curPathLength ] = gapBestIndex;
							curPathLength++;
						}
					}
					else
					{
						// if here, then there was no good gapped path so quit and restart from iA, iB+1
						done = 1;
						curPathLength--;
						break;
					}

					// test this gapped path against the best seen
					// starting from iA, iB

					// if our currently best gapped path from iA and iB is LONGER
					// than the current best; or, it's equal length and the score's
					// better, keep the new path.

					if ( curPathLength > newBestPathLength
						 || ( curPathLength == newBestPathLength && curTotalScore < newBestPathScore ) )
					{
						std::copy( currentPath.begin(), currentPath.begin() + curPathLength, newBestPath.begin() );

						newBestPathLength = curPathLength;
						newBestPathScore  = curTotalScore;
					}
				}

				// At this point, we've found the best path starting at iA, iB.

				if ( newBestPathLength == 0 )
					continue;

				// Insert path
				const bool bestPathBetterThanCurrentPath
					= newBestPathLength > bestPathLength
					  || ( newBestPathLength == bestPathLength && newBestPathScore < bestPathScore );

				//////////////////////////////////////////////////////////////////////////////////////
				// TODO Selection 2 to experiment
				//////////////////////////////////////////////////////////////////////////////////////
				// if ( bestPathBetterThanCurrentPath )
				//{
				//	Path savePath = Path();
				//	savePath.resize( newBestPathLength );
				//	std::move( newBestPath.begin(), newBestPath.begin() + newBestPathLength, savePath.begin() );
				//	pathResults.push_front( savePath );
				//	scoreResults.push_front( newBestPathScore );

				//	if ( pathResults.size() > p_parameters.maxPath )
				//	{
				//		pathResults.pop_back();
				//		scoreResults.pop_back();
				//	}

				//	bestPathLength = newBestPathLength;
				//	bestPathScore  = newBestPathScore;

				//	addedPathsCounter++;
				//}

				std::list<Path>::iterator  itPath	= pathResults.begin();
				std::list<float>::iterator itScore	= scoreResults.begin();
				bool					   inserted = false;

				while ( itPath != pathResults.end() )
				{
					const bool bestPathBetterThanCurrentPath
						= newBestPathLength > itPath->size()
						  || ( newBestPathLength == itPath->size() && newBestPathScore < ( *itScore ) );

					if ( bestPathBetterThanCurrentPath )
					{
						// Will be inserted in first => update new best length
						if ( itPath == pathResults.begin() )
							bestPathLength = newBestPathLength;

						Path savePath = Path();
						savePath.resize( newBestPathLength );
						std::move( newBestPath.begin(), newBestPath.begin() + newBestPathLength, savePath.begin() );
						pathResults.insert( itPath, savePath );
						scoreResults.insert( itScore, newBestPathScore );

						if ( pathResults.size() > p_parameters.maxPath )
						{
							pathResults.pop_back();
							scoreResults.pop_back();
						}

						inserted = true;
						break;
					}

					itPath++;
					itScore++;
				}

				if ( !inserted && pathResults.size() < p_parameters.maxPath )
				{
					if ( pathResults.size() == 0 )
						bestPathLength = newBestPathLength;

					Path savePath = Path();
					savePath.resize( newBestPathLength );
					std::move( newBestPath.begin(), newBestPath.begin() + newBestPathLength, savePath.begin() );

					pathResults.insert( itPath, savePath );
					scoreResults.insert( itScore, newBestPathScore );

					inserted = true;
				}
			}
		}

		std::vector<CEAlign::Path> res = std::vector<CEAlign::Path>();
		res.resize( pathResults.size() );
		std::move( pathResults.begin(), pathResults.end(), res.begin() );

		return res;
	}
	CEAlign::BestPathResult CEAlign::_pickBest( const std::vector<Vec3f> & p_residuePositionsA,
												const std::vector<Vec3f> & p_residuePositionsB,
												const std::vector<Path> &  p_results,
												const CustomParameters &   p_parameters )
	{
		CEAlign::BestPathResult result = CEAlign::BestPathResult();

		for ( int i = 0; i < p_results.size(); i++ )
		{
			const Path & curPath		= p_results[ i ];
			const size_t positionsCount = p_parameters.windowSize * curPath.size();

			std::vector<Vec3f> residuePositionsA = std::vector<Vec3f>();
			residuePositionsA.resize( positionsCount );

			std::vector<Vec3f> residuePositionsB = std::vector<Vec3f>();
			residuePositionsB.resize( positionsCount );

			Vec3f centerOfMassA = VEC3F_ZERO;
			Vec3f centerOfMassB = VEC3F_ZERO;

			for ( int i = 0; i < curPath.size(); i++ )
			{
				const size_t startIndexA = curPath[ i ].first;
				const size_t startIndexB = curPath[ i ].second;

				for ( int k = 0; k < p_parameters.windowSize; k++ )
				{
					const Vec3f positionA = p_residuePositionsA[ startIndexA + k ];
					const Vec3f positionB = p_residuePositionsB[ startIndexB + k ];

					residuePositionsA[ i * p_parameters.windowSize + k ] = positionA;
					residuePositionsB[ i * p_parameters.windowSize + k ] = positionB;

					centerOfMassA += positionA / float( positionsCount );
					centerOfMassB += positionB / float( positionsCount );
				}
			}

			for ( size_t i = 0; i < positionsCount; i++ )
			{
				residuePositionsA[ i ] -= centerOfMassA;
				residuePositionsB[ i ] -= centerOfMassB;
			}

			Eigen::Matrix3Xd in	 = Eigen::Matrix3Xd( 3, positionsCount );
			Eigen::Matrix3Xd out = Eigen::Matrix3Xd( 3, positionsCount );

			for ( int i = 0; i < positionsCount; i++ )
			{
				in.col( i ) << residuePositionsA[ i ].x, residuePositionsA[ i ].y, residuePositionsA[ i ].z;
				out.col( i ) << residuePositionsB[ i ].x, residuePositionsB[ i ].y, residuePositionsB[ i ].z;
			}

			const Mat4f eigenMatrix
				= _find3DAffineTransform( residuePositionsA, residuePositionsB, centerOfMassA, centerOfMassB );

			const Mat3f rotation = { eigenMatrix[ 0 ][ 0 ], eigenMatrix[ 0 ][ 1 ], eigenMatrix[ 0 ][ 2 ],
									 eigenMatrix[ 1 ][ 0 ], eigenMatrix[ 1 ][ 1 ], eigenMatrix[ 1 ][ 2 ],
									 eigenMatrix[ 2 ][ 0 ], eigenMatrix[ 2 ][ 1 ], eigenMatrix[ 2 ][ 2 ] };

			const Vec3f translation = centerOfMassA - ( rotation * centerOfMassB );

			const Mat4f rotationMatrix = Mat4f( rotation[ 0 ][ 0 ],
												rotation[ 0 ][ 1 ],
												rotation[ 0 ][ 2 ],
												0,
												rotation[ 1 ][ 0 ],
												rotation[ 1 ][ 1 ],
												rotation[ 1 ][ 2 ],
												0,
												rotation[ 2 ][ 0 ],
												rotation[ 2 ][ 1 ],
												rotation[ 2 ][ 2 ],
												0,
												0,
												0,
												0,
												1. );

			const Mat4f transformationMatrix = Mat4f( rotation[ 0 ][ 0 ],
													  rotation[ 0 ][ 1 ],
													  rotation[ 0 ][ 2 ],
													  0,
													  rotation[ 1 ][ 0 ],
													  rotation[ 1 ][ 1 ],
													  rotation[ 1 ][ 2 ],
													  0,
													  rotation[ 2 ][ 0 ],
													  rotation[ 2 ][ 1 ],
													  rotation[ 2 ][ 2 ],
													  0,
													  translation.x,
													  translation.y,
													  translation.z,
													  1. );

			const float rmsd = float(
				Analysis::RMSD::computeRMSD( residuePositionsA, residuePositionsB, MAT4F_ID, rotationMatrix ) );

			if ( !result.isValid() || rmsd < result.rmsd
				 || ( rmsd == result.rmsd && positionsCount > result.positionsCount ) )
			{
				result.pathIndex			= i;
				result.rmsd					= rmsd;
				result.transformationMatrix = transformationMatrix;
				result.positionsCount		= positionsCount;
			}
		}

		return result;
	}

	float CEAlign::_computeRMSDOnAtomOfPath( const Model::Molecule & p_staticMolecule,
											 const Model::Molecule & p_mobileMolecule,
											 const Path &			 p_path,
											 const int				 p_windowSize,
											 const Mat4f &			 p_transfoMatrix )
	{
		const size_t	   pathSize				 = p_path.size();
		std::vector<Vec3f> residuePositionsPath1 = std::vector<Vec3f>();
		residuePositionsPath1.reserve( p_staticMolecule.getAtomCount() );
		std::vector<Vec3f> residuePositionsPath2 = std::vector<Vec3f>();
		residuePositionsPath2.reserve( p_mobileMolecule.getAtomCount() );

		for ( int i = 0; i < pathSize; i++ )
		{
			for ( int iWinSize = 0; iWinSize < p_windowSize; iWinSize++ )
			{
				const uint iResidue1 = uint( p_path[ i ].first + iWinSize );
				const uint iResidue2 = uint( p_path[ i ].second + iWinSize );

				if ( iResidue1 >= p_staticMolecule.getResidueCount()
					 || iResidue2 >= p_mobileMolecule.getResidueCount() )
				{
					break;
				}

				const Model::Residue * const residue1 = p_staticMolecule.getResidue( iResidue1 );
				const Model::Residue * const residue2 = p_mobileMolecule.getResidue( iResidue2 );

				if ( residue1 == nullptr || residue2 == nullptr )
					continue;

				for ( uint i = residue1->getIndexFirstAtom();
					  i < residue1->getIndexFirstAtom() + residue1->getAtomCount();
					  i++ )
				{
					residuePositionsPath1.emplace_back( p_staticMolecule.getCurrentAtomPositionFrame()[ i ] );
				}

				for ( uint i = residue2->getIndexFirstAtom();
					  i < residue2->getIndexFirstAtom() + residue2->getAtomCount();
					  i++ )
				{
					residuePositionsPath2.emplace_back( p_mobileMolecule.getCurrentAtomPositionFrame()[ i ] );
				}
			}
		}

		return Analysis::RMSD::computeRMSD( residuePositionsPath1, residuePositionsPath2, MAT4F_ID, p_transfoMatrix );
	}

	// Given two sets of 3D points, find the rotation + translation + scale
	// which best maps the first set to the second.
	// Source: http://en.wikipedia.org/wiki/Kabsch_algorithm

	// The input 3D points are stored as columns.
	Mat4f CEAlign::_find3DAffineTransform( const std::vector<Vec3f> & p_in,
										   const std::vector<Vec3f> & p_out,
										   const Vec3f &			  p_inCenterOfMass,
										   const Vec3f &			  p_outCenterOfMass )
	{
		if ( p_in.size() != p_out.size() )
			throw "Find3DAffineTransform(): input data mis-match";

		//////////////////////////////////////////////////////////////
		// Eigen
		//////////////////////////////////////////////////////////////

		Eigen::Matrix3Xd in	 = Eigen::Matrix3Xd( 3, p_in.size() );
		Eigen::Matrix3Xd out = Eigen::Matrix3Xd( 3, p_out.size() );

		for ( int i = 0; i < p_in.size(); i++ )
		{
			in.col( i ) << p_in[ i ][ 0 ], p_in[ i ][ 1 ], p_in[ i ][ 2 ];
			out.col( i ) << p_out[ i ][ 0 ], p_out[ i ][ 1 ], p_out[ i ][ 2 ];
		}
		Eigen::Affine3d A;
		A.linear()		= Eigen::Matrix3d::Identity( 3, 3 );
		A.translation() = Eigen::Vector3d::Zero();

		if ( in.cols() != out.cols() )
			throw "Find3DAffineTransform(): input data mis-match";

		// First find the scale, by finding the ratio of sums of some distances,
		// then bring the datasets to the same scale.
		double dist_in = 0, dist_out = 0;
		for ( int col = 0; col < in.cols() - 1; col++ )
		{
			dist_in += ( in.col( col + 1 ) - in.col( col ) ).norm();
			dist_out += ( out.col( col + 1 ) - out.col( col ) ).norm();
		}

		if ( dist_in <= 0 || dist_out <= 0 )
			return MAT4F_ID;

		double scale = dist_out / dist_in;
		scale		 = 1;
		out /= scale;

		// Find the centroids then shift to the origin
		Eigen::Vector3d in_ctr	= Eigen::Vector3d::Zero();
		Eigen::Vector3d out_ctr = Eigen::Vector3d::Zero();
		for ( int col = 0; col < in.cols(); col++ )
		{
			in_ctr += in.col( col );
			out_ctr += out.col( col );
		}
		in_ctr /= in.cols();
		out_ctr /= out.cols();
		for ( int col = 0; col < in.cols(); col++ )
		{
			in.col( col ) -= in_ctr;
			out.col( col ) -= out_ctr;
		}

		// SVD
		Eigen::MatrixXd					  Cov = in * out.transpose();
		Eigen::JacobiSVD<Eigen::MatrixXd> svd( Cov, Eigen::ComputeThinU | Eigen::ComputeThinV );

		// Find the rotation
		double d = ( svd.matrixV() * svd.matrixU().transpose() ).determinant();
		if ( d > 0 )
			d = 1.0;
		else
			d = -1.0;
		Eigen::Matrix3d I = Eigen::Matrix3d::Identity( 3, 3 );
		I( 2, 2 )		  = d;
		Eigen::Matrix3d R = svd.matrixV() * I * svd.matrixU().transpose();

		// The final transform
		A.linear()						   = scale * R;
		const Eigen::Vector3d rotatedInCtr = R * in_ctr;
		A.translation()					   = scale * ( out_ctr - rotatedInCtr );

		Eigen::Matrix3d eigenRotation = A.rotation().matrix();
		Mat3f			rotation	  = _eigenMat3dToMat3f( eigenRotation );

		return Mat4f( rotation[ 0 ][ 0 ],
					  rotation[ 0 ][ 1 ],
					  rotation[ 0 ][ 2 ],
					  0,
					  rotation[ 1 ][ 0 ],
					  rotation[ 1 ][ 1 ],
					  rotation[ 1 ][ 2 ],
					  0,
					  rotation[ 2 ][ 0 ],
					  rotation[ 2 ][ 1 ],
					  rotation[ 2 ][ 2 ],
					  0,
					  0,
					  0,
					  0,
					  1. );

		//////////////////////////////////////////////////////////////
		// TODO VTX (WIP)
		//////////////////////////////////////////////////////////////

		//// Default output
		// Mat4f vtxA = MAT4F_ID;

		//// SVD
		// Matrix<float> vtxU;
		// std::vector<float>	vtxSigmas;
		// Matrix<float> vtxV;

		// Matrix<float> vtxCov = Matrix<float>( 3, 3 );

		// for ( int i = 0; i < 3; i++ )
		//{
		//	double sumX = 0.f;
		//	double sumY = 0.f;
		//	double sumZ = 0.f;

		//	for ( int j = 0; j < p_in.size(); j++ )
		//	{
		//		double baseValue = 0;

		//		if ( i == 0 )
		//			baseValue = p_out[ j ].x;
		//		else if ( i == 1 )
		//			baseValue = p_out[ j ].y;
		//		else if ( i == 2 )
		//			baseValue = p_out[ j ].z;

		//		sumX += baseValue * p_in[ j ].x;
		//		sumY += baseValue * p_in[ j ].y;
		//		sumZ += baseValue * p_in[ j ].z;
		//	}
		//	vtxCov.set( i, 0, sumX );
		//	vtxCov.set( i, 1, sumY );
		//	vtxCov.set( i, 2, sumZ );
		//}

		// Util::getSVD( vtxCov, vtxU, vtxSigmas, vtxV );

		// const Matrix<float> vtxTransposedU = Util::transpose( vtxU );

		// double vtxD = Util::getDeterminant( Util::matMult( vtxV, vtxTransposedU ) );
		// if ( vtxD > 0 )
		//	vtxD = 1.0;
		// else
		//	vtxD = -1.0;

		// Matrix<float> vtxI = Matrix<float>( 3, 3 );
		// vtxI.set( 2, 2, vtxD );

		// const Matrix<float> rotation = Util::matMult( vtxV, Util::matMult( vtxI, vtxTransposedU )
		// );

		// return Mat4f( rotation.get( 0, 0 ),
		//			  rotation.get( 0, 1 ),
		//			  rotation.get( 0, 2 ),
		//			  0,
		//			  rotation.get( 1, 0 ),
		//			  rotation.get( 1, 1 ),
		//			  rotation.get( 1, 2 ),
		//			  0,
		//			  rotation.get( 2, 0 ),
		//			  rotation.get( 2, 1 ),
		//			  rotation.get( 2, 2 ),
		//			  0,
		//			  0,
		//			  0,
		//			  0,
		//			  1. );
	}

	Eigen::Matrix3d CEAlign::_mat3fToEigenMat3d( const Mat3f & _mat )
	{
		Eigen::Matrix3d eigenMatrix;

		for ( int i = 0; i < 3; i++ )
		{
			for ( int j = 0; j < 3; j++ )
			{
				eigenMatrix << _mat[ i ][ j ];
			}
		}

		return eigenMatrix;
	}
	Mat3f CEAlign::_eigenMat3dToMat3f( const Eigen::Matrix3d & _eigenMat )
	{
		return { _eigenMat.row( 0 )[ 0 ], _eigenMat.row( 0 )[ 1 ], _eigenMat.row( 0 )[ 2 ],
				 _eigenMat.row( 1 )[ 0 ], _eigenMat.row( 1 )[ 1 ], _eigenMat.row( 1 )[ 2 ],
				 _eigenMat.row( 2 )[ 0 ], _eigenMat.row( 2 )[ 1 ], _eigenMat.row( 2 )[ 2 ] };
	}

} // namespace VTX::Analysis::StructuralAlignmentMethod
