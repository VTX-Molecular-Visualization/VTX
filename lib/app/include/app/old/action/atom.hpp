#ifndef __VTX_APP_OLD_ACTION_ATOM__
#define __VTX_APP_OLD_ACTION_ATOM__

#include "app/old/action.hpp"
#include "app/old/action/visible.hpp"
#include "app/old/component/chemistry/atom.hpp"
#include "app/old/core/action/base_action.hpp"
#include <util/color/rgba.hpp>

namespace VTX::App::Old::Action::Atom
{
	class ChangeColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Old::Component::Chemistry::Atom & p_atom, const Util::Color::Rgba & p_color ) :
			_atom( p_atom ), _color( p_color )
		{
		}

		virtual void execute() override;

	  private:
		App::Old::Component::Chemistry::Atom & _atom;
		const Util::Color::Rgba				  _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( App::Old::Component::Chemistry::Atom & p_atom, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_atom, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	class Delete : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Delete( App::Old::Component::Chemistry::Atom & p_atom ) : _atom( p_atom )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		App::Old::Component::Chemistry::Atom & _atom;
	};

	class Copy : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Copy( const App::Old::Component::Chemistry::Atom & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Atom & _target;
	};

	class Extract : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Extract( const App::Old::Component::Chemistry::Atom & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Atom & _target;
	};
} // namespace VTX::App::Old::Action::Atom
#endif
