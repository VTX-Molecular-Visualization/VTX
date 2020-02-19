#include "console.hpp"
#include "define.hpp"
#include "event/event.hpp"
#include "style.hpp"
#include "util/logger.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		void Console::receiveEvent( const Event::VTX_EVENT & p_event, void * const p_arg )
		{
			if ( p_event == Event::Global::LOG_CONSOLE )
			{
				Util::Logger::Log * log = (Util::Logger::Log *)( p_arg );
				_addLog( *log );
			}
		}

		void Console::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Console.Console" ), isVisiblePtr(), flags ) == false )
			{
				ImGui::End();
				return;
			}
			for ( Util::Logger::Log log : _logs )
			{
				ImGui::Selectable( ( "[" + log.date + "] " + "[" + log.level + "] " + log.message ).c_str() );
			}

			// Scripting.
			static char action[ 64 ] = "";
			if ( ImGui::InputText( "##action",
								   action,
								   IM_ARRAYSIZE( action ),
								   ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll
									   | ImGuiInputTextFlags_NoUndoRedo ) )
			{ VTXApp::get().action( std::string( action ) ); }

			if ( ImGui::GetScrollY() >= ImGui::GetScrollMaxY() ) { ImGui::SetScrollHereY( 1.0f ); }

			ImGui::End();
		}
		void Console::_addLog( const Util::Logger::Log & p_log )
		{
			_logs.push_back( p_log );
			if ( _logs.size() > VTX_CONSOLE_SIZE ) { _logs.pop_front(); }
		}

		void Console::_clear() { _logs.clear(); }
	} // namespace UI
} // namespace VTX
