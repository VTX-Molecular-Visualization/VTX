#include "molecule.hpp"
#include "action/molecule.hpp"
#include "action/selectable.hpp"
#include "action/transformable.hpp"
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
				Model::Configuration::Molecule & config = _model->getConfiguration();

				ImGui::PushID( ( "ViewMolecule" + std::to_string( _model->getId() ) ).c_str() );
				bool notClosed = true;
				if ( ImGui::CollapsingHeader( _model->getName().c_str(), &notClosed, ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					bool isVisible = _model->isVisible();
					if ( ImGui::Checkbox( LOCALE( "View.Visible" ), &isVisible ) )
					{
						VTX_ACTION( new Action::Molecule::ChangeVisibility(
							*_model, (Action::Visible::ChangeVisibility::VISIBILITY_MODE)isVisible ) );
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Data" ), ImGuiTreeNodeFlags_DefaultOpen ) )
					{
						ImGui::Text( LOCALE( "View.Chains%Count" ), _model->getChainCount() );
						ImGui::Text( LOCALE( "View.Residues%Count" ), _model->getResidueCount() );
						ImGui::Text( LOCALE( "View.Atoms%Count" ), _model->getAtomCount() );
						ImGui::Text( LOCALE( "View.Bonds%Count" ), _model->getBondCount() );
						ImGui::Text( LOCALE( "View.SecondaryStructure%State" ),
									 config.isSecondaryStructureLoadedFromFile ? LOCALE( "View.Loaded" )
																			   : LOCALE( "View.Computed" ) );
						if ( config.isSecondaryStructureLoadedFromFile )
						{
							if ( ImGui::Button( LOCALE( "View.Compute" ) ) )
							{
								VTX_ACTION( new Action::Molecule::ComputeSecondaryStructure( *_model ) );
							}
						}

						if ( ImGui::Button( LOCALE( "Sequence" ) ) )
						{
							VTX_ACTION( new Action::Molecule::ToggleSequenceVisibility( *_model ) );
						}
					}
					if ( _model->getFrameCount() > 1 )
					{
						if ( ImGui::CollapsingHeader( LOCALE( "View.Molecule.Dynamic" ),
													  ImGuiTreeNodeFlags_DefaultOpen ) )
						{
							ImGui::Text( "Frames: %d", _model->getFrameCount() );
							int frame = int( _model->getFrame() );
							if ( ImGui::SliderInt( LOCALE( "View.Frame" ), &frame, 0, _model->getFrameCount() - 1 ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeFrame( *_model, frame ) );
							}
							if ( ImGui::InputInt( "##FrameInput", &frame, 1 ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeFrame( *_model, frame ) );
							}
							bool isPlaying = _model->isPlaying();
							if ( ImGui::Checkbox( LOCALE( "View.Play" ), &isPlaying ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeIsPlaying( *_model, isPlaying ) );
							}
							int fps = _model->getFPS();
							if ( ImGui::SliderInt( LOCALE( "View.FPS" ), &fps, 0, Setting::VIDEO_FPS_DEFAULT ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeFPS( *_model, fps ) );
							}
							if ( ImGui::InputInt( "##FPSInput", &fps, 1 ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeFPS( *_model, fps ) );
							}
						}
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Transform" ) ) )
					{
						ImGui::PushID( "Position" );
						Vec3f translation = _model->getTransform().getTranslationVector();
						float t[]		  = { translation.x, translation.y, translation.z };
						if ( ImGui::InputFloat3( LOCALE( "View.Transform.Position" ), t, 2 ) )
						{
							VTX_ACTION(
								new Action::Transformable::SetTranslation( *_model, Vec3f( t[ 0 ], t[ 1 ], t[ 2 ] ) ) );
						}
						ImGui::PopID();
						/*
						ImGui::PushID( "Rotation" );
						Vec3f rotation = _model->getTransform().getRotationVector();
						float r[]	   = { rotation.x, rotation.y, rotation.z };
						if ( ImGui::InputFloat3( LOCALE( "View.Transform.Rotation" ), t, 2 ) )
						{
							// VTX_ACTION(
							//	new Action::Molecule::TransformableRotate( *_model, Vec3f( t[ 0 ], t[ 1 ], t[ 2 ] )
						) );
						}
						ImGui::PopID();
						*/
						ImGui::PushID( "Scale" );
						Vec3f scale = _model->getTransform().getScaleVector();
						float s		= scale.x;
						if ( ImGui::InputFloat( LOCALE( "View.Transform.Scale" ), &s, 1.f ) )
						{
							VTX_ACTION( new Action::Transformable::SetScale( *_model, s ) );
						}
						ImGui::PopID();
					}
					if ( ImGui::CollapsingHeader( LOCALE( "View.Options" ) ) )
					{
						// Molecule color mode.
						const char * modes[]   = { LOCALE( "Enum.ColorMode.Atom" ),
												   LOCALE( "Enum.ColorMode.Residue" ),
												   LOCALE( "Enum.ColorMode.Chain" ),
												   LOCALE( "Enum.ColorMode.Protein" ),
												   LOCALE( "Enum.ColorMode.Inherited" ) };
						int			 colorMode = (int)_model->getColorMode();
						if ( ImGui::Combo( LOCALE( "View.Molecule.ColorMode" ), &colorMode, modes, 5 ) )
						{
							VTX_ACTION(
								new Action::Molecule::ChangeColorMode( *_model, (Generic::COLOR_MODE)colorMode ) );
						}

						// Molecule color.
						Color::Rgb color = _model->getColor();
						if ( ImGui::ColorEdit3( LOCALE( "View.Color" ), (float *)&color ) )
						{
							VTX_ACTION( new Action::Molecule::ChangeColor( *_model, color ) );
							// VTX_ACTION( new Action::Molecule::ChangeColorMode( View::MOLECULE_COLOR_MODE::PROTEIN )
							// );
						}

						// Secondary structure color mode.
						const char * modesSS[]	 = { LOCALE( "Enum.ColorMode.SS.JMol" ),
													 LOCALE( "Enum.ColorMode.SS.Protein" ),
													 LOCALE( "Enum.ColorMode.SS.Chain" ),
													 LOCALE( "Enum.ColorMode.SS.Residue" ) };
						int			 colorModeSS = (int)_model->getSecondaryStructure().getColorMode();
						if ( ImGui::Combo( LOCALE( "View.Molecule.SSColorMode" ), &colorModeSS, modesSS, 4 ) )
						{
							VTX_ACTION( new Action::Molecule::ChangeColorModeSecondaryStructure(
								*_model, (Model::SecondaryStructure::COLOR_MODE)colorModeSS ) );
						}

						// Solvents.
						if ( config.solventAtomIds.size() > 0 || config.solventResidueSymbols.size() > 0 )
						{
							bool showSolvent = _model->showSolvent();
							if ( ImGui::Checkbox( LOCALE( "View.Molecule.Solvent" ), &showSolvent ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeShowSolvent( *_model, showSolvent ) );
							}
						}
						if ( config.ionAtomIds.size() > 0 || config.ionResidueSymbols.size() > 0 )
						{
							bool showIon = _model->showIon();
							if ( ImGui::Checkbox( LOCALE( "View.Molecule.Ion" ), &showIon ) )
							{
								VTX_ACTION( new Action::Molecule::ChangeShowIon( *_model, showIon ) );
							}
						}
					}
#ifdef _DEBUG
					if ( ImGui::CollapsingHeader( "Transform-debug" ) )
					{
						float *x, *y, *z;

						ImGui::PushID( "Debug" );

						ImGui::Text( "Translation" );
						const Mat4f & translation = _model->getTransform().getTranslation();
						x						  = (float *)&translation[ 0 ];
						y						  = (float *)&translation[ 1 ];
						z						  = (float *)&translation[ 2 ];
						ImGui::PushID( "Translation" );
						ImGui::InputFloat3( "X", x, 2 );
						ImGui::InputFloat3( "Y", y, 2 );
						ImGui::InputFloat3( "Z", z, 2 );
						ImGui::PopID();

						ImGui::Text( "Rotation" );
						const Mat4f & rotation = _model->getTransform().getRotation();
						x					   = (float *)&rotation[ 0 ];
						y					   = (float *)&rotation[ 1 ];
						z					   = (float *)&rotation[ 2 ];
						ImGui::PushID( "Rotation" );
						ImGui::InputFloat3( "X", x, 2 );
						ImGui::InputFloat3( "Y", y, 2 );
						ImGui::InputFloat3( "Z", z, 2 );
						ImGui::PopID();

						ImGui::Text( "Scale" );
						const Mat4f & scale = _model->getTransform().getScale();
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

					Util::UI::drawRepresentations( *_model, *_model );
				}
				if ( notClosed == false )
				{
					// VTX_ACTION( new Action::Selectable::Unselect( *_model ) );
				}
				ImGui::PopID();
			}
		} // namespace UI
	}	  // namespace View
} // namespace VTX
