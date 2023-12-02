#include "_benchmarks.hpp"

#include "core_async.hpp"

using namespace Primitives;

////////////////////////////////////////////////////////////////////////////////

BENCHMARK_DEFINE_F(CoreAsync, Simple)(benchmark::State& state)
{
	std::vector<int> values(count);
	std::generate(values.begin(), values.end(), [&]() { return distribution(generator); });

	while (state.KeepRunning())
	{
		std::deque<Future<int>> result;

		for (std::size_t index = 0; index < count; ++index)
		{
			auto func_inc = [](int value)
			{
				return value + 1;
			};

			auto func_dec = [](int value)
			{
				return value - 1;
			};

			auto func_none = [](int value)
			{
				return value;
			};

			auto func_final = [](int value1, int value2, int value3)
			{
				return value1 + value2 + value3;
			};

			auto future1 = scheduler_->AddTask(func_inc, values[index]);
			auto future2 = scheduler_->AddTask(func_dec, values[index]);
			auto future3 = scheduler_->AddTask(func_none, values[index]);

			result.push_back(scheduler_->AddTask(func_final, std::move(future1), std::move(future2), std::move(future3)));
		}

		auto done = result.begin();

		for (;;)
		{
			done = std::partition
			(
				done, result.end(),
				[](auto& result) { return result.IsReady(); }
			);

			if (done == result.end())
			{
				break;
			}	
		}

		continue;
	}
}

BENCHMARK_REGISTER_F(CoreAsync, Simple)->Repetitions(4)->Unit(benchmark::kMicrosecond)->ReportAggregatesOnly(false)->DisplayAggregatesOnly(false);//->Threads(4);

////////////////////////////////////////////////////////////////////////////////

//	BENCHMARK_DEFINE_F(Parallel, ModerateTask_Benchmark)(benchmark::State& state)
//	{
//		while (state.KeepRunning())
//		{
//		}
//	}
//	
//	BENCHMARK_REGISTER_F(Parallel, ModerateTask_Benchmark)->Repetitions(4)->Unit(benchmark::kMicrosecond)->ReportAggregatesOnly(false)->DisplayAggregatesOnly(false);

////////////////////////////////////////////////////////////////////////////////