#pragma once

namespace VTX::Test::ExternalTool::Action
{

	// Tool that allow to test if the binding with python is actually taking place.
	class ToolAction
	{
	  public:
		static void reset() noexcept;
		static bool executed() noexcept;

	  public:
		void execute();
	};
} // namespace VTX::Test::ExternalTool::Action
