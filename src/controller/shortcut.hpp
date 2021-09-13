#ifndef __VTX_CONTROLLER_SHORTCUT__
#define __VTX_CONTROLLER_SHORTCUT__

#include "base_keyboard_controller.hpp"
#include "id.hpp"

namespace VTX
{
	namespace Controller
	{
		enum class SHORTCUTGROUP
		{
			DEFAULT,
			RENDER,
			SCENE,
		};

		class Shortcut : public BaseKeyboardController
		{
		  public:
			Shortcut() = default;
			Shortcut( const SHORTCUTGROUP & p_group ) : BaseKeyboardController(), _currentGroup( p_group ) {};

			const SHORTCUTGROUP & getGroup() const { return _currentGroup; };
			void				  setGroup( const SHORTCUTGROUP & p_group ) { _currentGroup = p_group; };
			void removeGroup( const SHORTCUTGROUP & p_group ) { _currentGroup = SHORTCUTGROUP::DEFAULT; };

		  protected:
			void _handleKeyDownEvent( const Qt::Key & ) override;
			void update( const float & ) override {}

		  private:
			bool		  _handleDefaultShortcut( const Qt::Key & );
			bool		  _handleRenderShortcut( const Qt::Key & );
			bool		  _handleSceneShortcut( const Qt::Key & );
			SHORTCUTGROUP _currentGroup = SHORTCUTGROUP::DEFAULT;
		};
	} // namespace Controller
} // namespace VTX
#endif
