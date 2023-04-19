#include "app/action/visible.hpp"
#include <util/logger.hpp>

namespace VTX::Action::Visible
{
	void ChangeVisibility::execute()
	{
		for ( Generic::BaseVisible * const visible : _visibles )
		{
			const bool newVisibility = _getVisibilityBool( *visible );
			visible->setVisible( newVisibility );
		}
	}

	bool ChangeVisibility::_getVisibilityBool( const Generic::BaseVisible & visible ) const
	{
		bool newVisibility;
		switch ( _mode )
		{
		case VISIBILITY_MODE::SHOW:
		case VISIBILITY_MODE::SOLO:
		case VISIBILITY_MODE::ALL: newVisibility = true; break;
		case VISIBILITY_MODE::HIDE: newVisibility = false; break;
		case VISIBILITY_MODE::TOGGLE: newVisibility = !visible.isVisible(); break;
		default: newVisibility = visible.isVisible(); break;
		}

		return newVisibility;
	}
	bool ChangeVisibility::_getVisibilityBool() const
	{
		bool newVisibility;
		switch ( _mode )
		{
		case VISIBILITY_MODE::SHOW:
		case VISIBILITY_MODE::SOLO:
		case VISIBILITY_MODE::ALL: newVisibility = true; break;
		case VISIBILITY_MODE::HIDE: newVisibility = false; break;
		case VISIBILITY_MODE::TOGGLE:
			VTX_ERROR( "can't manage toggle without target." );
			newVisibility = false;
			break;
		default: newVisibility = false; break;
		}

		return newVisibility;
	}
} // namespace VTX::Action::Visible
