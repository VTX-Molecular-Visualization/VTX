#ifndef __VTX_APP_ACTION_ATOM__
#define __VTX_APP_ACTION_ATOM__

#include "app/action.hpp"
#include "app/action/visible.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/core/action/base_action.hpp"
#include "app/old_app/color/rgba.hpp"

namespace VTX::App::Action::Atom
{
	class ChangeColor : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Component::Chemistry::Atom & p_atom, const Color::Rgba & p_color ) :
			_atom( p_atom ), _color( p_color )
		{
		}

		virtual void execute() override;

	  private:
		App::Component::Chemistry::Atom & _atom;
		const Color::Rgba				  _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( App::Component::Chemistry::Atom & p_atom, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_atom, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	class Delete : public App::Core::Action::BaseAction
	{
	  public:
		explicit Delete( App::Component::Chemistry::Atom & p_atom ) : _atom( p_atom )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		App::Component::Chemistry::Atom & _atom;
	};

	class Copy : public App::Core::Action::BaseAction
	{
	  public:
		explicit Copy( const App::Component::Chemistry::Atom & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const App::Component::Chemistry::Atom & _target;
	};

	class Extract : public App::Core::Action::BaseAction
	{
	  public:
		explicit Extract( const App::Component::Chemistry::Atom & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const App::Component::Chemistry::Atom & _target;
	};
} // namespace VTX::App::Action::Atom
#endif
