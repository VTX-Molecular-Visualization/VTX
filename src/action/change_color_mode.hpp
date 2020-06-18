#ifndef __VTX_ACTION_CHANGE_COLOR_MODE__
#define __VTX_ACTION_CHANGE_COLOR_MODE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"
#include <magic_enum.hpp>

namespace VTX
{
	namespace Action
	{
		class ChangeColorMode : public BaseAction
		{
		  public:
			explicit ChangeColorMode( const Generic::COLOR_MODE p_mode ) : _mode( p_mode ) {}

			virtual void execute() override
			{
				VTX_SETTING().colorMode = _mode;
				for ( const Object3D::Scene::PairMoleculePtrFloat & pair : VTXApp::get().getScene().getMolecules() )
				{
					pair.first->setColorMode();
				}
			};

			virtual void displayUsage() override { VTX_INFO( "ATOM|RESIDUE|CHAIN|PROTEIN" ); }

		  private:
			const Generic::COLOR_MODE _mode;
		};
	} // namespace Action
} // namespace VTX
#endif
