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
		class RMSDData
		{
		  public:
			RMSDData( const Model::Molecule * const p_firstMolecule,
					  const Model::Molecule * const p_secondMolecule,
					  const double					p_rmsd ) :
				firstMolecule( p_firstMolecule ),
				secondMolecule( p_secondMolecule ), rmsd( p_rmsd )
			{
			}

			const Model::Molecule * const firstMolecule;
			const Model::Molecule * const secondMolecule;
			const double				  rmsd;
		};

	  public:
		static void computeRMSD( const Model::Molecule * const p_firstMolecule,
								 const Model::Molecule * const p_secondMolecule,
								 const bool					   p_considerTransform = true );

		static double internalRMSD( const Model::Molecule & p_firstMolecule,
									const Model::Molecule & p_secondMolecule,
									const bool				p_considerTransform = true );

		static double internalRMSD( const std::vector<Vec3f> & p_vectorPositionsA,
									const std::vector<Vec3f> & p_vectorPositionsB,
									const Mat4f &			   p_transformA = MAT4F_ID,
									const Mat4f &			   p_transformB = MAT4F_ID );
	};

} // namespace VTX::Analysis
#endif
