#pragma once

namespace madX::Root
{
	class Processor
	{
	public:
		Processor() = default;

		void ProcessNodes(const Meta::State&);
	};
}