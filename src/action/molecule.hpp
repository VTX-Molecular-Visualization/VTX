#ifndef __VTX_ACTION_MOLECULE__
#define __VTX_ACTION_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "util/molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Molecule
		{
			class ChangeColor : public BaseAction
			{
			  public:
				explicit ChangeColor( Model::Molecule & p_molecule, const std::vector<float> & p_color ) :
					_molecule( p_molecule ), _color( p_color )
				{
				}
				explicit ChangeColor( Model::Molecule & p_molecule, const Color::Rgb & p_color ) :
					_molecule( p_molecule ), _color( p_color )
				{
				}

				virtual void execute() override
				{
					_molecule.setColor( _color );
					_molecule.setColorMode();
				}

			  private:
				Model::Molecule & _molecule;
				const Color::Rgb  _color;
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
					Util::Molecule::computeSecondaryStructure( _molecule );
					_molecule.createSecondaryStructure();
				}

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
					Generic::destroy<Model::Molecule>( &_molecule );
				}

			  private:
				Model::Molecule & _molecule;
			};
		} // namespace Molecule
	}	  // namespace Action
} // namespace VTX
#endif
