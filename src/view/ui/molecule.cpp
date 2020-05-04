#include "molecule.hpp"
#include "action/change_color_mode.hpp"
#include "action/colorable_change_color.hpp"
#include "action/molecule_change_fps.hpp"
#include "action/molecule_change_frame.hpp"
#include "action/molecule_change_is_playing.hpp"
#include "action/molecule_change_show_ion.hpp"
#include "action/molecule_change_show_solvent.hpp"
#include "action/molecule_compute_secondary_structure.hpp"
#include "action/selectable_unselect.hpp"
#include "action/transformable_rotate.hpp"
#include "action/transformable_set_scale.hpp"
#include "action/transformable_set_translation.hpp"
#include "action/visible_change_visibility.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void Molecule::_draw()
			{
				ImGui::PushID( ( "ViewMolecule" + std::to_string( _getModel().getId() ) ).c_str() );
				bool notClosed = true;
				if ( ImGui::CollapsingHeader(
						 _getModel().getName().c_str(), &notClosed, ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					bool isVisible = _getModel().isVisible();
					if ( ImGui::Checkbox( LOCALE( "View.Visible" ), &isVisible ) )
					{
						VTX_ACTION( new Action::VisibleChangeVisibility(
							_getModel(), (Action::VisibleChangeVisibility::VISIBILITY_MODE)isVisible ) );
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Data" ), ImGuiTreeNodeFlags_DefaultOpen ) )
					{
						ImGui::Text( LOCALE( "View.Chains%Count" ), _getModel().getChainCount() );
						ImGui::Text( LOCALE( "View.Residues%Count" ), _getModel().getResidueCount() );
						ImGui::Text( LOCALE( "View.Atoms%Count" ), _getModel().getAtomCount() );
						ImGui::Text( LOCALE( "View.Bonds%Count" ), _getModel().getBondCount() / 2 );
						ImGui::Text( LOCALE( "View.SecondaryStructure%State" ),
									 _getModel().secondaryStructureLoadedFromFile() ? LOCALE( "View.Loaded" )
																					: LOCALE( "View.Computed" ) );
						if ( _getModel().secondaryStructureLoadedFromFile() )
						{
							if ( ImGui::Button( LOCALE( "View.Compute" ) ) )
							{ VTX_ACTION( new Action::MoleculeComputeSecondaryStructure( _getModel() ) ); }
						}
					}
					if ( _getModel().getFrameCount() > 1 )
					{
						if ( ImGui::CollapsingHeader( LOCALE( "View.Molecule.Dynamic" ),
													  ImGuiTreeNodeFlags_DefaultOpen ) )
						{
							ImGui::Text( "Frames: %d", _getModel().getFrameCount() );
							int frame = int( _getModel().getFrame() );
							if ( ImGui::SliderInt(
									 LOCALE( "View.Frame" ), &frame, 0, _getModel().getFrameCount() - 1 ) )
							{ VTX_ACTION( new Action::MoleculeChangeFrame( _getModel(), frame ) ); }
							if ( ImGui::InputInt( "##FrameInput", &frame, 1 ) )
							{ VTX_ACTION( new Action::MoleculeChangeFrame( _getModel(), frame ) ); }
							bool isPlaying = _getModel().isPlaying();
							if ( ImGui::Checkbox( LOCALE( "View.Play" ), &isPlaying ) )
							{ VTX_ACTION( new Action::MoleculeChangeIsPlaying( _getModel(), isPlaying ) ); }
							int fps = _getModel().getFPS();
							if ( ImGui::SliderInt( LOCALE( "View.FPS" ), &fps, 0, VIDEO_FPS ) )
							{ VTX_ACTION( new Action::MoleculeChangeFPS( _getModel(), fps ) ); }
							if ( ImGui::InputInt( "##FPSInput", &fps, 1 ) )
							{ VTX_ACTION( new Action::MoleculeChangeFPS( _getModel(), fps ) ); }
						}
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Transform" ) ) )
					{
						ImGui::PushID( "Position" );
						Vec3f translation = _getModel().getTransform().getTranslationVector();
						float t[]		  = { translation.x, translation.y, translation.z };
						if ( ImGui::InputFloat3( LOCALE( "View.Transform.Position" ), t, 2 ) )
						{
							VTX_ACTION( new Action::TransformableSetTranslation( _getModel(),
																				 Vec3f( t[ 0 ], t[ 1 ], t[ 2 ] ) ) );
						}
						ImGui::PopID();
						/*
						ImGui::PushID( "Rotation" );
						Vec3f rotation = _getModel().getTransform().getRotationVector();
						float r[]	   = { rotation.x, rotation.y, rotation.z };
						if ( ImGui::InputFloat3( LOCALE( "View.Transform.Rotation" ), t, 2 ) )
						{
							// VTX_ACTION(
							//	new Action::TransformableRotate( _getModel(), Vec3f( t[ 0 ], t[ 1 ], t[ 2 ] ) ) );
						}
						ImGui::PopID();
						*/
						ImGui::PushID( "Scale" );
						Vec3f scale = _getModel().getTransform().getScaleVector();
						float s		= scale.x;
						if ( ImGui::InputFloat( LOCALE( "View.Transform.Scale" ), &s, 1.f ) )
						{ VTX_ACTION( new Action::TransformableSetScale( _getModel(), s ) ); }
						ImGui::PopID();
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Options" ) ) )
					{
						Vec3f color = _getModel().getColor();
						if ( ImGui::ColorEdit3( LOCALE( "View.Color" ), (float *)&color ) )
						{
							VTX_ACTION( new Action::ColorableChangeColor( _getModel(), color ) );
							VTX_ACTION( new Action::ChangeColorMode( View::MOLECULE_COLOR_MODE::PROTEIN ) );
						}
						if ( _getModel().getPRM().solventIds.size() > 0
							 || _getModel().getPSF().solventResidueSymbols.size() > 0 )
						{
							bool showSolvent = _getModel().showSolvent();
							if ( ImGui::Checkbox( LOCALE( "View.Molecule.Solvent" ), &showSolvent ) )
							{ VTX_ACTION( new Action::MoleculeChangeShowSolvent( _getModel(), showSolvent ) ); }
						}
						if ( _getModel().getPRM().ionIds.size() > 0
							 || _getModel().getPSF().ionResidueSymbols.size() > 0 )
						{
							bool showIon = _getModel().showIon();
							if ( ImGui::Checkbox( LOCALE( "View.Molecule.Ion" ), &showIon ) )
							{ VTX_ACTION( new Action::MoleculeChangeShowIon( _getModel(), showIon ) ); }
						}
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
				if ( notClosed == false ) { VTX_ACTION( new Action::SelectableUnselect( _getModel() ) ); }
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
