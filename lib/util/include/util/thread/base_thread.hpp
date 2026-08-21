#ifndef __VTX_UTIL_THREAD_BASE_THREAD__
#define __VTX_UTIL_THREAD_BASE_THREAD__

#include <atomic>
#include <concepts>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <util/thread/stop_token.hpp>
#include <util/types.hpp>

namespace VTX::Util::Thread
{

	class ThreadManager;

	/**
	 * @brief Thread base classe, managed by ThreadManager.
	 */
	class BaseThread
	{
		friend class ThreadManager;

	  public:
		/**
		 * @brief Types.
		 */
		using AsyncOp			 = std::function<uint( StopToken, BaseThread & )>;
		using EndCallback		 = std::function<void( BaseThread &, uint, bool p_manuallyStopped )>;
		using ProgressCallback	 = std::function<void( const BaseThread & )>;
		using TerminatedCallback = std::function<void( const BaseThread & )>;
		using ID				 = std::jthread::id;

		/**
		 * @brief Wait for the thread to finish and destroy it.
		 */
		virtual ~BaseThread();

		/**
		 * @brief Waits for the thread to finish.
		 * If the thread is not joinable, does nothing.
		 */
		void wait();

		/**
		 * @brief Stops the thread.
		 * If the thread is not joinable, does nothing.
		 */
		virtual void stop();

		/**
		 * @brief Checks if the thread has ended.
		 */
		inline bool isFinished() const { return _finished.load( std::memory_order_acquire ); }

		/**
		 * @brief Checks if the thread has been manually stopped.
		 */
		inline bool isManuallyStopped() const { return _stopped.load(); }

		/**
		 * @brief Returns the id.
		 */
		inline ID getId() const { return _id; }

		/**
		 * @brief Returns the progress.
		 */
		inline float getProgress() const { return _progress.load( std::memory_order_relaxed ); }

		/**
		 * @brief Set the progress.
		 */
		void setProgress( const float p_value );

		/**
		 * @brief Progress text accessors.
		 */
		std::string getProgressText() const;
		void		setProgressText( const std::string & p_text );

		/**
		 * @brief Sets the progress callback.
		 */
		void setProgressCallback( ProgressCallback p_callback );
		void setTerminatedCallback( TerminatedCallback p_callback );

	  protected:
		/**
		 * @brief Never used directly.
		 * ThreadManager is responsible of the creation of BaseThread
		 * object.
		 */
		BaseThread( ThreadManager & p_manager );

		/**
		 * @brief Starts the content of the function in a new thread.
		 * Once the function returns
		 * p_callback is called.
		 */
		void _start( const AsyncOp & p_function, const EndCallback & p_callback = {} );

	  private:
		/**
		 * @brief Reference to the thread manager that owns this thread.
		 */
		ThreadManager & _manager;

		/**
		 * @brief Thread std object.
		 */
		std::jthread _thread;

		/**
		 * @brief Thread id.
		 */
		ID _id;

		/**
		 * @brief Current progress [0, 1].
		 */
		std::atomic<float> _progress = 0.f;

		/**
		 * @brief Flag as finised.
		 */
		std::atomic<bool> _finished = false;

		/**
		 * @brief Flag as stopped.
		 */
		std::atomic<bool> _stopped = false;

		/**
		 * @brief Mutex for progress text.
		 */
		mutable std::mutex _progressTextMutex;

		/**
		 * @brief Progress text.
		 */
		std::string _progressText;

		/**
		 * @brief Callbacks.
		 */
		ProgressCallback   _progressCallback;
		TerminatedCallback _terminatedCallback;
	};

	/**
	 * @brief Concept for derived threads.
	 */
	template<typename T>
	concept ManagedThread = std::derived_from<T, BaseThread> && requires( T & p_thread ) {
		{ p_thread.start() } -> std::same_as<void>;
	};

	using OptionalThreadReference = std::optional<std::reference_wrapper<BaseThread>>;

	/**
	 * @brief Short-hand for types that are usually used together
	 */
	struct ThreadData
	{
		StopToken				stopToken;
		OptionalThreadReference thrRef;
	};
} // namespace VTX::Util::Thread
#endif
