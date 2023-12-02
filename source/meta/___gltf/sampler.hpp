#pragma once

namespace madX::Meta::glTF
{
	struct Sampler
	{
		enum class MinFilter
		{
		};

		enum class MagFilter
		{
		};

		enum class WrapMode
		{
		};

		std::optional<MinFilter> min_filter_;
		std::optional<MagFilter> mag_filter_;

		WrapMode wrap_s_;
		WrapMode wrap_t_;

		Name name_;
		Extras extras_;
	};

	using Samplers = std::vector<Sampler>;

	Samplers GetSamplers(const simdjson::dom::element& element);
}