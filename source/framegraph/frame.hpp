import stl.core;
//import madX.Core.Container.Graph;
import madX.Framegraph.Pass;

namespace madX::Framegraph
{
	class Frame
	{
	private:
		std::vector<std::unique_ptr<PassBase>> passes_;
	public:
		Frame();

		//TODO: setup here
		template <typename PassData, typename... Arguments> requires std::is_constructible_v<Pass<PassData>, Arguments...>
		[[maybe_unused]] decltype(auto) AddPass(Arguments&&... arguments)
		{
			const auto& pass = passes_.emplace_back(std::make_unique<Pass<PassData>>(std::forward<Arguments>(arguments)...));
			return (Pass<PassData>*)pass.get();
		}
	};
}