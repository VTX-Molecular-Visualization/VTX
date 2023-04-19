#ifndef __VTX_ACTION_VISIBLE__
#define __VTX_ACTION_VISIBLE__

#include "app/core/action/base_action.hpp"
#include "app/old_app/generic/base_visible.hpp"
#include <vector>

namespace VTX::Action::Visible
{
	class ChangeVisibility : public Core::Action::BaseAction
	{
	  public:
		enum class VISIBILITY_MODE : int
		{
			HIDE = 0,
			SHOW,
			TOGGLE,
			SOLO,
			ALL
		};

		explicit ChangeVisibility( const VISIBILITY_MODE p_mode ) : _mode( p_mode ) {};
		explicit ChangeVisibility( Generic::BaseVisible & p_visible, const VISIBILITY_MODE p_mode ) : _mode( p_mode )
		{
			_visibles.emplace_back( &p_visible );
		}
		explicit ChangeVisibility( std::vector<Generic::BaseVisible *> & p_visibles, const VISIBILITY_MODE p_mode ) :
			_mode( p_mode )
		{
			_visibles.resize( p_visibles.size() );
			for ( int i = 0; i < p_visibles.size(); i++ )
				_visibles[ i ] = p_visibles[ i ];
		}

		virtual void execute() override;

	  protected:
		std::vector<Generic::BaseVisible *> _visibles = std::vector<Generic::BaseVisible *>();
		const VISIBILITY_MODE				_mode;

		bool _getVisibilityBool( const Generic::BaseVisible & visible ) const;
		bool _getVisibilityBool() const;
	};
} // namespace VTX::Action::Visible
#endif
