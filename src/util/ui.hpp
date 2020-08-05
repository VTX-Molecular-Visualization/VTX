#ifndef __VTX_UTIL_UI__
#define __VTX_UTIL_UI__

#ifdef _MSC_VER
#pragma once
#endif

#include "action/representable.hpp"
#include "generic/base_representable.hpp"
#include "localization/language.hpp"
#include "vtx_app.hpp"
#include <imgui/imgui.h>

namespace VTX
{
	namespace Util
	{
		namespace UI
		{
			static void drawRepresentations( Generic::BaseRepresentable & p_representable,
											 Model::Molecule &			  p_molecule )
			{
				if ( ImGui::CollapsingHeader( LOCALE( "View.Representation" ), ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					for ( std::set<Generic::REPRESENTATION>::const_iterator & r
						  = p_representable.getRepresentations().begin();
						  r != p_representable.getRepresentations().end();
						  ++r )
					{
						ImGui::Text( std::to_string( (int)( *r ) ).c_str() );
						ImGui::PushID( (int)( *r ) );
						if ( ImGui::Button( LOCALE( "View.Delete" ) ) )
						{
							VTX_ACTION( new Action::RepresentableRemoveRepresentation(
								p_representable, p_molecule, (Generic::REPRESENTATION)*r ) );
						}
						ImGui::PopID();
					}

					const char * representations[] = { LOCALE( "Enum.Representation.BallsAndSticks" ),
													   LOCALE( "Enum.Representation.VanDerWaals" ),
													   LOCALE( "Enum.Representation.Sticks" ),
													   LOCALE( "Enum.Representation.SAS" ),
													   LOCALE( "Enum.Representation.Cartoon" ) };
					int			 representation;
					if ( ImGui::Combo( LOCALE( "View.Add" ), &representation, representations, 5 ) )
					{
						VTX_ACTION( new Action::RepresentableAddRepresentation(
							p_representable, p_molecule, (Generic::REPRESENTATION)representation ) );
					}
				}
			}
		} // namespace UI
	}	  // namespace Util
} // namespace VTX

#endif
