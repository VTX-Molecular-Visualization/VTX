#include "molecule.hpp"
#include "action/scale.hpp"
#include "action/translate.hpp"
#include "vtx_app.hpp"
#include <glm/gtx/euler_angles.hpp>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void Molecule::_draw()
			{
				ImGui::PushID( ( "ViewMolecule" + std::to_string( _getModel().getId() ) ).c_str() );
				if ( ImGui::CollapsingHeader( _getModel().getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					if ( ImGui::CollapsingHeader( LOCALE( "View.Data" ), ImGuiTreeNodeFlags_DefaultOpen ) )
					{
						ImGui::Text( "Chains: %d", _getModel().getChainCount() );
						ImGui::Text( "Residues: %d", _getModel().getResidueCount() );
						ImGui::Text( "Atoms: %d", _getModel().getAtomCount() );
						ImGui::Text( "Bonds: %d", _getModel().getBondCount() / 2 );
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Transform" ) ) )
					{
						ImGui::Text( LOCALE( "View.Transform.Position" ) );
						ImGui::PushID( "Position" );
						Vec3f translation = _getModel().getTransform().getTranslationVector();
						float t[]		  = { translation.x, translation.y, translation.z };
						if ( ImGui::InputFloat3( "Position", t, 2 ) )
						{
							VTXApp::get().getActionManager().execute(
								new Action::Translate( _getModel(), Vec3f( t[ 0 ], t[ 1 ], t[ 2 ] ) ) );
						}
						ImGui::PopID();
						ImGui::Text( LOCALE( "View.Transform.Scale" ) );
						Vec3f scale = _getModel().getTransform().getScaleVector();
						float s		= scale.x;
						ImGui::PushID( "Scale" );
						if ( ImGui::InputFloat( "Scale", &s, 1.f ) )
						{ VTXApp::get().getActionManager().execute( new Action::Scale( _getModel(), s ) ); }
						ImGui::PopID();
					}

#ifdef _DEBUG
					if ( ImGui::CollapsingHeader( "Transform-debug" ) )
					{
						float *x, *y, *z;

						ImGui::PushID( "Debug" );

						ImGui::Text( "Translation" );
						const Mat4f & translation = _getModel().getTransform().getTranslation();
						x						  = (float *)&translation[ 0 ];
						y						  = (float *)&translation[ 1 ];
						z						  = (float *)&translation[ 2 ];
						ImGui::PushID( "Translation" );
						ImGui::InputFloat3( "X", x, 2 );
						ImGui::InputFloat3( "Y", y, 2 );
						ImGui::InputFloat3( "Z", z, 2 );
						ImGui::PopID();

						ImGui::Text( "Rotation" );
						const Mat4f & rotation = _getModel().getTransform().getRotation();
						x					   = (float *)&rotation[ 0 ];
						y					   = (float *)&rotation[ 1 ];
						z					   = (float *)&rotation[ 2 ];
						ImGui::PushID( "Rotation" );
						ImGui::InputFloat3( "X", x, 2 );
						ImGui::InputFloat3( "Y", y, 2 );
						ImGui::InputFloat3( "Z", z, 2 );
						ImGui::PopID();

						ImGui::Text( "Scale" );
						const Mat4f & scale = _getModel().getTransform().getScale();
						x					= (float *)&scale[ 0 ];
						y					= (float *)&scale[ 1 ];
						z					= (float *)&scale[ 2 ];
						ImGui::PushID( "Scale" );
						ImGui::InputFloat3( "X", x, 2 );
						ImGui::InputFloat3( "Y", y, 2 );
						ImGui::InputFloat3( "Z", z, 2 );
						ImGui::PopID();

						ImGui::PopID();
					}
#endif
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
