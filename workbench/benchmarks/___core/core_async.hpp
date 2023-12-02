#pragma once

#include "core/_core.hpp"

using namespace Core;

class CoreAsync : public Template
{
protected:
	Scheduler* scheduler_;

	void SetUp(const ::benchmark::State&)
	{
		scheduler_ = new (std::nothrow) Scheduler;
	}

	void TearDown(const ::benchmark::State&)
	{
		delete (scheduler_);
	}
};

namespace Primitives
{
	constexpr std::size_t count = 1024;

	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_int_distribution<> distribution(10, 20);
}