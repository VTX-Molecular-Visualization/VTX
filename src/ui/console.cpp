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
		void Console::receiveEvent( const Event::VTXEvent & p_event )
		{
			if ( p_event.name == Event::Global::LOG_CONSOLE )
			{
				const Event::VTXEventLog & event = dynamic_cast<const Event::VTXEventLog &>( p_event );
				// Copy event to keep history.
				_addLog( event );
			}
		}

		void Console::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );
			ImGui::SetNextWindowBgAlpha( IMGUI_STYLE_BG_ALPHA );

			if ( ImGui::Begin( LOCALE( "Console.Console" ), isVisiblePtr(), flags ) == false )
			{
				ImGui::End();
				return;
			}
			for ( const Event::VTXEventLog & log : _logs )
			{
				bool popColor = false;
				if ( log.level == "ERROR" )
				{
					ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.f, 0.f, 0.f, 1.f ) );
					popColor = true;
				}
				if ( log.level == "WARNING" )
				{
					ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.f, 1.f, 0.f, 1.f ) );
					popColor = true;
				}
				if ( log.level == "DEBUG" )
				{
					ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.f, 1.f, 0.f, 1.f ) );
					popColor = true;
				}
				ImGui::Selectable( ( "[" + log.date + "] " + "[" + log.level + "] " + log.message ).c_str() );
				if ( popColor ) { ImGui::PopStyleColor(); }
			}

			// Scripting.
			static char action[ 64 ] = "";
			if ( ImGui::InputText( "##action",
								   action,
								   IM_ARRAYSIZE( action ),
								   ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll
									   | ImGuiInputTextFlags_NoUndoRedo ) )
			{ VTX_ACTION( std::string( action ) ); }

			if ( ImGui::GetScrollY() >= ImGui::GetScrollMaxY() ) { ImGui::SetScrollHereY( 1.0f ); }

			ImGui::End();
		}
		void Console::_addLog( const Event::VTXEventLog & p_log )
		{
			_logs.emplace_back( p_log );
			if ( _logs.size() > VTX_CONSOLE_SIZE ) { _logs.pop_front(); }
		}

		void Console::_clear() { _logs.clear(); }
	} // namespace UI
} // namespace VTX
