#ifndef __VTX_ACTION_CHANGE_REPRESENTATION__
#define __VTX_ACTION_CHANGE_REPRESENTATION__

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
		class ChangeRepresentation : public BaseAction
		{
		  public:
			explicit ChangeRepresentation() {}
			explicit ChangeRepresentation( const View::MOLECULE_REPRESENTATION p_representation ) :
				_representation( p_representation )
			{
			}

			virtual void setParameters( std::vector<std::string> & p_parameters ) override
			{
				_representation = magic_enum::enum_cast<View::MOLECULE_REPRESENTATION>( p_parameters.at( 1 ) ).value();
			}

			virtual void execute() override
			{
				Setting::Rendering::representation = _representation;
				for ( const Object3D::Scene::PairMoleculePtrFloat & pair : VTXApp::get().getScene().getMolecules() )
				{
					pair.first->setRepresentation();
				}
			};

			virtual void displayUsage() override { VTX_INFO( "BALL_AND_STICK|VAN_DER_WAALS|STICK|SAS" ); }

		  private:
			View::MOLECULE_REPRESENTATION _representation = Setting::Rendering::representation;
		};
	} // namespace Action
} // namespace VTX
#endif
