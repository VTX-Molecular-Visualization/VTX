#ifndef __VTX_STATE_PLAY__
#define __VTX_STATE_PLAY__

#include "base_state.hpp"

namespace VTX
{
	namespace Model
	{
		class Path;
	}

	namespace State
	{
		class Play : public BaseState
		{
		  public:
			Play() = default;

			virtual void enter( void * const ) override;
			virtual void exit() override;
			virtual void update( const float & ) override;

		  private:
			Model::Path *					 _path	  = nullptr;
			const std::vector<std::string> * _actions = nullptr;
			float							 _time	  = 0.f;

			void _setCamera() const;
			void _executeActions( const float p_time );
		};
	} // namespace State
} // namespace VTX
#endif
