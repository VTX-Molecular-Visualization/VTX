#ifndef __VTX_APP_ACTION_BASE_ACTION__
#define __VTX_APP_ACTION_BASE_ACTION__

namespace VTX::App::Action
{
	/**
	 * @brief Base class for all actions.
	 */
	class BaseAction
	{
	  public:
		virtual ~BaseAction() = default;

		/**
		 * @brief Execute this action.
		 */
		virtual void execute() = 0;
	};

} // namespace VTX::App::Action
#endif
