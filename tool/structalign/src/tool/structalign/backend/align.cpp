#include <tool/structalign/backend/align.hpp>
#include <util/math.hpp>
#include <util/math/matrix.hpp>

namespace VTX::Tool::Structalign::backend
{

	namespace
	{
		using Path = std::vector<std::pair<VTX::Index, VTX::Index>>;

		Vec3f computeResidueCenterOfMass( const std::span<const Vec3f> p_atomPositions )
		{
			Vec3f result { 0.f, 0.f, 0.f };
			for ( const auto & it_pos : p_atomPositions )
			{
				result += it_pos;
			}
			result /= p_atomPositions.size();
			return result;
		}

		std::vector<Vec3f> generateResiduePositionsVector( const Structure & p_system )
		{
			std::vector<Vec3f> residuePositionsVector = std::vector<Vec3f>();
			residuePositionsVector.resize( p_system.topology.get().getResidueCount() );

			size_t index = 0;

			for ( uint it_resIdx = 0; it_resIdx < p_system.topology.get().getResidueCount(); it_resIdx++ )
			{
				bool considerResidue = p_system.includeSolvent
									   || p_system.topology.get().getResidueCategory( it_resIdx )
											  != VTX::Core::ChemDB::Category::TYPE::SOLVENT;
				if ( not considerResidue )
				{
					continue;
				}
				const auto &   firstAtomIdx = p_system.topology.get().getResidueFirstAtom( it_resIdx );
				const uint32_t atomCount	= p_system.topology.get().getResidueAtomCount( it_resIdx );
				const uint32_t lastAtomIdx	= atomCount + firstAtomIdx;
				if ( p_system.visibility.has_value() )
				{
					bool atLeastOneAtomVisible = false;

					for ( uint32_t it_resAtomIdx = firstAtomIdx; it_resAtomIdx < lastAtomIdx; it_resAtomIdx++ )
					{
						if ( p_system.visibility->atoms.test( it_resAtomIdx ) )
						{
							atLeastOneAtomVisible = true;
							break;
						}
					}
					considerResidue &= atLeastOneAtomVisible;
				}

				if ( considerResidue )
				{
					auto start						= std::next( p_system.positions.begin(), firstAtomIdx );
					residuePositionsVector[ index ] = computeResidueCenterOfMass( { start, atomCount } );
					index++;
				}
			}

			residuePositionsVector.resize( index );

			return residuePositionsVector;
		}

		Util::Math::Matrix<float> computeDistanceMatrix( const std::vector<Vec3f> & p_positions )
		{
			// VG : Need to check if the distance formula used is the (7) from page 2

			Util::Math::Matrix<float> distanceMatrix
				= Util::Math::Matrix<float>( p_positions.size(), p_positions.size() );

			for ( size_t iRow = 0; iRow < p_positions.size(); iRow++ )
			{
				for ( size_t iCol = iRow; iCol < p_positions.size(); iCol++ )
				{
					const float dist = VTX::Util::Math::distance( p_positions[ iRow ], p_positions[ iCol ] );

					distanceMatrix.set( iRow, iCol, dist );
					distanceMatrix.set( iCol, iRow, dist );
				}
			}

			return distanceMatrix;
		}

		VTX::Util::Math::Matrix<float> computeScoreMatrix(
			const VTX::Util::Math::Matrix<float> & p_distanceMatrix1,
			const VTX::Util::Math::Matrix<float> & p_distanceMatrix2,
			const CEAlignParameters &			   p_parameters
		)
		{
			VTX::Util::Math::Matrix<float> scoreMatrix = VTX::Util::Math::Matrix<float>(
				p_distanceMatrix1.getRowCount(), p_distanceMatrix2.getRowCount(), -1.f
			);

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
							score += fabsf(
								p_distanceMatrix1.get( iA + row, iA + col )
								- p_distanceMatrix2.get( iB + row, iB + col )
							);
						}
					}

					scoreMatrix.set( iA, iB, score / sumSize );
				}
			}

			return scoreMatrix;
		}
	} // namespace

	void align( const CEAlignParameters & p_params, CEAlignResults & p_results )
	{
		CEAlignParameters correctedParameters = p_params;

		// Ensure good value for windowSize
		const int minMolSize = std::min(
			p_params.staticStructure.topology.get().getResidueCount(),
			p_params.mobileStructure.topology.get().getResidueCount()
		);

		correctedParameters.windowSize = std::min( p_params.windowSize, static_cast<uint32_t>( sqrt( minMolSize ) ) );

		std::vector<Vec3f> staticMoleculeResiduePositions = generateResiduePositionsVector( p_params.staticStructure );
		std::vector<Vec3f> mobileMoleculeResiduePositions = generateResiduePositionsVector( p_params.mobileStructure );

		// Calculate distance matrix
		const VTX::Util::Math::Matrix<float> distanceMatrixStaticMol
			= computeDistanceMatrix( staticMoleculeResiduePositions );
		const VTX::Util::Math::Matrix<float> distanceMatrixMobileMol
			= computeDistanceMatrix( mobileMoleculeResiduePositions );

		// Calculate CE similarities
		const VTX::Util::Math::Matrix<float> scoreMatrix
			= computeScoreMatrix( distanceMatrixStaticMol, distanceMatrixMobileMol, correctedParameters );

		// Calculate Top N Paths
		const std::vector<Path> bestPaths
			= _findPath( scoreMatrix, distanceMatrixStaticMol, distanceMatrixMobileMol, correctedParameters );

		const BestPathResult bestPathResult = _pickBest(
			staticMoleculeResiduePositions, mobileMoleculeResiduePositions, bestPaths, correctedParameters
		);

		const double atomRmsdOnPath = _computeRMSDOnAtomOfPath(
			p_staticMolecule,
			p_mobileMolecule,
			bestPaths[ bestPathResult.pathIndex ],
			p_parameters.windowSize,
			bestPathResult.transformationMatrix
		);

		StructuralAlignment::AlignmentResult result
			= StructuralAlignment::AlignmentResult( &p_staticMolecule, &p_mobileMolecule );

		result.transformationMatrix = bestPathResult.transformationMatrix;
		result.alignedResiduesRMSD	= atomRmsdOnPath;
		result.alignedResidueCount	= bestPathResult.positionsCount;

		return result;
	}

} // namespace VTX::Tool::Structalign::backend
