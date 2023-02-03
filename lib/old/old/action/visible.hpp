#ifndef __VTX_ACTION_VISIBLE__
#define __VTX_ACTION_VISIBLE__

#include "base_action.hpp"
#include "generic/base_visible.hpp"
#include "tool/logger.hpp"
#include <vector>

namespace VTX::Action::Visible
{
	class ChangeVisibility : public BaseAction
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

		virtual void execute() override
		{
			for ( Generic::BaseVisible * const visible : _visibles )
			{
				const bool newVisibility = _getVisibilityBool( *visible );
				visible->setVisible( newVisibility );
			}
		}

	  protected:
		std::vector<Generic::BaseVisible *> _visibles = std::vector<Generic::BaseVisible *>();
		const VISIBILITY_MODE				_mode;

		inline bool _getVisibilityBool( const Generic::BaseVisible & visible ) const
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
		inline bool _getVisibilityBool() const
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
	};
} // namespace VTX::Action::Visible
#endif
