#ifndef __VTX_ANALYSIS_RMSD__
#define __VTX_ANALYSIS_RMSD__

#include "define.hpp"
#include <vector>

namespace VTX::Model
{
	class Molecule;
}

namespace VTX::Analysis
{
	class RMSD
	{
	  public:
		static double computeRMSD( const Model::Molecule & p_firstMolecule,
								   const Model::Molecule & p_secondMolecule,
								   const bool			   p_considerTransform = true );

		static double computeRMSD( const std::vector<Vec3f> & p_vectorPositionsA,
								   const std::vector<Vec3f> & p_vectorPositionsB,
								   const Mat4f &			  p_transformA = MAT4F_ID,
								   const Mat4f &			  p_transformB = MAT4F_ID );
	};

} // namespace VTX::Analysis
#endif
