#ifndef __VTX_APP_PASS_ACTION_EXECUTER__
#define __VTX_APP_PASS_ACTION_EXECUTER__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include <tuple>

namespace VTX::App::Pass
{
	class QueuedAction;

	void subscribe( QueuedAction ) noexcept;

	template<typename SomeAction>
	class QueueAction
	{
	  public:
		template<typename... Args>
		inline void execute( Args &&... args )
		{
			subscribe( QueuedAction( SomeAction(), std::forward<Args>( args )... ) );
		}
	};

	class ActionExecuter : public IPass
	{
	  public:
		ActionExecuter();

		void update( const float, const float );

	  private:
		ECS::Entity _queueEntity = entt::null;
		float		_skipTime	 = 0.f;
	};

	class QueuedAction
	{
	  public:
		QueuedAction() = default;
		inline void execute() { _ptr->execute(); }

	  private:
		struct _interface
		{
			virtual ~_interface()  = default;
			virtual void execute() = 0;
		};
		struct _dummy
		{
		};
		template<typename SomeAction, typename... Args>
		class _wrapper final : public _interface
		{
			SomeAction _obj;

			std::tuple<std::remove_reference_t<Args>...> _args;

		  public:
			_wrapper() = delete;
			_wrapper( SomeAction && p_action, Args &&... args ) :
				_obj( std::forward<SomeAction>( p_action ) ),
				_args( std::make_tuple<Args>( std::forward<Args>( args ) )... )
			{
			}
			void execute() override
			{
				if constexpr ( not std::same_as<SomeAction, _dummy> )
				{
					auto exec = [ this ]( auto... args ) { _obj.execute( args... ); };
					std::apply( exec, _args );
				}
			}
		};
		std::unique_ptr<_interface> _ptr = std::make_unique<_wrapper<_dummy>>();

	  public:
		template<typename SomeAction, typename... Args>
		QueuedAction( SomeAction && p_action, Args &&... args ) :
			_ptr(
				new _wrapper<SomeAction, Args...>( std::forward<SomeAction>( p_action ), std::forward<Args>( args )... )
			)
		{
		}
		template<typename SomeAction>
		QueuedAction( SomeAction && p_action ) :
			_ptr( new _wrapper<SomeAction>( std::forward<SomeAction>( p_action ) ) )
		{
		}
	};
} // namespace VTX::App::Pass

#endif
