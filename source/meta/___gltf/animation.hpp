#pragma once

namespace madX::Meta::glTF
{
	struct Animation
	{
		struct Channel
		{
			struct Target
			{
				enum class Path
				{
					Translation,
					Rotation,
					Scale,
					Weights
				};

				std::optional<Index> node_;
				Path path_;

				Extras extras_;
			};

			Index sampler_;
			Target target_;

			Extras extras_;
		};

		struct Sampler
		{
			enum class Interpolation
			{
				Linear,
				Step,
				Cubicspline
			};

			Index input_;
			Interpolation interpolation_ = Interpolation::Linear;
			Index output_;

			Extras extras_;
		};

		std::vector<Channel> channels_;
		std::vector<Sampler> samplers_;

		Name name_;
		Extras extras_;
	};

	using Animations = std::vector<Animation>;
	Animations GetAnimations(const simdjson::dom::element& element);
}
