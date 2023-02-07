#ifndef __VTX_ANALYSIS_RMSD__
#define __VTX_ANALYSIS_RMSD__

#include <util/types.hpp>
#include <util/constants.hpp>
#include <vector>

namespace VTX::Model
{
	class Molecule;
	class Selection;
} // namespace VTX::Model

namespace VTX::Analysis
{
	class RMSD
	{
	  public:
		class RMSDData
		{
		  public:
			RMSDData() {};

			const Model::Molecule * getFirstMolecule() const { return _firstMolecule; }
			void setFirstMolecule( const Model::Molecule * const p_firstMolecule ) { _firstMolecule = p_firstMolecule; }

			const Model::Molecule * getSecondMolecule() const { return _secondMolecule; }
			void					setSecondMolecule( const Model::Molecule * const p_secondMolecule )
			{
				_secondMolecule = p_secondMolecule;
			}

			bool hasResidueCount() const { return _residueCount > 0; }
			uint getResidueCount() const { return _residueCount; }
			void setResidueCount( const uint p_residueCount ) { _residueCount = p_residueCount; }

			bool hasAtomCount() const { return _atomCount > 0; }
			uint getAtomCount() const { return _atomCount; }
			void setAtomCount( const uint p_atomCount ) { _atomCount = p_atomCount; }

			double getRMSD() const { return _rmsd; }
			void   setRMSD( const double p_rmsd ) { _rmsd = p_rmsd; }

		  private:
			const Model::Molecule * _firstMolecule;
			const Model::Molecule * _secondMolecule;
			uint					_residueCount = 0;
			uint					_atomCount	  = 0;

			double _rmsd;
		};

	  public:
		static void callRMSDComputation( const Model::Molecule * const p_firstMolecule,
										 const Model::Molecule * const p_secondMolecule,
										 const bool					   p_considerTransform = true );

		static void callRMSDComputation( const Model::Selection & p_selection, const bool p_considerTransform = true );

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
