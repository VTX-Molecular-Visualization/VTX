#ifndef __VTX_UI_SHORTCUT__
#define __VTX_UI_SHORTCUT__

#include <QKeySequence>
#include <QShortcut>

namespace VTX::UI
{
	class Shortcut
	{
	  public:
		template<typename T, typename Func>
		static void createGlobal( const QKeySequence & p_sequence, T * const p_object, const Func & p_function )
		{
			create( p_sequence, p_object, p_function, Qt::ShortcutContext::WindowShortcut );
		}
		template<typename T, typename Func>
		static void createLocal( const QKeySequence & p_sequence, T * const p_object, const Func & p_function )
		{
			create( p_sequence, p_object, p_function, Qt::ShortcutContext::WidgetWithChildrenShortcut );
		}

	  private:
		template<typename T, typename Func>
		static void create( const QKeySequence &		p_sequence,
							T * const					p_object,
							const Func &				p_function,
							const Qt::ShortcutContext & p_context = Qt::ShortcutContext ::WindowShortcut )
		{
			QShortcut * const qshortcut = new QShortcut( p_sequence, p_object );
			qshortcut->setContext( p_context );

			p_object->connect( qshortcut, &QShortcut::activated, p_object, p_function );
		}

	  public:
		class Main
		{
		  public:
			static const QKeySequence NEW_SCENE;
			static const QKeySequence OPEN_SCENE;
			static const QKeySequence DOWNLOAD_MOLECULE;
			static const QKeySequence SAVE_SCENE;
			static const QKeySequence SAVE_AS_SCENE;

			static const QKeySequence FULLSCREEN;
			static const QKeySequence RESTORE_LAYOUT;

			static const QKeySequence SELECT_ALL;
			static const QKeySequence CLEAR_SELECTION;

			static const QKeySequence DUPLICATE_SELECTION;
			static const QKeySequence EXTRACT_SELECTION;
			static const QKeySequence DELETE_SELECTION;
			static const QKeySequence ORIENT_ON_SELECTION;

			static const QKeySequence SET_SELECTION_PICKER;
			static const QKeySequence SET_MEASUREMENT_PICKER;
		};

		class Render
		{
		  public:
			static const QKeySequence TOGGLE_CAMERA_CONTROLLER;
			static const QKeySequence RESET_CAMERA_CONTROLLER;
			static const QKeySequence TOGGLE_CAMERA;
			static const QKeySequence ADD_VIEWPOINT;
			static const QKeySequence SNAPSHOT;
			static const QKeySequence TOGGLE_ALL_OVERLAYS;
		};

		class Dev
		{
		  public:
			static const QKeySequence COMPILE_SHADER;
			static const QKeySequence ACTIVE_RENDERER;
			static const QKeySequence REFRESH_SES;

			static const QKeySequence CHANGE_RENDER_MODE;
			static const QKeySequence PRINT_CAMERA_INFOS;
		};
	};

} // namespace VTX::UI

#endif
