#ifndef VTX_APP_INPUT_INPUT_MANAGER_HPP
#define VTX_APP_INPUT_INPUT_MANAGER_HPP

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
		enum struct Action : uint8_t
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
		InputManager() noexcept
		{
			clearActions();
			consume();
		}

		/**
		 * @brief Accessors and mutators.
		 */
		bool hasAction( const Action p_action ) const noexcept
		{ return _actions[ static_cast<std::size_t>( p_action ) ]; }

		Vec3i translationAxis() const noexcept
		{
			return Vec3i(
				static_cast<int>( hasAction( Action::MoveRight ) ) - static_cast<int>( hasAction( Action::MoveLeft ) ),
				static_cast<int>( hasAction( Action::MoveUp ) ) - static_cast<int>( hasAction( Action::MoveDown ) ),
				static_cast<int>( hasAction( Action::MoveBack ) )
					- static_cast<int>( hasAction( Action::MoveFront ) ) );
		}

		int rotationAxis() const noexcept
		{
			return static_cast<int>( hasAction( Action::RotateRight ) )
				   - static_cast<int>( hasAction( Action::RotateLeft ) );
		}

		bool accelerate() const noexcept { return hasAction( Action::Accelerate ); }

		bool decelerate() const noexcept { return hasAction( Action::Decelerate ); }

		const Vec2i & rotate() const noexcept { return _rotate; }

		const Vec2i & rotateAlt() const noexcept { return _rotateAlt; }

		const Vec2i & pan() const noexcept { return _pan; }

		int zoom() const noexcept { return _zoom; }

		/**
		 * @brief Mutators.
		 */
		void setAction( const Action p_action, const bool p_value ) noexcept
		{ _actions[ static_cast<std::size_t>( p_action ) ] = p_value; }

		void clearActions() noexcept { _actions.fill( false ); }

		void rotateBy( const Vec2i & p_delta ) noexcept { _rotate += p_delta; }

		void rotateAltBy( const Vec2i & p_delta ) noexcept { _rotateAlt += p_delta; }

		void panBy( const Vec2i & p_delta ) noexcept { _pan += p_delta; }

		void zoomBy( const int p_delta ) noexcept { _zoom += p_delta; }

		/**
		 * @brief Consume data (reset to zero).
		 */
		void consume() noexcept
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
		Vec2i _rotate = Vec2i( 0 );

		/**
		 * @brief Mouse right drag action.
		 */
		Vec2i _rotateAlt = Vec2i( 0 );

		/**
		 * @brief Mouse middle drag action.
		 */
		Vec2i _pan = Vec2i( 0 );

		/**
		 * @brief Wheel action.
		 *
		 */
		int _zoom {};
	};
} // namespace VTX::App::Input

#endif
