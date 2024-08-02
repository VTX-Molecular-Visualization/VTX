#include <app/ui/base_application.hpp>
#include <example/tool.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Example
{
	Tool::Tool() {}

	Tool::~Tool() {}

	void Tool::init() { VTX_DEBUG( "Tool::init" ); }

	void Tool::onAppStart() { VTX_DEBUG( "Tool::onAppStart" ); }

	void Tool::createUI() { VTX_DEBUG( "Tool::createUI" ); }

	void Tool::onAppStop() { VTX_DEBUG( "Tool::onAppStop" ); }
} // namespace VTX::Tool::Example
