#include "molecule_sequence.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void MoleculeSequence::_drawContent()
			{
				ImGui::Text( _model->getName().c_str() );
				ImGui::Text( ( "/" + _model->getPath().filename().stem().string() ).c_str() );

				for ( Model::Chain * const chain : _model->getChains() )
				{
					ImGui::Text( chain->getSequence().c_str() );
				}
			}

		} // namespace UI

	} // namespace View
} // namespace VTX
