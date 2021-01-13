#ifndef __VTX_MODEL_GENERATED_MOLECULE__
#define __VTX_MODEL_GENERATED_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "molecule.hpp"
#include "selection.hpp"

namespace VTX
{
	namespace Model
	{
		class GeneratedMolecule : public Molecule
		{
			VTX_MODEL

		  public:
			void copyFromSelection( const Model::Selection & p_selection );
			void extractFromSelection( const Model::Selection & p_selection ) { copyFromSelection( p_selection ); };

		  protected:
			GeneratedMolecule();
		};
	} // namespace Model
} // namespace VTX

#endif
