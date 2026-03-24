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
			struct New : public App::UI::DescAction
			{
				New();
			};

			struct Download : public App::UI::DescAction
			{
				Download();
			};

			struct Open : public App::UI::DescAction
			{
				Open();
			};

			struct OpenRecent : public App::UI::DescAction
			{
				OpenRecent();
			};

			struct Delete : public App::UI::DescAction
			{
				Delete();
			};

			struct Save : public App::UI::DescAction
			{
				Save();
			};

			struct SaveAs : public App::UI::DescAction
			{
				SaveAs();
			};

			struct Import : public App::UI::DescAction
			{
				Import();
			};

			struct Export : public App::UI::DescAction
			{
				Export();
			};

			struct Quit : public App::UI::DescAction
			{
				Quit();
			};
		} // namespace System

		// Camera.
		namespace Camera
		{
			struct Orthographic : public App::UI::DescAction
			{
				Orthographic();
			};

			struct Perspective : public App::UI::DescAction
			{
				Perspective();
			};

			struct Trackball : public App::UI::DescAction
			{
				Trackball();
			};

			struct Freefly : public App::UI::DescAction
			{
				Freefly();
			};

			struct Orient : public App::UI::DescAction
			{
				Orient();
			};

			struct Reset : public App::UI::DescAction
			{
				Reset();
			};
		} // namespace Camera
		namespace Snapshot
		{
			struct Snapshot : public App::UI::DescAction
			{
				Snapshot();
			};

			struct Export : public App::UI::DescAction
			{
				Export();
			};
		} // namespace Snapshot
		namespace Theme
		{
			struct System : public App::UI::DescAction
			{
				System();
			};

			struct Light : public App::UI::DescAction
			{
				Light();
			};

			struct Dark : public App::UI::DescAction
			{
				Dark();
			};

			struct ResetLayout : public App::UI::DescAction
			{
				ResetLayout();
			};

			struct SetFont : public App::UI::DescAction
			{
				SetFont();
			};
		} // namespace Theme
		namespace Help
		{
			struct Documentation : public App::UI::DescAction
			{
				Documentation();
			};

			struct Report : public App::UI::DescAction
			{
				Report();
			};

			struct CheckUpdates : public App::UI::DescAction
			{
				CheckUpdates();
			};

			struct About : public App::UI::DescAction
			{
				About();
			};

		} // namespace Help
		namespace Selection
		{
			struct Lock : public App::UI::DescAction
			{
				Lock();
			};

			struct Save : public App::UI::DescAction
			{
				Save();
			};

			struct Clear : public App::UI::DescAction
			{
				Clear();
			};

			struct SetGranularitySystem : public App::UI::DescAction
			{
				SetGranularitySystem();
			};

			struct SetGranularityChain : public App::UI::DescAction
			{
				SetGranularityChain();
			};

			struct SetGranularityResidue : public App::UI::DescAction
			{
				SetGranularityResidue();
			};

			struct SetGranularityAtom : public App::UI::DescAction
			{
				SetGranularityAtom();
			};
		} // namespace Selection
		namespace Preset
		{
			template<typename P>
			struct Add : public App::UI::DescAction
			{
				Add()
				{
					name = "New";
					tip	 = "Create a new empty preset";
					icon = Style::Icons::NEW;
				}
			};

			template<typename P>
			struct Duplicate : public App::UI::DescAction
			{
				Duplicate()
				{
					name = "Duplicate";
					tip	 = "Create a new preset from this one";
					icon = Style::Icons::COPY;
				}
			};

			template<typename P>
			struct Delete : public App::UI::DescAction
			{
				Delete()
				{
					name = "Delete";
					tip	 = "Delete this preset";
					icon = Style::Icons::DELETE;
				}
			};

			template<typename P>
			struct Apply : public App::UI::DescAction
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
				struct Open : public App::UI::DescAction
				{
					Open();
				};

				struct Clear : public App::UI::DescAction
				{
					Clear();
				};

				struct Refresh : public App::UI::DescAction
				{
					Refresh();
				};
			} // namespace Cache
		} // namespace Option
	} // namespace Action

} // namespace VTX::UI::QT

#endif
