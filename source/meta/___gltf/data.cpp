#include "_meta.hpp"

namespace madX::Meta::glTF
{
	//TODO: move it to workers
	Data GetData(const std::filesystem::path& path, const std::string& uri)
	{
		std::filesystem::path file(path / uri);

		if (false == std::filesystem::exists(file))
		{
			return {};
		}
		//assert(std::filesystem::exists(file));

		auto size = std::filesystem::file_size(file);
		assert(size != 0);

		std::ifstream stream(file, std::ios::in | std::ios::binary);
		assert(stream.is_open());

		Data data(size);
		stream.read(reinterpret_cast<char*>(data.data()), size);

		assert(data.size() == stream.gcount());

		return data;
	}
}
