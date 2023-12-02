#include "_tests.hpp"

#include "core_async.hpp"

TEST_F(CoreAsync, Basics)
{
	GTEST_SKIP();

	auto func_loop = [](int value)
	{
		int output = 0;

		for (int loop = 0; loop < value; ++loop)
		{
			output += loop;
		}

		return output;
	};

	auto func_final = [](int value1, int value2)
	{
		return value1 + value2;
	};

	int value1 = 12345;
	int result1 = 76193340; //1+2+...+12345-1

	int value2 = 54321;
	int result2 = 1475358360; //1+2+...+54321-1

	auto result1a = func_loop(value1);
	auto result2a = func_loop(value2);

	ASSERT_EQ(result1, result1a);
	ASSERT_EQ(result2, result2a);
	ASSERT_EQ(result1 + result2, func_final(result1a, result2a));

	////////////////////////////////////////////////////////////////////////////////

	auto [task1, future1] = Task::CreateTask(func_loop, value1);
	auto [task2, future2] = Task::CreateTask(func_loop, value2);
	auto [task3, future3] = Task::CreateTask(func_final, future1, future2);

	ASSERT_TRUE(future1.IsValid());
	ASSERT_TRUE(future2.IsValid());
	ASSERT_TRUE(future3.IsValid());

	ASSERT_FALSE(future1.IsReady());
	ASSERT_FALSE(future2.IsReady());
	ASSERT_FALSE(future3.IsReady());

	ASSERT_TRUE(IsFuture<decltype(future1)>);
	ASSERT_TRUE(IsFuture<decltype(future2)>);
	ASSERT_TRUE(IsFuture<decltype(future3)>);

	ASSERT_TRUE(task1.IsReady());
	ASSERT_TRUE(task2.IsReady());
	ASSERT_FALSE(task3.IsReady());

	ASSERT_TRUE(task1.IsValid());
	ASSERT_TRUE(task2.IsValid());
	ASSERT_TRUE(task3.IsValid());

	task1.Invoke();
	ASSERT_TRUE(future1.IsReady());
	ASSERT_FALSE(task1.IsValid());
	ASSERT_FALSE(task3.IsReady());
	task2.Invoke();
	ASSERT_TRUE(future2.IsReady());
	ASSERT_FALSE(task2.IsValid());
	ASSERT_TRUE(task3.IsReady());

	task3.Invoke();
	ASSERT_FALSE(task3.IsValid());

	ASSERT_EQ(result1, future1.Get());
	ASSERT_EQ(result2, future2.Get());
	ASSERT_EQ(result1 + result2, future3.Get());

	////////////////////////////////////////////////////////////////////////////////

	ASSERT_TRUE(future1.IsValid());
	ASSERT_TRUE(future2.IsValid());
	ASSERT_TRUE(future3.IsValid());

	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_TRUE(future3.IsReady());

	ASSERT_TRUE(task1.IsReady());
	ASSERT_TRUE(task2.IsReady());
	ASSERT_TRUE(task3.IsReady());

	ASSERT_FALSE(task1.IsValid());
	ASSERT_FALSE(task2.IsValid());
	ASSERT_FALSE(task3.IsValid());

	return;
}

TEST_F(CoreAsync, Executor)
{
	GTEST_SKIP();

	auto func_loop = [](int value)
	{
		int output = 0;

		for (int loop = 0; loop < value; ++loop)
		{
			output += loop;
		}

		return output;
	};

	auto func_final = [](int value1, int value2)
	{
		return value1 + value2;
	};

	int value1 = 12345;
	int result1 = 76193340; //1+2+...+12345-1

	int value2 = 54321;
	int result2 = 1475358360; //1+2+...+54321-1

	////////////////////////////////////////////////////////////////////////////////

	auto [task1, future1] = Task::CreateTask(func_loop, value1);
	auto [task2, future2] = Task::CreateTask(func_loop, value2);
	auto [task3, future3] = Task::CreateTask(func_final, future1, future2);

	Executor<1024> executor;

	ASSERT_TRUE(executor.TryPush(task3));
	ASSERT_TRUE(executor.TryPush(task2));
	ASSERT_TRUE(executor.TryPush(task1));

	ASSERT_FALSE(future1.IsReady());
	ASSERT_FALSE(future2.IsReady());
	ASSERT_FALSE(future3.IsReady());
	
	ASSERT_TRUE(executor.TryProcess());
	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_FALSE(future3.IsReady());

	ASSERT_TRUE(executor.TryProcess());
	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_TRUE(future3.IsReady());

	ASSERT_TRUE(executor.TryProcess());
	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_TRUE(future3.IsReady());

	ASSERT_TRUE(executor.TryProcess());
	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_TRUE(future3.IsReady());

	ASSERT_FALSE(executor.TryProcess());
	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_TRUE(future3.IsReady());

	ASSERT_FALSE(executor.TryProcess());
	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_TRUE(future3.IsReady());

	return;
}

TEST_F(CoreAsync, QuasiWorker)
{
	GTEST_SKIP();

	using namespace std::literals::chrono_literals;

	Executor<2> executor;

//	auto processor = [&](std::stop_token token)
//	{
//		while (false == token.stop_requested())
//		{
//			if (executor.TryProcess())
//			{
//				continue;
//			}
//	
//			std::this_thread::yield();
//		}
//	
//		return;
//	};

	auto task = [](int value)
	{
		return value + 1;
	};

	////////////////////////////////////////////////////////////////////////////////

	auto [task1, future1] = Task::CreateTask(task, 111);
	auto [task2, future2] = Task::CreateTask(task, 222);
	auto [task3, future3] = Task::CreateTask(task, 333);

	ASSERT_TRUE(future1.IsValid());
	ASSERT_TRUE(future2.IsValid());
	ASSERT_TRUE(future3.IsValid());
	
	ASSERT_FALSE(future1.IsReady());
	ASSERT_FALSE(future2.IsReady());
	ASSERT_FALSE(future3.IsReady());
	
	ASSERT_TRUE(task1.IsReady());
	ASSERT_TRUE(task2.IsReady());
	ASSERT_TRUE(task3.IsReady());

	ASSERT_TRUE(task1.IsValid());
	ASSERT_TRUE(task2.IsValid());
	ASSERT_TRUE(task3.IsValid());
	
//	////////////////////////////////////////////////////////////////////////////////
	
	ASSERT_TRUE(executor.TryPush(task1));
	ASSERT_TRUE(executor.TryPush(task2));
	ASSERT_FALSE(executor.TryPush(task3));
	
	ASSERT_TRUE(future1.IsValid());
	ASSERT_TRUE(future2.IsValid());
	ASSERT_TRUE(future3.IsValid());

	ASSERT_FALSE(future1.IsReady());
	ASSERT_FALSE(future2.IsReady());
	ASSERT_FALSE(future3.IsReady());
	
	ASSERT_TRUE(task1.IsReady());
	ASSERT_TRUE(task2.IsReady());
	ASSERT_TRUE(task3.IsReady());

	ASSERT_FALSE(task1.IsValid());
	ASSERT_FALSE(task2.IsValid());
	ASSERT_TRUE(task3.IsValid());

	////////////////////////////////////////////////////////////////////////////////

	ASSERT_TRUE(executor.TryProcess());
	ASSERT_TRUE(future1.IsValid());
	ASSERT_TRUE(future2.IsValid());
	ASSERT_TRUE(future3.IsValid());
	ASSERT_TRUE(future1.IsReady());
	ASSERT_FALSE(future2.IsReady());
	ASSERT_FALSE(future3.IsReady());

	ASSERT_TRUE(executor.TryProcess());
	ASSERT_TRUE(future1.IsValid());
	ASSERT_TRUE(future2.IsValid());
	ASSERT_TRUE(future3.IsValid());
	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_FALSE(future3.IsReady());

	ASSERT_FALSE(executor.TryProcess());
	ASSERT_FALSE(executor.TryProcess());
	ASSERT_FALSE(executor.TryProcess());
	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_FALSE(future3.IsReady());

	////////////////////////////////////////////////////////////////////////////////

	ASSERT_FALSE(executor.TryProcess());
	ASSERT_FALSE(executor.TryProcess());
	ASSERT_FALSE(executor.TryProcess());

	ASSERT_TRUE(executor.TryPush(task3));
	ASSERT_TRUE(executor.TryProcess());

	ASSERT_TRUE(future1.IsReady());
	ASSERT_TRUE(future2.IsReady());
	ASSERT_TRUE(future3.IsReady());

	auto result1 = future1.Get();
	auto result2 = future2.Get();
	auto result3 = future3.Get();

	//std::jthread worker(processor);
	//std::this_thread::sleep_for(1ms);
	//worker.request_stop();

	////////////////////////////////////////////////////////////////////////////////

	return;
}