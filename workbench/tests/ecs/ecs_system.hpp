#pragma once

#include "ecs/_ecs.hpp"
#include "ecs/registry.hpp"

using namespace ECS;
using Entity = EntityType;

namespace Primitives
{
	////////////////////////////////////////////////////////////////////////////////

	struct Position
	{
		int x_{ 0 };
		int y_{ 0 };

		Position() = default;
		Position(int x, int y)
			: x_(x)
			, y_(y)
		{
		}
	};

	struct Velocity
	{
		int x_{ 0 };
		int y_{ 0 };

		Velocity() = default;
		Velocity(int x, int y)
			: x_(x)
			, y_(y)
		{
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	struct UpdateEvent
	{
		int dt_ = 0;
	};

	////////////////////////////////////////////////////////////////////////////////

	class GravitySystem : public System<UpdateEvent>
	{
		int g_ = 0;
	public:
		GravitySystem(int g)
			: g_(g)
		{
		}

		void Process(Registry& registry, UpdateEvent&& event) override
		{
			registry.GetView<Velocity>().Each
			(
				[this, &event](auto&&, Velocity& velocity)
				{
					velocity.x_ += g_ * event.dt_;
					velocity.y_ += g_ * event.dt_;
				}
			);
		}
	};

	class MovementSystem : public System<UpdateEvent>
	{
	public:
		void Process(Registry& registry, UpdateEvent&& event) override
		{
			registry.GetView<Position, Velocity>().Each
			(
				[this, &event](auto&&, Position& position, Velocity& velocity)
			{
				position.x_ += velocity.x_ * event.dt_;
				position.y_ += velocity.y_ * event.dt_;
			}
			);
		}
	};
}

class ECS_System : public Template
{
};