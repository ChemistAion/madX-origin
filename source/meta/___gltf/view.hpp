#pragma once

namespace madX::Meta::glTF
{
	struct View
	{
		enum class Target : Type
		{
			ArrayBuffer = 34962,
			ElementArrayBuffer = 34963
		};

		Index buffer_;
		Offset offset_ = 0;
		Size size_ = 0;
		Size stride_ = 0; //TODO: optional
		std::optional<Target> target_;

		Name name_;
		Extras extras_;
	};

	using Views = std::vector<View>;
	Views GetViews(const simdjson::dom::element& element);
}