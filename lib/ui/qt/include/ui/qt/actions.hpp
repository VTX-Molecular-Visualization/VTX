#ifndef __VTX_UI_QT_ACTIONS__
#define __VTX_UI_QT_ACTIONS__

#include "app/services.hpp"
#include "ui/qt/style/icons.hpp"
#include <QStyle>
#include <app/ui/concepts.hpp>

namespace VTX::UI::QT
{
	namespace Action
	{
		// System.
		namespace System
		{
			struct New : App::UI::DescAction
			{
				New();
			};

			struct Download : App::UI::DescAction
			{
				Download();
			};

			struct Open : App::UI::DescAction
			{
				Open();
			};

			struct OpenRecent : App::UI::DescAction
			{
				OpenRecent();
			};

			struct Save : App::UI::DescAction
			{
				Save();
			};

			struct SaveAs : App::UI::DescAction
			{
				SaveAs();
			};

			struct Import : App::UI::DescAction
			{
				Import();
			};

			struct Export : App::UI::DescAction
			{
				Export();
			};

			struct Quit : App::UI::DescAction
			{
				Quit();
			};
		} // namespace System

		// Camera.
		namespace Camera
		{
			struct Orthographic : App::UI::DescAction
			{
				Orthographic();
			};

			struct Perspective : App::UI::DescAction
			{
				Perspective();
			};

			struct Trackball : App::UI::DescAction
			{
				Trackball();
			};

			struct Freefly : App::UI::DescAction
			{
				Freefly();
			};

			struct Orient : App::UI::DescAction
			{
				Orient();
			};

			struct Reset : App::UI::DescAction
			{
				Reset();
			};
		} // namespace Camera
		namespace Snapshot
		{
			struct Snapshot : App::UI::DescAction
			{
				Snapshot();
			};

			struct Export : App::UI::DescAction
			{
				Export();
			};
		} // namespace Snapshot
		namespace Theme
		{
			struct System : App::UI::DescAction
			{
				System();
			};

			struct Light : App::UI::DescAction
			{
				Light();
			};

			struct Dark : App::UI::DescAction
			{
				Dark();
			};

			struct ResetLayout : App::UI::DescAction
			{
				ResetLayout();
			};

			struct SetFont : App::UI::DescAction
			{
				SetFont();
			};
		} // namespace Theme
		namespace Help
		{
			struct Documentation : App::UI::DescAction
			{
				Documentation();
			};

			struct Report : App::UI::DescAction
			{
				Report();
			};

			struct CheckUpdates : App::UI::DescAction
			{
				CheckUpdates();
			};

			struct About : App::UI::DescAction
			{
				About();
			};

		} // namespace Help
		namespace Selection
		{
			struct Lock : App::UI::DescAction
			{
				Lock();
			};

			struct Save : App::UI::DescAction
			{
				Save();
			};

			struct Clear : App::UI::DescAction
			{
				Clear();
			};

			struct SetGranularitySystem : App::UI::DescAction
			{
				SetGranularitySystem();
			};

			struct SetGranularityChain : App::UI::DescAction
			{
				SetGranularityChain();
			};

			struct SetGranularityResidue : App::UI::DescAction
			{
				SetGranularityResidue();
			};

			struct SetGranularityAtom : App::UI::DescAction
			{
				SetGranularityAtom();
			};

			struct Show : App::UI::DescAction
			{
				Show();
			};

			struct Hide : App::UI::DescAction
			{
				Hide();
			};

			struct Solo : App::UI::DescAction
			{
				Solo();
			};

			struct SetColorScheme : App::UI::DescAction
			{
				SetColorScheme();
			};

			struct SetRepresentation : App::UI::DescAction
			{
				SetRepresentation();
			};

			struct Delete : App::UI::DescAction
			{
				Delete();
			};
		} // namespace Selection
		namespace Preset
		{
			template<typename P>
			struct Add : App::UI::DescAction
			{
				Add()
				{
					name = "New";
					tip	 = "Create a new empty preset";
					icon = Style::Icons::NEW;
				}
			};

			template<typename P>
			struct Duplicate : App::UI::DescAction
			{
				Duplicate()
				{
					name = "Duplicate";
					tip	 = "Create a new preset from this one";
					icon = Style::Icons::COPY;
				}
			};

			template<typename P>
			struct Delete : App::UI::DescAction
			{
				Delete()
				{
					name = "Delete";
					tip	 = "Delete this preset";
					icon = Style::Icons::DELETE;
				}
			};

			template<typename P>
			struct Apply : App::UI::DescAction
			{
				Apply()
				{
					name = "Apply";
					tip	 = "Apply this preset";
					icon = Style::Icons::APPLY;
				}
			};
		} // namespace Preset
		namespace Option
		{
			namespace Cache
			{
				struct Open : App::UI::DescAction
				{
					Open();
				};

				struct Clear : App::UI::DescAction
				{
					Clear();
				};

				struct Refresh : App::UI::DescAction
				{
					Refresh();
				};
			} // namespace Cache
		} // namespace Option
	} // namespace Action

} // namespace VTX::UI::QT

#endif
