#include "molecule.hpp"
#include "action/molecule.hpp"
#include "action/selectable.hpp"
#include "action/transformable.hpp"
#include "action/visible.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			void Molecule::_draw()
			{
				Model::Configuration::Molecule & config = _getModel().getConfiguration();

				ImGui::PushID( ( "ViewMolecule" + std::to_string( _getModel().getId() ) ).c_str() );
				bool notClosed = true;
				if ( ImGui::CollapsingHeader(
						 _getModel().getName().c_str(), &notClosed, ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					bool isVisible = _getModel().isVisible();
					if ( ImGui::Checkbox( LOCALE( "View.Visible" ), &isVisible ) )
					{
						VTX_ACTION( new Action::Visible::ChangeVisibility(
							_getModel(), (Action::Visible::ChangeVisibility::VISIBILITY_MODE)isVisible ) );
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Data" ), ImGuiTreeNodeFlags_DefaultOpen ) )
					{
						ImGui::Text( LOCALE( "View.Chains%Count" ), _getModel().getChainCount() );
						ImGui::Text( LOCALE( "View.Residues%Count" ), _getModel().getResidueCount() );
						ImGui::Text( LOCALE( "View.Atoms%Count" ), _getModel().getAtomCount() );
						ImGui::Text( LOCALE( "View.Bonds%Count" ), _getModel().getBondCount() );
						ImGui::Text( LOCALE( "View.SecondaryStructure%State" ),
									 config.isSecondaryStructureLoadedFromFile ? LOCALE( "View.Loaded" )
																			   : LOCALE( "View.Computed" ) );
						if ( config.isSecondaryStructureLoadedFromFile )
						{
							if ( ImGui::Button( LOCALE( "View.Compute" ) ) )
							{
								VTX_ACTION( new Action::Molecule::ComputeSecondaryStructure( _getModel() ) );
							}
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
							{
								VTX_ACTION( new Action::Molecule::ChangeFrame( _getModel(), frame ) );
							}
							if ( ImGui::InputInt( "##FrameInput", &frame, 1 ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeFrame( _getModel(), frame ) );
							}
							bool isPlaying = _getModel().isPlaying();
							if ( ImGui::Checkbox( LOCALE( "View.Play" ), &isPlaying ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeIsPlaying( _getModel(), isPlaying ) );
							}
							int fps = _getModel().getFPS();
							if ( ImGui::SliderInt( LOCALE( "View.FPS" ), &fps, 0, Setting::VIDEO_FPS_DEFAULT ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeFPS( _getModel(), fps ) );
							}
							if ( ImGui::InputInt( "##FPSInput", &fps, 1 ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeFPS( _getModel(), fps ) );
							}
						}
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Transform" ) ) )
					{
						ImGui::PushID( "Position" );
						Vec3f translation = _getModel().getTransform().getTranslationVector();
						float t[]		  = { translation.x, translation.y, translation.z };
						if ( ImGui::InputFloat3( LOCALE( "View.Transform.Position" ), t, 2 ) )
						{
							VTX_ACTION( new Action::Transformable::SetTranslation( _getModel(),
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
							//	new Action::Molecule::TransformableRotate( _getModel(), Vec3f( t[ 0 ], t[ 1 ], t[ 2 ] )
						) );
						}
						ImGui::PopID();
						*/
						ImGui::PushID( "Scale" );
						Vec3f scale = _getModel().getTransform().getScaleVector();
						float s		= scale.x;
						if ( ImGui::InputFloat( LOCALE( "View.Transform.Scale" ), &s, 1.f ) )
						{
							VTX_ACTION( new Action::Transformable::SetScale( _getModel(), s ) );
						}
						ImGui::PopID();
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Options" ) ) )
					{
						std::vector<float> color = _getModel().getColor().toStdVector();
						if ( ImGui::ColorEdit3( LOCALE( "View.Color" ), (float *)&color ) )
						{
							VTX_ACTION( new Action::Molecule::ChangeColor( _getModel(), color ) );
							// VTX_ACTION( new Action::Molecule::ChangeColorMode( View::MOLECULE_COLOR_MODE::PROTEIN )
							// );
						}
						if ( config.solventAtomIds.size() > 0 || config.solventResidueSymbols.size() > 0 )
						{
							bool showSolvent = _getModel().showSolvent();
							if ( ImGui::Checkbox( LOCALE( "View.Molecule.Solvent" ), &showSolvent ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeShowSolvent( _getModel(), showSolvent ) );
							}
						}
						if ( config.ionAtomIds.size() > 0 || config.ionResidueSymbols.size() > 0 )
						{
							bool showIon = _getModel().showIon();
							if ( ImGui::Checkbox( LOCALE( "View.Molecule.Ion" ), &showIon ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeShowIon( _getModel(), showIon ) );
							}
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

					Util::UI::drawRepresentations( _getModel() );
				}
				if ( notClosed == false )
				{
					VTX_ACTION( new Action::Selectable::Unselect( _getModel() ) );
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
