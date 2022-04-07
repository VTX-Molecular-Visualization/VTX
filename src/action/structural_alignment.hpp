#ifndef __VTX_ACTION_STRUCTURAL_ALIGNMENT__
#define __VTX_ACTION_STRUCTURAL_ALIGNMENT__

#include "action/base_action.hpp"
#include "model/molecule.hpp"
#include "tool/logger.hpp"
#include <string>
#include <cmath>

namespace VTX::Action::StructuralAlignment
{
	class ComputeRMSD : public BaseAction
	{
	  public:
		explicit ComputeRMSD( const Model::Molecule & p_firstMolecule, const Model::Molecule & p_secondMolecule ) :
			_firstMolecule( p_firstMolecule ), _secondMolecule( p_secondMolecule )
		{
		}

		virtual void execute() override 
		{
			const size_t minAtomLength = _firstMolecule.getAtomCount() < _secondMolecule.getAtomCount()
											 ? _firstMolecule.getAtomCount()
											 : _secondMolecule.getAtomCount();

			float rmsd = 0;

			const Model::Molecule::AtomPositionsFrame & frame1 = _firstMolecule.getAtomPositionFrame( 0 );
			const Model::Molecule::AtomPositionsFrame & frame2 = _secondMolecule.getAtomPositionFrame( 0 );


			for ( uint i = 0; i < minAtomLength; i++ )
			{
				rmsd += Util::Math::distance( frame1[ i ], frame2[ i ] ) / minAtomLength;
			}

			rmsd = sqrtf( rmsd );

			VTX_INFO( "RMSD : " + std::to_string( rmsd ));
		}

	  private:
		const Model::Molecule & _firstMolecule;
		const Model::Molecule & _secondMolecule;
	};

	class ComputeStructuralAlignment : public BaseAction
	{
	  public:
		explicit ComputeStructuralAlignment( const Model::Molecule & p_firstMolecule,
											 const Model::Molecule & p_secondMolecule ) :
			_firstMolecule( p_firstMolecule ),
			_secondMolecule( p_secondMolecule )
		{
		}

		virtual void execute() override { VTX_INFO( "Compute Structural alignment" ); }

	  private:
		const Model::Molecule & _firstMolecule;
		const Model::Molecule & _secondMolecule;
	};

} // namespace VTX::Action::StructuralAlignment
#endif
