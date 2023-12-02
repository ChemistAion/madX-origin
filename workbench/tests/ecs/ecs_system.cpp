#include "_tests.hpp"

#include "ecs_system.hpp"


TEST_F(ECS_System, Basics)
{
	using namespace Primitives;

	Registry registry;

	registry.Mount<GravitySystem>(9);
	registry.Mount<MovementSystem>();
	
	auto entity = registry.Create();
	
	registry.Emplace<Position>(entity, 1, 2);
	registry.Emplace<Velocity>(entity, 3, 4);
	
	registry.Process(UpdateEvent { 2 });

	return;
}