#ifndef __VTX_ACTION_SELECTION__
#define __VTX_ACTION_SELECTION__

#include "app/action/visible.hpp"
#include "app/core/action/base_action.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/model/atom.hpp"
#include "app/old_app/model/category.hpp"
#include "app/old_app/model/chain.hpp"
#include "app/old_app/model/generated_molecule.hpp"
#include "app/old_app/model/molecule.hpp"
#include "app/old_app/model/residue.hpp"
#include "app/old_app/model/selection.hpp"
#include "app/old_app/model/viewpoint.hpp"
#include <vector>

namespace VTX::Action::Selection
{
	class SelectAll : public Core::Action::BaseAction
	{
	  public:
		explicit SelectAll() {}

		virtual void execute() override;
	};
	class ClearSelection : public Core::Action::BaseAction
	{
	  public:
		explicit ClearSelection( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class SelectModels : public Core::Action::BaseAction
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
	class UnselectModels : public Core::Action::BaseAction
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

	class SelectMolecule : public Core::Action::BaseAction
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
	class SelectCategory : public Core::Action::BaseAction
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
	class SelectChain : public Core::Action::BaseAction
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
	class SelectResidue : public Core::Action::BaseAction
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
	class SelectAtom : public Core::Action::BaseAction
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

	class UnselectMolecule : public Core::Action::BaseAction
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
	class UnselectCategory : public Core::Action::BaseAction
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
	class UnselectChain : public Core::Action::BaseAction
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
	class UnselectResidue : public Core::Action::BaseAction
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
	class UnselectAtom : public Core::Action::BaseAction
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

	class SelectViewpoint : public Core::Action::BaseAction
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
		}

		virtual void execute() override;

	  private:
		Model::Selection &				_selection;
		std::vector<Model::Viewpoint *> _viewpoints = std::vector<Model::Viewpoint *>();
		const bool						_appendToSelection;
	};
	class UnselectViewpoint : public Core::Action::BaseAction
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
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
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

	class ToggleWatersVisibility : public Core::Action::BaseAction
	{
	  public:
		explicit ToggleWatersVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ToggleSolventVisibility : public Core::Action::BaseAction
	{
	  public:
		explicit ToggleSolventVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ToggleHydrogensVisibility : public Core::Action::BaseAction
	{
	  public:
		explicit ToggleHydrogensVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ToggleIonsVisibility : public Core::Action::BaseAction
	{
	  public:
		explicit ToggleIonsVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ToggleTrajectoryPlaying : public Core::Action::BaseAction
	{
	  public:
		explicit ToggleTrajectoryPlaying( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};

	class ChangeRepresentationPreset : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Selection & p_selection, const int p_indexPreset ) :
			_selection( p_selection ), _indexPreset( p_indexPreset )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
		const int		   _indexPreset;
	};

	class Copy : public Core::Action::BaseAction
	{
	  public:
		explicit Copy( const Model::Selection & p_source,
					   const int				p_frame = Model::GeneratedMolecule::ALL_FRAMES_INDEX ) :
			_selection( p_source ),
			_frame( p_frame )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  protected:
		void _copyFrame( const Model::Molecule & p_source, const Model::Selection & p_selection, const int p_frame );

	  private:
		const Model::Selection & _selection;
		const int				 _frame;
	};

	class Extract : public Core::Action::BaseAction
	{
	  public:
		explicit Extract( Model::Selection & p_source ) : _selection( p_source )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};
	class Delete : public Core::Action::BaseAction
	{
	  public:
		explicit Delete( Model::Selection & p_selection ) : _selection( p_selection )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		Model::Selection & _selection;
	};
	////////////////////////////////////////////////////////////////////////////////////////
} // namespace VTX::Action::Selection
#endif
