#pragma once

#include <app/tool/base_tool.hpp>

namespace VTX::Tool::Structalign
{

	class StructAlign : public App::Tool::BaseTool
	{
	  public:
		StructAlign();
		void registerActions() override;
		void buildUI() override;
	};

} // namespace VTX::Tool::Structalign
