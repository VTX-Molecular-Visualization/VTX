#include <tool/structalign/backend/align.hpp>

namespace VTX::Tool::Structalign::backend
{

	namespace
	{
		Vec3f computeResidueCenterOfMass( const std::span<const Vec3f> p_atomPositions )
		{
			Vec3f result { 0.f, 0.f, 0.f };
			for ( const auto & it_pos : p_atomPositions )
			{
				result += it_pos;
			}
			result /= p_atomPositions.size();
			return result
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

		std::vector<Vec3f> staticMoleculeResiduePositions
			= _generateResiduePositionsVector( p_staticMolecule, castedParameters );
		std::vector<Vec3f> mobileMoleculeResiduePositions
			= _generateResiduePositionsVector( p_mobileMolecule, castedParameters );

		// Calculate distance matrix
		const Math::Matrix<float> distanceMatrixStaticMol = _computeDistanceMatrix( staticMoleculeResiduePositions );
		const Math::Matrix<float> distanceMatrixMobileMol = _computeDistanceMatrix( mobileMoleculeResiduePositions );

		// Calculate CE similarities
		const Math::Matrix<float> scoreMatrix
			= _computeScoreMatrix( distanceMatrixStaticMol, distanceMatrixMobileMol, correctedParameters );

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

	std::vector<Vec3f> CEAlign::_generateResiduePositionsVector(
		const Model::Molecule &	 p_molecule,
		const CustomParameters & p_parameters
	)
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
} // namespace VTX::Tool::Structalign::backend
