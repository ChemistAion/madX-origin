#include "_tests.hpp"

#include "ecs_feature.hpp"

TEST_F(ECS_Feature, Basics)
{
	Registry registryA;
	Registry registryB;

	//struct EmptyType
	//{
	//};
	class EmptyType : public Feature
	{
	public:
		int x_ = 0;
		EmptyType() = default;
		EmptyType(int x)
			: x_ { x }
		{
		}
	};

	//struct OtherEmptyType
	//{
	//};
	class OtherEmptyType : public Feature
	{
	public:
		int y_ = 0;
		OtherEmptyType() = default;
	
		OtherEmptyType(int y)
			: y_{ y }
		{
		}
	};

	struct ChildTest : public EmptyType
	{
	};
	//	class ChildTest : public EmptyType//, public OtherEmptyType
	//	{
	//	public:
	//		//ChildTest() = default;
	//	};

	ASSERT_FALSE(registryA.Has<EmptyType>());
	ASSERT_FALSE(registryA.Has<void>());
	ASSERT_FALSE(registryA.Has<void*>());
	
	ASSERT_FALSE(registryB.Has<EmptyType>());
	ASSERT_FALSE(registryB.Has<void>());
	ASSERT_FALSE(registryB.Has<void*>());
	
	//auto& testA1 = registryA.Emplace<EmptyType>(11);
	registryA.Emplace<EmptyType>();
 	//auto& testA2 = registryA.Emplace<OtherEmptyType>(22);
 	registryA.Emplace<OtherEmptyType>();
	registryA.Emplace<ChildTest>();
	
	registryB.Emplace<EmptyType>();
	registryB.Emplace<OtherEmptyType>();
	registryB.Emplace<ChildTest>();
	
	ASSERT_TRUE(registryA.Has<EmptyType>());
	ASSERT_TRUE(registryA.Has<OtherEmptyType>());
	ASSERT_TRUE(registryA.Has<ChildTest>());
	
	ASSERT_TRUE(registryB.Has<EmptyType>());
	ASSERT_TRUE(registryB.Has<OtherEmptyType>());
	ASSERT_TRUE(registryB.Has<ChildTest>());

	////////////////////////////////////////////////////////////////////////////////

	//auto& testA1 = static_cast<EmptyType&>(registryA.Get<EmptyType>());
	//auto& testA2 = static_cast<OtherEmptyType&>(registryA.Get<OtherEmptyType>());

	return;
}