#ifndef __VTX_UI_QT_ACTIONS__
#define __VTX_UI_QT_ACTIONS__

#include <QAction>
#include <QActionGroup>
#include <app/core/action/action_system.hpp>
#include <app/ui/concepts.hpp>
#include <app/vtx_app.hpp>
#include <util/collection.hpp>
#include <util/hashing.hpp>

namespace VTX::UI::QT
{
	template<typename A>
	concept ConceptAction = std::is_base_of_v<App::UI::DescAction, A>;

	namespace Action
	{
		class Factory
		{
		  public:
			inline static QAction * const get( const App::UI::DescAction & p_action )
			{
				return _getOrCreate( Hash( &p_action ), p_action );
			}

			template<ConceptAction A>
			inline static QAction * get()
			{
				return _getOrCreate( VTX::Util::Collection<QAction *>::hash<A>(), A() );
			}

		  private:
			inline static VTX::Util::Collection<QAction *>		_ACTIONS;
			inline static VTX::Util::Collection<QActionGroup *> _ACTION_GROUPS;

			static QAction * const _getOrCreate( const Hash & p_hash, const App::UI::DescAction & p_action );
		};

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
				void connect() const override;
			};

			struct Perspective : public App::UI::DescAction
			{
				Perspective();
				void connect() const override;
			};

			struct Trackball : public App::UI::DescAction
			{
				Trackball();
				void connect() const override;
			};

			struct Freefly : public App::UI::DescAction
			{
				Freefly();
				void connect() const override;
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
		namespace Option
		{
			struct ShowToolBarText : public App::UI::DescAction
			{
				ShowToolBarText();
			};
		} // namespace Option

	} // namespace Action

} // namespace VTX::UI::QT

#endif
