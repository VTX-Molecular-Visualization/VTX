#ifndef __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_METHOD_CE_ALIGN__
#define __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_METHOD_CE_ALIGN__

#include "analysis/structural_alignment/core/struct/residue_center_of_mass_data_set.hpp"
#include "analysis/structural_alignment/core/structural_alignment.hpp"
#include "old_tool/analysis/matrix.hpp"
#include <Eigen/Geometry>
#include <util/constants.hpp>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::Model
{
	class Molecule;
	class Residue;
} // namespace VTX::Model

namespace VTX::Tool::Analysis::StructuralAlignment::Core::Method
{
	class CEAlign : public StructuralAlignment::AlignmentMethod
	{
	  public:
		class CustomParameters : public StructuralAlignment::AlignmentParameters
		{
		  public:
			CustomParameters();

			float d0 = 3.f;
			float d1 = 4.f;
		};

	  private:
		using AlignedFragmentPair = std::pair<size_t, size_t>;
		using Path				  = std::vector<AlignedFragmentPair>;

		class BestPathResult
		{
		  public:
			int	   pathIndex			= -1;
			Mat4f  transformationMatrix = MAT4F_ID;
			float  rmsd					= FLOAT_MAX;
			size_t positionsCount		= 0;

			bool isValid() const { return pathIndex >= 0; }
		};

	  public:
		StructuralAlignment::AlignmentResult compute(
			const Model::Molecule &							 p_staticMolecule,
			Model::Molecule &								 p_mobileMolecule,
			const StructuralAlignment::AlignmentParameters & p_parameters ) override;

	  private:
		// OK
		static std::vector<Vec3f> _generateResiduePositionsVector( const Model::Molecule &	p_molecule,
																   const CustomParameters & p_parameters );

		// OK
		static VTX::Analysis::Matrix<float> _computeDistanceMatrix( const std::vector<Vec3f> & p_atomPositions );

		// OK
		static VTX::Analysis::Matrix<float> _computeScoreMatrix( const VTX::Analysis::Matrix<float> & p_distanceMatrix1,
																 const VTX::Analysis::Matrix<float> & p_distanceMatrix2,
																 const CustomParameters &			  p_parameters );

		std::vector<Path> _findPath( const VTX::Analysis::Matrix<float> & p_scoreMatrix,
									 const VTX::Analysis::Matrix<float> & p_distanceMatrixA,
									 const VTX::Analysis::Matrix<float> & p_distanceMatrixB,
									 const CustomParameters &			  p_parameters );

		BestPathResult _pickBest( const std::vector<Vec3f> & p_residuePositionsA,
								  const std::vector<Vec3f> & p_residuePositionsB,
								  const std::vector<Path> &	 p_results,
								  const CustomParameters &	 p_parameters );

		Mat4f _find3DAffineTransform( const std::vector<Vec3f> & p_in,
									  const std::vector<Vec3f> & p_out,
									  const Vec3f &				 p_inCenterOfMass,
									  const Vec3f &				 p_outCenterOfMass );

		Eigen::Matrix3d _mat3fToEigenMat3d( const Mat3f & _mat );
		Mat3f			_eigenMat3dToMat3f( const Eigen::Matrix3d & _eigenMat );

		static Vec3f _computeResidueCenterOfMass( const Model::Residue & p_residue );

		static float _computeRMSDOnAtomOfPath( const Model::Molecule & p_staticMolecule,
											   const Model::Molecule & p_mobileMolecule,
											   const Path &			   p_path,
											   const int			   p_windowSize,
											   const Mat4f &		   transformationMatrix = MAT4F_ID );

		static Struct::ResidueCenterOfMassDataSet _residuePositionsDataSet;
	};

} // namespace VTX::Tool::Analysis::StructuralAlignment::Core::Method

#endif
