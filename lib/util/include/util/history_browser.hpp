#ifndef __VTX_UTIL_HISTORY_BROWSER__
#define __VTX_UTIL_HISTORY_BROWSER__

#include <string>
#include <vector>

namespace VTX::Util
{

	/**
	 * @brief Class responsible for keeping an history of string while allowing to navigate through it starting from the
	 * more recent to the older. This has been implemented for UI usage while having the feature being testable.
	 */
	class HistoryBrowser
	{
	  public:
		/**
		 * @brief Adds a new string to the history
		 * @param p_newHistory
		 */
		void add( std::string p_newHistory ) noexcept;

		/**
		 * @brief Assumes there is at least one element in the history
		 * @return
		 */
		inline std::string &	   last() noexcept { return _history.back(); }
		inline const std::string & last() const noexcept { return _history.back(); }

		/**
		 * @brief Move forward in the history if possible. Does not activate browsing by itself and does nothing if
		 * browsing isn't activated.
		 */
		void tryMoveForward() noexcept;

		/**
		 * @brief Start browsing if it hasn't started yet. If it has, return one step back in the history.
		 */
		void tryMoveBackward() noexcept;

		/**
		 * @brief Reinitialize the browsing while keeping history. Meaning tryMoveForward will do nothing and
		 * tryMoveBackward will activate browsing back up.
		 */
		void resetBrowsing() noexcept;

		/**
		 * @brief Returns the string currently being browsed.
		 * @return
		 */
		std::string currentString() const noexcept;

		/**
		 * @brief Returns whether the history is empty or not.
		 * @return
		 */
		inline bool empty() const noexcept { return _history.empty(); }

	  private:
		std::vector<std::string> _history;
		bool					 _browsingHistory = false;
		size_t					 _historyIdx	  = 0;
	};
} // namespace VTX::Util

#endif
