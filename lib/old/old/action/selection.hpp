#ifndef __VTX_ACTION_SELECTION__
#define __VTX_ACTION_SELECTION__

#include "base_action.hpp"
#include "id.hpp"
#include "model/atom.hpp"
#include "model/category.hpp"
#include "model/chain.hpp"
#include "model/generated_molecule.hpp"
#include "model/label.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "model/viewpoint.hpp"
#include "visible.hpp"
#include <vector>

namespace VTX::Action::Selection
{
	class SelectAll : public BaseAction
	{
	  public:
		explicit SelectAll() {}

		virtual void execute() override;
	};
	class SelectModels : public BaseAction
	{
	  public:
		explicit SelectModels( Model::Selection &			  p_selection,
							   const std::vector<Model::ID> & p_models,
							   const bool					  p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_models.resize( p_models.size() );
			for ( int i = 0; i < _models.size(); i++ )
				_models[ i ] = p_models[ i ];
		}

		virtual void execute() override;

	  private:
		Model::Selection &	   _selection;
		std::vector<Model::ID> _models = std::vector<Model::ID>();
		const bool			   _appendToSelection;
	};
	class UnselectModels : public BaseAction
	{
	  public:
		explicit UnselectModels( Model::Selection & p_selection, const std::vector<Model::ID> & p_models ) :
			_selection( p_selection )
		{
			_models.resize( p_models.size() );
			for ( int i = 0; i < _models.size(); i++ )
				_models[ i ] = p_models[ i ];
		}

		virtual void execute() override;

	  private:
		Model::Selection &	   _selection;
		std::vector<Model::ID> _models = std::vector<Model::ID>();
	};

	class SelectViewpoint : public BaseAction
	{
	  public:
		explicit SelectViewpoint( Model::Selection & p_selection,
								  Model::Viewpoint & p_viewpoint,
								  const bool		 p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_viewpoints.emplace_back( &p_viewpoint );
		}
		explicit SelectViewpoint( Model::Selection &				p_selection,
								  std::vector<Model::Viewpoint *> & p_viewpoints,
								  const bool						p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_viewpoints.resize( p_viewpoints.size() );
			for ( int i = 0; i < _viewpoints.size(); i++ )
				_viewpoints[ i ] = p_viewpoints[ i ];

			// VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

		virtual void execute() override;

	  private:
		Model::Selection &				_selection;
		std::vector<Model::Viewpoint *> _viewpoints = std::vector<Model::Viewpoint *>();
		const bool						_appendToSelection;
	};

	class UnselectViewpoint : public BaseAction
	{
	  public:
		explicit UnselectViewpoint( Model::Selection & p_selection,
									Model::Viewpoint & p_viewpoint,
									bool			   p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_viewpoints.emplace_back( &p_viewpoint );
		}
		explicit UnselectViewpoint( Model::Selection &				  p_selection,
									std::vector<Model::Viewpoint *> & p_viewpoints,
									bool							  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_viewpoints.resize( p_viewpoints.size() );
			for ( int i = 0; i < p_viewpoints.size(); i++ )
				_viewpoints[ i ] = p_viewpoints[ i ];
		}

		virtual void execute() override;

	  private:
		Model::Selection &				_selection;
		std::vector<Model::Viewpoint *> _viewpoints = std::vector<Model::Viewpoint *>();
		const bool						_check;
	};

	class SelectMolecule : public BaseAction
	{
	  public:
		explicit SelectMolecule( Model::Selection & p_selection,
								 Model::Molecule &	p_molecule,
								 const bool			p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_molecules.emplace_back( &p_molecule );
		}
		explicit SelectMolecule( Model::Selection &						p_selection,
								 const std::vector<Model::Molecule *> & p_molecules,
								 const bool								p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_molecules.resize( p_molecules.size() );
			for ( int i = 0; i < _molecules.size(); i++ )
				_molecules[ i ] = p_molecules[ i ];
		}

		virtual void execute() override;

	  private:
		Model::Selection &			   _selection;
		std::vector<Model::Molecule *> _molecules = std::vector<Model::Molecule *>();
		const bool					   _appendToSelection;
	};

	class SelectCategory : public BaseAction
	{
	  public:
		explicit SelectCategory( Model::Selection & p_selection,
								 Model::Category &	p_category,
								 const bool			p_appendToSelection = false ) :
			_selection( p_selection ),
			_categories { &p_category }, _appendToSelection( p_appendToSelection )
		{
		}

		explicit SelectCategory( Model::Selection &						p_selection,
								 const std::vector<Model::Category *> & p_categories,
								 const bool								p_appendToSelection = false ) :
			_selection( p_selection ),
			_categories( p_categories ), _appendToSelection( p_appendToSelection )
		{
		}

		virtual void execute() override;

	  private:
		Model::Selection &			   _selection;
		std::vector<Model::Category *> _categories;
		const bool					   _appendToSelection;
	};

	class SelectChain : public BaseAction
	{
	  public:
		explicit SelectChain( Model::Selection & p_selection,
							  Model::Chain &	 p_chain,
							  const bool		 p_appendToSelection = false ) :
			_selection( p_selection ),
			_chains { &p_chain }, _appendToSelection( p_appendToSelection )
		{
		}

		explicit SelectChain( Model::Selection &				  p_selection,
							  const std::vector<Model::Chain *> & p_chains,
							  const bool						  p_appendToSelection = false ) :
			_selection( p_selection ),
			_chains( p_chains ), _appendToSelection( p_appendToSelection )
		{
		}

		virtual void execute() override;

	  private:
		Model::Selection &			_selection;
		std::vector<Model::Chain *> _chains;
		const bool					_appendToSelection;
	};

	class SelectResidue : public BaseAction
	{
	  public:
		explicit SelectResidue( Model::Selection & p_selection,
								Model::Residue &   p_residue,
								const bool		   p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_residues.emplace_back( &p_residue );
		}
		explicit SelectResidue( Model::Selection &					  p_selection,
								const std::vector<Model::Residue *> & p_residues,
								const bool							  p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_residues.resize( p_residues.size() );
			for ( int i = 0; i < _residues.size(); i++ )
				_residues[ i ] = p_residues[ i ];
		}

		virtual void execute() override;

	  private:
		Model::Selection &			  _selection;
		std::vector<Model::Residue *> _residues = std::vector<Model::Residue *>();
		const bool					  _appendToSelection;
	};

	class SelectAtom : public BaseAction
	{
	  public:
		explicit SelectAtom( Model::Selection & p_selection,
							 Model::Atom &		p_atom,
							 const bool			p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_atoms.emplace_back( &p_atom );
		}

		explicit SelectAtom( Model::Selection &					p_selection,
							 const std::vector<Model::Atom *> & p_atoms,
							 const bool							p_appendToSelection = false ) :
			_selection( p_selection ),
			_atoms( p_atoms ), _appendToSelection( p_appendToSelection )
		{
		}

		virtual void execute() override;

	  private:
		Model::Selection &		   _selection;
		std::vector<Model::Atom *> _atoms = std::vector<Model::Atom *>();
		const bool				   _appendToSelection;
	};

	class UnselectMolecule : public BaseAction
	{
	  public:
		explicit UnselectMolecule( Model::Selection & p_selection,
								   Model::Molecule &  p_molecule,
								   bool				  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_molecules.emplace_back( &p_molecule );
		}
		explicit UnselectMolecule( Model::Selection &					  p_selection,
								   const std::vector<Model::Molecule *> & p_molecules,
								   bool									  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_molecules.resize( p_molecules.size() );
			for ( int i = 0; i < _molecules.size(); i++ )
				_molecules[ i ] = p_molecules[ i ];
		}

		virtual void execute() override;

	  private:
		Model::Selection &			   _selection;
		std::vector<Model::Molecule *> _molecules = std::vector<Model::Molecule *>();
		const bool					   _check;
	};

	class UnselectCategory : public BaseAction
	{
	  public:
		explicit UnselectCategory( Model::Selection & p_selection,
								   Model::Category &  p_category,
								   bool				  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_categories.emplace_back( &p_category );
		}
		explicit UnselectCategory( Model::Selection &					  p_selection,
								   const std::vector<Model::Category *> & p_categories,
								   bool									  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_categories.resize( p_categories.size() );
			for ( int i = 0; i < p_categories.size(); i++ )
				_categories[ i ] = p_categories[ i ];
		}

		virtual void execute() override;

	  private:
		Model::Selection &			   _selection;
		std::vector<Model::Category *> _categories = std::vector<Model::Category *>();
		const bool					   _check;
	};

	class UnselectChain : public BaseAction
	{
	  public:
		explicit UnselectChain( Model::Selection & p_selection, Model::Chain & p_chain, bool p_check = false ) :
			_selection( p_selection ), _check( p_check )
		{
			_chains.emplace_back( &p_chain );
		}
		explicit UnselectChain( Model::Selection &					p_selection,
								const std::vector<Model::Chain *> & p_chains,
								bool								p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_chains.resize( p_chains.size() );
			for ( int i = 0; i < p_chains.size(); i++ )
				_chains[ i ] = p_chains[ i ];
		}

		virtual void execute() override;

	  private:
		Model::Selection &			_selection;
		std::vector<Model::Chain *> _chains = std::vector<Model::Chain *>();
		const bool					_check;
	};

	class UnselectResidue : public BaseAction
	{
	  public:
		explicit UnselectResidue( Model::Selection & p_selection, Model::Residue & p_residue, bool p_check = false ) :
			_selection( p_selection ), _check( p_check )
		{
			_residues.emplace_back( &p_residue );
		}
		explicit UnselectResidue( Model::Selection &					p_selection,
								  const std::vector<Model::Residue *> & p_residues,
								  bool									p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_residues.resize( p_residues.size() );
			for ( int i = 0; i < p_residues.size(); i++ )
				_residues[ i ] = p_residues[ i ];
		}

		virtual void execute() override;

	  private:
		Model::Selection &			  _selection;
		std::vector<Model::Residue *> _residues = std::vector<Model::Residue *>();
		const bool					  _check;
	};

	class UnselectAtom : public BaseAction
	{
	  public:
		explicit UnselectAtom( Model::Selection & p_selection, Model::Atom & p_atom, bool p_check = false ) :
			_selection( p_selection ), _check( p_check )
		{
			_atoms.emplace_back( &p_atom );
		}
		explicit UnselectAtom( Model::Selection &				  p_selection,
							   const std::vector<Model::Atom *> & p_atoms,
							   bool								  p_check = false ) :
			_selection( p_selection ),
			_atoms( p_atoms ), _check( p_check )
		{
		}
		virtual void execute() override;

	  private:
		Model::Selection &		   _selection;
		std::vector<Model::Atom *> _atoms = std::vector<Model::Atom *>();
		const bool				   _check;
	};

	class ClearSelection : public BaseAction
	{
	  public:
		explicit ClearSelection( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	///////////////////////////// ACTION ON SELECTION ///////////////////////////////
	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( const Model::Selection &		p_selection,
								   const Generic::BaseVisible & p_objReference,
								   const ID::VTX_ID &			p_objRefTypeId,
								   const VISIBILITY_MODE		p_mode ) :
			Visible::ChangeVisibility( p_mode ),
			_selection( p_selection ), _objRefTypeId( p_objRefTypeId )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		explicit ChangeVisibility( const Model::Selection & p_selection, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_mode ), _selection( p_selection ), _objRefTypeId( "" )
		{
		}

		virtual void execute() override;

		void show( const bool p_show );
		void solo();

	  private:
		const Model::Selection & _selection;
		const ID::VTX_ID		 _objRefTypeId;
	};

	class ToggleWatersVisibility : public BaseAction
	{
	  public:
		explicit ToggleWatersVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ToggleSolventVisibility : public BaseAction
	{
	  public:
		explicit ToggleSolventVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ToggleHydrogensVisibility : public BaseAction
	{
	  public:
		explicit ToggleHydrogensVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ToggleIonsVisibility : public BaseAction
	{
	  public:
		explicit ToggleIonsVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ToggleTrajectoryPlaying : public BaseAction
	{
	  public:
		explicit ToggleTrajectoryPlaying( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ChangeRepresentationPreset : public BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Selection & p_selection, const int p_indexPreset ) :
			_selection( p_selection ), _indexPreset( p_indexPreset )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
		const int		   _indexPreset;
	};

	class Orient : public BaseAction
	{
	  public:
		explicit Orient( const Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		const Model::Selection & _selection;
	};

	class Copy : public BaseAction
	{
	  public:
		explicit Copy( const Model::Selection & p_source,
					   const int				p_frame = Model::GeneratedMolecule::ALL_FRAMES_INDEX ) :
			_selection( p_source ),
			_frame( p_frame )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  protected:
		void _copyFrame( const Model::Molecule & p_source, const Model::Selection & p_selection, const int p_frame );

	  private:
		const Model::Selection & _selection;
		const int				 _frame;
	};

	class Extract : public BaseAction
	{
	  public:
		explicit Extract( Model::Selection & p_source ) : _selection( p_source )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};
	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Selection & p_selection ) : _selection( p_selection )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};
	////////////////////////////////////////////////////////////////////////////////////////
} // namespace VTX::Action::Selection
#endif