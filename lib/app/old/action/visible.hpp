#ifndef __VTX_APP_OLD_ACTION_VISIBLE__
#define __VTX_APP_OLD_ACTION_VISIBLE__

#include "app/old/action.hpp"
#include "app/old/component/generic/base_visible.hpp"
#include "app/old/core/action/base_action.hpp"
#include "enum_visibility_mode.hpp"
#include <vector>

namespace VTX::App::Old::Action::Visible
{
	class ChangeVisibility : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeVisibility( const VISIBILITY_MODE p_mode ) : _mode( p_mode ) {};
		explicit ChangeVisibility( Component::Generic::BaseVisible & p_visible, const VISIBILITY_MODE p_mode ) :
			_mode( p_mode )
		{
			_visibles.emplace_back( &p_visible );
		}
		explicit ChangeVisibility( std::vector<Component::Generic::BaseVisible *> & p_visibles,
								   const VISIBILITY_MODE							p_mode ) :
			_mode( p_mode )
		{
			_visibles.resize( p_visibles.size() );
			for ( int i = 0; i < p_visibles.size(); i++ )
				_visibles[ i ] = p_visibles[ i ];
		}

		virtual void execute() override;

	  protected:
		std::vector<Component::Generic::BaseVisible *> _visibles = std::vector<Component::Generic::BaseVisible *>();
		const VISIBILITY_MODE						   _mode;

		bool _getVisibilityBool( const Component::Generic::BaseVisible & visible ) const;
		bool _getVisibilityBool() const;
	};
} // namespace VTX::App::Old::Action::Visible
#endif
