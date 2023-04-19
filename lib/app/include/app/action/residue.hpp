#ifndef __VTX_ACTION_RESIDUE__
#define __VTX_ACTION_RESIDUE__

#include "app/action/visible.hpp"
#include "app/old_app/color/rgba.hpp"
#include "app/old_app/model/representation/instantiated_representation.hpp"
#include "app/old_app/model/residue.hpp"
#include <unordered_set>

namespace VTX::Action::Residue
{
	class ChangeColor : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( Model::Residue & p_residue, const Color::Rgba & p_color ) :
			_color( p_color ), _residues { &p_residue }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeColor( const std::unordered_set<Model::Residue *> & p_residues, const Color::Rgba & p_color ) :
			_color( p_color ), _residues( p_residues )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Residue *> _residues;
		const Color::Rgba						   _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Residue & p_residue, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_residue, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	class ChangeRepresentationPreset : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Residue & p_residue, const int p_indexPreset ) :
			_indexPreset( p_indexPreset ), _residues { &p_residue }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<Model::Residue *> & p_residues,
											 const int									  p_indexPreset ) :
			_indexPreset( p_indexPreset ),
			_residues( p_residues )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Residue *> _residues;
		const int								   _indexPreset;
	};

	class RemoveRepresentation : public Core::Action::BaseAction
	{
	  public:
		explicit RemoveRepresentation( Model::Residue & p_residue ) : _residues { &p_residue }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveRepresentation( const std::unordered_set<Model::Residue *> & p_residues ) :
			_residues( p_residues )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Residue *> _residues;
	};

	class Delete : public Core::Action::BaseAction
	{
	  public:
		explicit Delete( Model::Residue & p_residue ) : _residue( p_residue )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		Model::Residue & _residue;
	};

	class Copy : public Core::Action::BaseAction
	{
	  public:
		explicit Copy( const Model::Residue & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const Model::Residue & _target;
	};

	class Extract : public Core::Action::BaseAction
	{
	  public:
		explicit Extract( const Model::Residue & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const Model::Residue & _target;
	};

	class ApplyRepresentation : public Core::Action::BaseAction
	{
	  public:
		explicit ApplyRepresentation( const std::unordered_set<Model::Residue *> &				p_residues,
									  const Model::Representation::InstantiatedRepresentation & p_source,
									  const Model::Representation::MEMBER_FLAG &				p_flag ) :
			_representation( p_source ),
			_flag( p_flag ), _residues( p_residues )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Residue *>				  _residues;
		const Model::Representation::InstantiatedRepresentation & _representation;
		const Model::Representation::MEMBER_FLAG				  _flag;
	};
} // namespace VTX::Action::Residue
#endif
