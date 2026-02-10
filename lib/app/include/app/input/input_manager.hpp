#ifndef __VTX_APP_INPUT_INPUT_MANAGER__
#define __VTX_APP_INPUT_INPUT_MANAGER__

#include <util/types.hpp>

namespace VTX::App::Input
{
	class InputManager
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		InputManager() { consume(); }

		/**
		 * @brief Accessors and mutators.
		 */
		inline bool moveFront() const { return _moveFront; }
		inline bool moveBack() const { return _moveBack; }
		inline bool moveLeft() const { return _moveLeft; }
		inline bool moveRight() const { return _moveRight; }
		inline bool moveUp() const { return _moveUp; }
		inline bool moveDown() const { return _moveDown; }
		inline bool rotateLeft() const { return _rotateLeft; }
		inline bool rotateRight() const { return _rotateRight; }
		inline bool accelerate() const { return _accelerate; }
		inline bool decelerate() const { return _decelerate; }

		inline const Vec2i & rotate() const { return _rotate; }
		inline const Vec2i & rotateAlt() const { return _rotateAlt; }
		inline const Vec2i & pan() const { return _pan; }
		inline int			 zoom() const { return _zoom; }

		/**
		 * @brief Mutators.
		 */
		inline void setMoveFront( const bool p_value ) { _moveFront = p_value; }
		inline void setMoveBack( const bool p_value ) { _moveBack = p_value; }
		inline void setMoveLeft( const bool p_value ) { _moveLeft = p_value; }
		inline void setMoveRight( const bool p_value ) { _moveRight = p_value; }
		inline void setMoveUp( const bool p_value ) { _moveUp = p_value; }
		inline void setMoveDown( const bool p_value ) { _moveDown = p_value; }
		inline void setRotateLeft( const bool p_value ) { _rotateLeft = p_value; }
		inline void setRotateRight( const bool p_value ) { _rotateRight = p_value; }
		inline void setAccelerate( const bool p_value ) { _accelerate = p_value; }
		inline void setDecelerate( const bool p_value ) { _decelerate = p_value; }

		inline void rotateBy( const Vec2i & p_delta ) { _rotate += p_delta; }
		inline void rotateAltBy( const Vec2i & p_delta ) { _rotateAlt += p_delta; }
		inline void panBy( const Vec2i & p_delta ) { _pan += p_delta; }
		inline void zoomBy( const int p_delta ) { _zoom += p_delta; }

		/**
		 * @brief Consume data (reset to zero).
		 */
		void consume()
		{
			_rotate	   = Vec2i( 0 );
			_rotateAlt = Vec2i( 0 );
			_pan	   = Vec2i( 0 );
			_zoom	   = 0;
		}

	  private:
		/**
		 * @brief Keys.
		 */
		bool _moveFront	  = false;
		bool _moveBack	  = false;
		bool _moveLeft	  = false;
		bool _moveRight	  = false;
		bool _moveUp	  = false;
		bool _moveDown	  = false;
		bool _rotateLeft  = false;
		bool _rotateRight = false;

		/**
		 * @brief Modifiers.
		 */
		bool _accelerate = false;
		bool _decelerate = false;

		/**
		 * @brief Mouse left drag action.
		 */
		Vec2i _rotate;

		/**
		 * @brief Mouse right drag action.
		 */
		Vec2i _rotateAlt;

		/**
		 * @brief Mouse middle drag action.
		 */
		Vec2i _pan;

		/**
		 * @brief Wheel action.
		 *
		 */
		int _zoom;
	};
} // namespace VTX::App::Input

#endif
