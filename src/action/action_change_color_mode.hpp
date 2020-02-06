#ifndef __VTX_ACTION_CHANGE_COLOR_MODE__
#define __VTX_ACTION_CHANGE_COLOR_MODE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/model_molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionChangeColorMode : public BaseAction
		{
		  public:
			explicit ActionChangeColorMode( const View::MOLECULE_COLOR_MODE p_mode ) : _mode( p_mode ) {}

			virtual void execute() override
			{
				Setting::Rendering::colorMode = _mode;
				for ( Model::ModelMolecule * const molecule : VTXApp::get().getScene().getMolecules() )
				{
					molecule->setColorMode();
				}
			};

		  private:
			const View::MOLECULE_COLOR_MODE _mode;
		};
	} // namespace Action
} // namespace VTX
#endif
