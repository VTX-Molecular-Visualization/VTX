#ifndef __VTX_APP_INPUT_INPUT_MANAGER__
#define __VTX_APP_INPUT_INPUT_MANAGER__

#include <array>
#include <util/types.hpp>

namespace VTX::App::Input
{
	class InputManager
	{
	  public:
		/**
		 * @brief Available input actions.
		 */
		enum struct Action : int
		{
			MoveFront = 0,
			MoveBack,
			MoveLeft,
			MoveRight,
			MoveUp,
			MoveDown,
			RotateLeft,
			RotateRight,
			Accelerate,
			Decelerate,
			Count
		};

		/**
		 * @brief Constructor.
		 */
		InputManager()
		{
			clearActions();
			consume();
		}

		/**
		 * @brief Accessors and mutators.
		 */
		inline bool hasAction( const Action p_action ) const
		{
			return _actions[ static_cast<std::size_t>( p_action ) ];
		}

		inline Vec3i translationAxis() const
		{
			return Vec3i(
				static_cast<int>( hasAction( Action::MoveRight ) ) - static_cast<int>( hasAction( Action::MoveLeft ) ),
				static_cast<int>( hasAction( Action::MoveUp ) ) - static_cast<int>( hasAction( Action::MoveDown ) ),
				static_cast<int>( hasAction( Action::MoveBack ) ) - static_cast<int>( hasAction( Action::MoveFront ) )
			);
		}

		inline int rotationAxis() const
		{
			return static_cast<int>( hasAction( Action::RotateRight ) )
				   - static_cast<int>( hasAction( Action::RotateLeft ) );
		}

		inline bool accelerate() const { return hasAction( Action::Accelerate ); }
		inline bool decelerate() const { return hasAction( Action::Decelerate ); }

		inline const Vec2i & rotate() const { return _rotate; }
		inline const Vec2i & rotateAlt() const { return _rotateAlt; }
		inline const Vec2i & pan() const { return _pan; }
		inline int			 zoom() const { return _zoom; }

		/**
		 * @brief Mutators.
		 */
		inline void setAction( const Action p_action, const bool p_value )
		{
			_actions[ static_cast<std::size_t>( p_action ) ] = p_value;
		}

		inline void clearActions() { _actions.fill( false ); }

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
		 * @brief Continuous actions.
		 */
		std::array<bool, static_cast<std::size_t>( Action::Count )> _actions = {};

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
