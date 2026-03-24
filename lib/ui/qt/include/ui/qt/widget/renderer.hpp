#ifndef __VTX_UI_QT_WIDGET_RENDERER__
#define __VTX_UI_QT_WIDGET_RENDERER__

#include "ui/qt/events.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QPointer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QWheelEvent>
#include <app/input/input_manager.hpp>
#include <vector>

namespace VTX::UI::QT::Widget
{
	enum struct KB_LAYOUT : uint8_t
	{
		QWERTY,
		AZERTY,
		COUNT
	};

	/**
	 * @brief Widget to present an OpenGL rendering context.
	 * No more Qt OpenGL implementation used.
	 */
	class Renderer : public BaseWidget<Renderer, QWidget>
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Positions for HUD elements.
		 */
		enum struct HUD_POSITION
		{
			TOP_LEFT,
			TOP_CENTER,
			TOP_RIGHT,
			CENTER_LEFT,
			CENTER_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_CENTER,
			BOTTOM_RIGHT
		};

		/**
		 * @brief Constructor.
		 */
		Renderer( QWidget * );
		~Renderer();

		/**
		 * @brief Get native surface, display and platform for graphic context creation.
		 */
		uintptr_t getNativeSurface() const;
		uintptr_t getNativeDisplay() const;
		uint8_t	  getNativePlatform() const;

		/**
		 * @brief Create a toolbar from type.
		 */
		template<typename TB>
		TB * const createToolBar( const HUD_POSITION p_pos )
		{
			TB * const toolBar = new TB( this );
			_addHUDWidget( toolBar, p_pos );
			return toolBar;
		}

		/**
		 * @brief Override resize.
		 */
		void resizeEvent( QResizeEvent * ) override;
		void showEvent( QShowEvent * ) override;
		void keyPressEvent( QKeyEvent * const ) override;
		void keyReleaseEvent( QKeyEvent * const ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void mouseMoveEvent( QMouseEvent * ) override;
		void mouseReleaseEvent( QMouseEvent * ) override;
		void mouseDoubleClickEvent( QMouseEvent * const ) override;
		void wheelEvent( QWheelEvent * const ) override;

	  protected:
		/**
		 * @brief Debounce callback.
		 */
		void onResizeFinished();

	  private:
		struct HUDItem
		{
			QPointer<QWidget> widget;
			HUD_POSITION	  position;
		};

		/**
		 * @brief HUD widgets and their anchor positions.
		 */
		std::vector<HUDItem> _hudItems;

		/**
		 * @brief Debounce timer for resize events.
		 */
		QTimer _resizeTimer;

		/**
		 * @brief Reference to the application's input manager.
		 */
		App::Input::InputManager & _inputManager;

		/**
		 * @brief Current keyboard layout.
		 */
		KB_LAYOUT _layout = KB_LAYOUT::QWERTY;

		/**
		 * @brief State for mouse dragging.
		 */
		QPointF _pressPos = {};
		QPointF _lastPos  = {};
		bool	_dragging = false;

		/**
		 * @brief Add a widget to the overlay at the given position.
		 */
		void _addHUDWidget( QWidget * const, const HUD_POSITION );

		/**
		 * @brief Synchronize overlay geometry.
		 */
		void _syncOverlayGeometry();

		/**
		 * @brief Handle keyboard events and forward them to the input manager.
		 */
		void _handleKeyboard( QKeyEvent * const, const bool p_enable );

		/**
		 * @brief Handle modifier keys (Shift, Ctrl).
		 */
		void _handleModifiers();

		/**
		 * @brief Convert a point from logical pixels to device pixels.
		 */
		QPoint _toDevicePixels( const QPointF & ) const;

		/**
		 * @brief Update current keyboard layout.
		 */
		void _onKBLayoutChange( const Events::KeyboardLayoutChanged & );
	};
} // namespace VTX::UI::QT::Widget

#endif
