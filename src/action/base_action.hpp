#ifndef __VTX_BASE_ACTION__
#define __VTX_BASE_ACTION__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Action
	{
		class BaseAction
		{
		  public:
			virtual void execute()	   = 0;
			virtual bool canBeUndone() = 0;
			virtual void undo() {};

		  private:
			virtual void displayUsage() {}
		};
	} // namespace Action
} // namespace VTX
#endif
