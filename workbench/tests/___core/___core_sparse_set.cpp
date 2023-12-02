#include "_tests.hpp"

#include "core_sparse_set.hpp"

#include "ecs/_ecs.hpp"

using namespace ECS;
using Entity = EntityType;

TEST_F(CoreSparseSet, Basics)
{
	SparseSet<EntityType> set;

	set.Reserve(16);

	ASSERT_EQ(set.Capacity(), 16);

	ASSERT_TRUE(set.Empty());

	ASSERT_EQ(set.Size(), 0u);

	ASSERT_EQ(set.begin(), set.end());
	ASSERT_EQ(set.cbegin(), set.cend());
	ASSERT_EQ(std::as_const(set).begin(), std::as_const(set).end());

	ASSERT_FALSE(set.Contains(EntityType(0)));
	ASSERT_FALSE(set.Contains(EntityType(42)));

	////////////////////////////////////////////////////////////////////////////////

	set.Emplace(EntityType(16));
	ASSERT_EQ(set.Index(EntityType(16)), 0u);
	ASSERT_FALSE(set.Empty());
	ASSERT_EQ(set.Size(), 1u);
	ASSERT_NE(set.begin(), set.end());
	ASSERT_NE(set.cbegin(), set.cend());
	ASSERT_FALSE(set.Contains(EntityType(0)));
	ASSERT_TRUE(set.Contains(EntityType(16)));
	ASSERT_EQ(set.Index(EntityType(16)), 0u);

	////////////////////////////////////////////////////////////////////////////////

	set.Erase(EntityType(16));
	ASSERT_TRUE(set.Empty());
	ASSERT_EQ(set.Size(), 0u);
	ASSERT_EQ(set.begin(), set.end());
	ASSERT_EQ(set.cbegin(), set.cend());
	ASSERT_FALSE(set.Contains(EntityType(0)));
	ASSERT_FALSE(set.Contains(EntityType(16)));
	set.Emplace(EntityType(16));
	ASSERT_EQ(set.Index(EntityType(16)), 0u);

	////////////////////////////////////////////////////////////////////////////////

	ASSERT_TRUE(std::is_move_constructible_v<decltype(set)>);
	ASSERT_TRUE(std::is_move_assignable_v<decltype(set)>);

	Core::SparseSet set_other(std::move(set));

	set = std::move(set_other);

	set_other = std::move(set);

	ASSERT_TRUE(set.Empty());
	ASSERT_FALSE(set_other.Empty());
	ASSERT_EQ(set_other.Index(EntityType(16)), 0u);

	set_other.Clear();

	ASSERT_TRUE(set_other.Empty());
	ASSERT_EQ(set_other.Size(), 0u);
	ASSERT_EQ(set.begin(), set.end());
	ASSERT_EQ(set.cbegin(), set.cend());
	ASSERT_FALSE(set.Contains(EntityType(0)));
	ASSERT_FALSE(set.Contains(EntityType(16)));
}