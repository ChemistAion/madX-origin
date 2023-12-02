#pragma once

namespace madX::ECS
{
	class Registry;

	template <typename... Events>
	class System;
	
	template <>
	class System<>
	{
	public:
		virtual ~System() = default;
	};
	
	template <typename Event>
	class System<Event>
		: public virtual System<>
	{
	public:
		//virtual void Process(Registry&, Event&) = 0;
		virtual void Process(Registry&, Event&&) = 0;
	};
	
	template <typename Event, typename... Events>
	class System<Event, Events...>
		: public System<Event>
		, public System<Events...>
	{
	};
}