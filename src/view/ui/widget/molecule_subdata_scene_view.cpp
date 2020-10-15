#include "molecule_subdata_scene_view.hpp"
#include "style.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				void MoleculeSubDataSceneView::setData( const Model::Chain & p_chain ) { fillData( p_chain, p_chain.getName(), VTX::Style::IconConst::get().CHAIN_SYMBOL ); }
				void MoleculeSubDataSceneView::setData( const Model::Residue & p_residue )
				{
					fillData( p_residue, p_residue.getSymbolName(), VTX::Style::IconConst::get().RESIDUE_SYMBOL );
				}
				void MoleculeSubDataSceneView::setData( const Model::Atom & p_atom ) { fillData( p_atom, p_atom.getSymbolName(), VTX::Style::IconConst::get().RESIDUE_SYMBOL ); }
			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
