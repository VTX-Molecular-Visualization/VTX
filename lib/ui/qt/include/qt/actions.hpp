#ifndef __VTX_UI_QT_ACTIONS__
#define __VTX_UI_QT_ACTIONS__

#include "resources.hpp"
#include <QAction>
#include <ui/descriptors.hpp>

namespace VTX::UI::QT::Action
{
	template<typename W>
	concept ConceptAction = std::is_base_of_v<QAction, W>;

	// Static map to store all actions.
	static std::map<std::string, QAction *> ACTIONS;

	class Action : public UI::Action
	{
		Action()
		{
			ACTIONS[ name ] = new QAction( name.c_str() );
			if ( tip.has_value() )
			{
				ACTIONS[ name ]->setToolTip( tip.value().c_str() );
			}
			if ( icon.has_value() )
			{
				// ACTIONS[ name ]->setIcon( Resources::ICON( icon.value() ) );
			}
		}

		virtual ~Action() { ACTIONS.erase( name ); }

		std::optional<QString> icon = std::nullopt;
	};

	////////////////////////////////////////

	struct New : public Action
	{
		New()
		{
			name = "New";
			tip	 = "Create a new project";
		}
	};

	struct Download : public UI::Action
	{
		Download()
		{
			name = "Download";
			tip	 = "Download structure from PDB id";
		}
	};

	struct Open : public UI::Action
	{
		Open()
		{
			name = "Open";
			tip	 = "Open a project or a molecular file";
		}
	};

	struct OpenRecent : public UI::Action
	{
		OpenRecent() { name = "Open recent"; }
	};

	struct Save : public UI::Action
	{
		Save()
		{
			name = "Save";
			tip	 = "Save project";
		}
	};

	struct SaveAs : public UI::Action
	{
		SaveAs()
		{
			name = "Save as...";
			tip	 = "Copy project in a new save";
		}
	};

	struct Import : public UI::Action
	{
		Import() { name = "Import"; }
	};

	struct Export : public UI::Action
	{
		Export() { name = "Export"; }
	};

	struct Quit : public UI::Action
	{
		Quit()
		{
			name = "Quit";
			tip	 = "Exit software";
		}
	};

	struct ChangeProjection : public UI::Action
	{
		ChangeProjection()
		{
			name = "Perspective";
			tip	 = "Change projection mode (perspective/orthographic)";
			icon = ":/sprite/camera/perspective.png";
		}
	};

	struct Trackball : public UI::Action
	{
		Trackball()
		{
			name = "Trackball";
			tip	 = "Use Trackball controller";
			icon = ":/sprite/camera/trackball.png";
		}
	};

	struct Freefly : public UI::Action
	{
		Freefly()
		{
			name = "Freefly";
			tip	 = "Use Freefly controller";
			icon = ":/sprite/camera/freefly.png";
		}
	};

	struct Orient : public UI::Action
	{
		Orient()
		{
			name = "Orient";
			tip	 = "Orient camera on selection";
		}
	};

	struct Reset : public UI::Action
	{
		Reset()
		{
			name = "Reset";
			tip	 = "Reset camera";
		}
	};

} // namespace VTX::UI::QT::Action

#endif
