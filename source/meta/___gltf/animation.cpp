#include "_meta.hpp"

#include "animation.hpp"

namespace madX::Meta::glTF
{
	Animations GetAnimations(const simdjson::dom::element& element)
	{
		Animations animations;

		const auto& items = JSON::Get<simdjson::dom::array>(element);
		animations.reserve(items.size());

		for (const auto item : items)
		{
			const auto object = JSON::Get<simdjson::dom::object>(item);
			auto& animation = animations.emplace_back();

			for (const auto [key, value] : object)
			{
				////////////////////////////////////////////////////////////////////////////////

				if (key == "channels")
				{
					const auto items = JSON::Get<simdjson::dom::array>(value);
					animation.channels_.reserve(items.size());

					for (const auto item : items)
					{
						const auto object = JSON::Get<simdjson::dom::object>(item);
						auto& channel = animation.channels_.emplace_back();

						for (const auto [key, value] : object)
						{
							if (key == "sampler")
							{
								channel.sampler_ = JSON::Get<Index>(value);
								continue;
							}

							if (key == "target")
							{
								const auto object = JSON::Get<simdjson::dom::object>(value);

								for (const auto [key, value] : object)
								{
									if (key == "node")
									{
										channel.target_.node_ = JSON::Get<Index>(value);
										continue;
									}

									if (key == "path")
									{
										auto path = JSON::Get<std::string_view>(value);

										if (path == "translation")
										{
											channel.target_.path_ = Animation::Channel::Target::Path::Translation;
											continue;
										}

										if (path == "rotation")
										{
											channel.target_.path_ = Animation::Channel::Target::Path::Rotation;
											continue;
										}

										if (path == "scale")
										{
											channel.target_.path_ = Animation::Channel::Target::Path::Scale;
											continue;
										}

										if (path == "weights")
										{
											channel.target_.path_ = Animation::Channel::Target::Path::Weights;
											continue;
										}

										assert(false);
									}

									if (key == "extensions")
									{
										continue;
									}

									if (key == "extras")
									{
										channel.target_.extras_ = std::move(GetExtras(value));
										continue;
									}

									assert(false);
								}

								continue;
							}

							////////////////////////////////////////////////////////////////////////////////

							if (key == "extensions")
							{
								continue;
							}

							if (key == "extras")
							{
								channel.extras_ = std::move(GetExtras(value));
								continue;
							}

							assert(false);
						}
					}

					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "samplers")
				{
					const auto items = JSON::Get<simdjson::dom::array>(value);
					animation.samplers_.reserve(items.size());

					for (const auto item : items)
					{
						const auto object = JSON::Get<simdjson::dom::object>(item);
						auto& sampler = animation.samplers_.emplace_back();

						for (const auto [key, value] : object)
						{
							if (key == "input")
							{
								sampler.input_ = JSON::Get<Index>(value);
								continue;
							}

							if (key == "interpolation")
							{
								auto interpolation = JSON::Get<std::string_view>(value);

								if (interpolation == "LINEAR")
								{
									sampler.interpolation_ = Animation::Sampler::Interpolation::Linear;
									continue;
								}

								if (interpolation == "STEP")
								{
									sampler.interpolation_ = Animation::Sampler::Interpolation::Step;
									continue;
								}

								if (interpolation == "CUBICSPLINE")
								{
									sampler.interpolation_ = Animation::Sampler::Interpolation::Cubicspline;
									continue;
								}

								assert(false);
							}

							if (key == "output")
							{
								sampler.output_ = JSON::Get<Index>(value);
								continue;
							}

							if (key == "extensions")
							{
								continue;
							}

							if (key == "extras")
							{
								sampler.extras_ = std::move(GetExtras(value));
								continue;
							}

							assert(false);
						}
					}

					continue;
				}

				////////////////////////////////////////////////////////////////////////////////

				if (key == "name")
				{
					animation.name_ = JSON::Get<std::string_view>(value);
					continue;
				}

				if (key == "extensions")
				{
					continue;
				}

				if (key == "extras")
				{
					animation.extras_ = std::move(GetExtras(value));
					continue;
				}

				assert(false);
			}
		}

		return animations;
	}
}

