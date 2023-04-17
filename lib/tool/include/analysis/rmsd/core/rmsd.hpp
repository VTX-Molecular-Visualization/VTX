#ifndef __VTX_TOOL_ANALYSIS_RMSD_CORE_RMSD__
#define __VTX_TOOL_ANALYSIS_RMSD_CORE_RMSD__

#include <string>
#include <util/constants.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Model
{
	class Molecule;
	class Selection;
} // namespace VTX::Model

namespace VTX::Tool::Analysis::RMSD::Core
{
	class RMSDData
	{
	  public:
		RMSDData() {};

		const Model::Molecule * getFirstMolecule() const { return _firstMolecule; }
		void setFirstMolecule( const Model::Molecule * const p_firstMolecule ) { _firstMolecule = p_firstMolecule; }

		const Model::Molecule * getSecondMolecule() const { return _secondMolecule; }
		void setSecondMolecule( const Model::Molecule * const p_secondMolecule ) { _secondMolecule = p_secondMolecule; }

		bool hasResidueCount() const { return _residueCount > 0; }
		uint getResidueCount() const { return _residueCount; }
		void setResidueCount( const uint p_residueCount ) { _residueCount = p_residueCount; }

		bool hasAtomCount() const { return _atomCount > 0; }
		uint getAtomCount() const { return _atomCount; }
		void setAtomCount( const uint p_atomCount ) { _atomCount = p_atomCount; }

		double getRMSD() const { return _rmsd; }
		void   setRMSD( const double p_rmsd ) { _rmsd = p_rmsd; }

	  private:
		const Model::Molecule * _firstMolecule	= nullptr;
		const Model::Molecule * _secondMolecule = nullptr;
		uint					_residueCount	= 0;
		uint					_atomCount		= 0;

		double _rmsd = -1.;
	};

	void callRMSDComputation( const Model::Molecule * const p_firstMolecule,
							  const Model::Molecule * const p_secondMolecule,
							  const bool					p_considerTransform = true );

	void callRMSDComputation( const Model::Selection & p_selection, const bool p_considerTransform = true );

	double computeRMSD( const Model::Molecule & p_firstMolecule,
						const Model::Molecule & p_secondMolecule,
						const bool				p_considerTransform = true );

	double computeRMSD( const std::vector<Vec3f> & p_vectorPositionsA,
						const std::vector<Vec3f> & p_vectorPositionsB,
						const Mat4f &			   p_transformA = MAT4F_ID,
						const Mat4f &			   p_transformB = MAT4F_ID );

	std::string getLogString( const RMSDData & p_data );

} // namespace VTX::Tool::Analysis::RMSD::Core
#endif
