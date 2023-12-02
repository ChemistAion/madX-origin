import stl.core;
import madX.Core.Container.Graph;

namespace madX::Framegraph
{
	enum class StateType
	{
		Default = 0,
		Imported = 1 << 0,
		Output = 1 << 1
	};

	class Resource
	{
		const std::string name_;

		Core::GraphNodeID pass_first_ = std::numeric_limits<Core::GraphNodeID>().max();
		Core::GraphNodeID pass_last_ = 0;


	public:
		Resource(const std::string name)
			: name_(name)
		{
		}

		////////////////////////////////////////////////////////////////////////////////

		auto GetName() const noexcept
		{
			return name_.c_str();
		}


	};
}