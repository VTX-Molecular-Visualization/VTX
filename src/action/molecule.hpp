#ifndef __VTX_ACTION_MOLECULE__
#define __VTX_ACTION_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "util/molecule.hpp"
#include "util/secondary_structure.hpp"
#include "visible.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Molecule
		{
			class ChangeColorMode : public BaseAction
			{
			  public:
				explicit ChangeColorMode( Model::Molecule & p_molecule, const Generic::COLOR_MODE p_colorMode ) :
					_molecule( p_molecule ), _colorMode( p_colorMode )
				{
				}

				virtual void execute() override { _molecule.setColorMode( _colorMode ); }

			  private:
				Model::Molecule &		  _molecule;
				const Generic::COLOR_MODE _colorMode;
			};

			class ChangeColor : public BaseAction
			{
			  public:
				explicit ChangeColor( Model::Molecule & p_molecule, const Color::Rgb & p_color ) :
					_molecule( p_molecule ), _color( p_color )
				{
				}

				virtual void execute() override
				{
					_molecule.setColor( _color );
					_molecule.refreshColors();
				}

			  private:
				Model::Molecule & _molecule;
				const Color::Rgb  _color;
			};

			class ChangeVisibility : public Visible::ChangeVisibility
			{
			  public:
				explicit ChangeVisibility( Model::Molecule & p_molecule, const VISIBILITY_MODE p_mode ) :
					Visible::ChangeVisibility( p_molecule, p_mode )
				{
				}

				virtual void execute() override
				{
					Model::Molecule & molecule = ( (Model::Molecule &)_visible );
					Visible::ChangeVisibility::execute();
					Util::Molecule::refreshRepresentationState( molecule );
				}
			};

			class ChangeFPS : public BaseAction
			{
			  public:
				explicit ChangeFPS( Model::Molecule & p_molecule, const int p_fps ) :
					_molecule( p_molecule ), _fps( p_fps )
				{
				}

				virtual void execute() override
				{
					_molecule.setFPS( Util::Math::clamp( _fps, 0, int( Setting::VIDEO_FPS_DEFAULT ) ) );
				}

			  private:
				Model::Molecule & _molecule;
				const int		  _fps;
			};

			class ChangeFrame : public BaseAction
			{
			  public:
				explicit ChangeFrame( Model::Molecule & p_molecule, const int p_frame ) :
					_molecule( p_molecule ), _frame( p_frame )
				{
				}

				virtual void execute() override
				{
					_molecule.setFrame( Util::Math::clamp( _frame, 0, (int)_molecule.getFrameCount() - 1 ) );
				}

			  private:
				Model::Molecule & _molecule;
				const int		  _frame;
			};

			class ChangeIsPlaying : public BaseAction
			{
			  public:
				explicit ChangeIsPlaying( Model::Molecule & p_molecule, const bool p_isPlaying ) :
					_molecule( p_molecule ), _isPlaying( p_isPlaying )
				{
				}

				virtual void execute() override { _molecule.setIsPlaying( _isPlaying ); }

			  private:
				Model::Molecule & _molecule;
				const bool		  _isPlaying;
			};

			class ChangeShowIon : public BaseAction
			{
			  public:
				explicit ChangeShowIon( Model::Molecule & p_molecule, const bool p_showIon ) :
					_molecule( p_molecule ), _showIon( p_showIon )
				{
				}

				virtual void execute() override { _molecule.setShowIon( _showIon ); }

			  private:
				Model::Molecule & _molecule;
				const bool		  _showIon;
			};

			class ChangeShowSolvent : public BaseAction
			{
			  public:
				explicit ChangeShowSolvent( Model::Molecule & p_molecule, const bool p_showSolvent ) :
					_molecule( p_molecule ), _showSolvent( p_showSolvent )
				{
				}

				virtual void execute() override { _molecule.setShowSolvent( _showSolvent ); }

			  private:
				Model::Molecule & _molecule;
				const bool		  _showSolvent;
			};

			class ComputeSecondaryStructure : public BaseAction
			{
			  public:
				explicit ComputeSecondaryStructure( Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

				virtual void execute() override
				{
					Util::SecondaryStructure::computeSecondaryStructure( _molecule );
					// TODO: update SS VBO.
				}

			  private:
				Model::Molecule & _molecule;
			};

			class ToggleSequenceVisibility : public BaseAction
			{
			  public:
				explicit ToggleSequenceVisibility( Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

				virtual void execute() override { _molecule.toggleSequenceVisibility(); }

			  private:
				Model::Molecule & _molecule;
			};

			class Delete : public BaseAction
			{
			  public:
				explicit Delete( Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

				virtual void execute() override
				{
					VTXApp::get().getScene().removeMolecule( &_molecule );
					delete &_molecule;
				}

			  private:
				Model::Molecule & _molecule;
			};

			class ChangeColorModeSecondaryStructure : public BaseAction
			{
			  public:
				explicit ChangeColorModeSecondaryStructure( Model::Molecule &							p_molecule,
															const Model::SecondaryStructure::COLOR_MODE p_colorMode ) :
					_molecule( p_molecule ),
					_colorMode( p_colorMode )
				{
				}

				virtual void execute() override { _molecule.getSecondaryStructure().setColorMode( _colorMode ); }

			  private:
				Model::Molecule &							_molecule;
				const Model::SecondaryStructure::COLOR_MODE _colorMode;
			};
		} // namespace Molecule
	}	  // namespace Action
} // namespace VTX
#endif
