#ifndef __VTX_APP_SYSTEM_TRAJECTORY__
#define __VTX_APP_SYSTEM_TRAJECTORY__

#include <span>

namespace VTX
{
	struct Vec3f;
}

namespace VTX::App::System
{
	/**
	 * @brief Enumerates playing mode for a trajectory
	 */
	enum class TrajectoryPlayMode : uint8_t
	{
		none,	  // only one frame
		forward,  // loop at the begining upon reaching the last
		backward, // loop at the end upon reaching the first
		pingpong  // forward then backward when end is reached
	};

	/**
	 * @brief Responsible for holding positional data of a list of atoms.
	 */
	struct Trajectory
	{
		TrajectoryPlayMode playMode = TrajectoryPlayMode::none; // Help the trajectory reader to schedule frame reading.
		uint			   requestedFrameIndex = 0;				// Here lies the Frame index that is requested.
		uint			   currentFrameIndex   = 0;				// Here is the actual index related to the positions.
		std::span<Vec3f>   currentAtomPositions;				// atom positions at the current index.
	};

	/**
	 * @brief Responsible for bridging the frame update to a generic frame reader
	 */
	class TrajectoryManager
	{
		struct _interface
		{
			virtual ~_interface()																		 = default;
			virtual void fillFrameIfAvailable( const uint & p_index, std::span<Vec3f> & p_out ) noexcept = 0;
		};
		struct _dummy
		{
		};
		template<typename T>
		class _wrapper final : public _interface
		{
			T _obj;

		  public:
			_wrapper( T && p_arg ) : _obj( std::forward( p_arg ) ) {}
			virtual void fillFrameIfAvailable( const uint & p_index, std::span<Vec3f> & p_out ) noexcept override
			{
				if constexpr ( not std::same_as<T, _dummy> )
				{
					_obj.fillFrameIfAvailable( p_index, p_out );
				}
			}
		};

		std::unique_ptr<_interface> _ptr = std::make_unique<_wrapper<_dummy>>();

	  public:
		TrajectoryManager() = default;

		template<typename TrajectoryReaderT>
		TrajectoryManager( TrajectoryReaderT && arg ) : _ptr( new _wrapper<TrajectoryReaderT>( std::forward( arg ) ) )
		{
		}

		void fillFrameIfAvailable( const uint & p_index, std::span<Vec3f> & p_out ) noexcept {}
	};

	/**
	 * @brief Responsible for querying the desired frame from the TrajectoryManager and submitting it to the Trajectory
	 * as the new current frame.
	 */
	class TrajectoryUpdater
	{
	  public:
		TrajectoryUpdater( TrajectoryManager p_trajManager ) noexcept;

		/**
		 * @brief Uses the requested frame index to update the current atom position
		 * @param
		 */
		void update( Trajectory & ) noexcept;
	};
} // namespace VTX::App::System

#endif
