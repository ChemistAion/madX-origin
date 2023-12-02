#pragma once

namespace madX::Meta::glTF
{
	struct Camera
	{
		struct Perspective
		{
			Float aspect_ratio_;
			Float fov_;
			Float z_far_;
			Float z_near_;

			Extras extras_;
		};

		struct Orthographic
		{
			Float x_mag_;
			Float y_mag_;
			Float z_far_;
			Float z_near_;

			Extras extras_;
		};

		std::variant<Orthographic, Perspective> projection_;

		Name name_;
		Extras extras_;
	};

	using Cameras = std::vector<Camera>;
	Cameras GetCameras(const simdjson::dom::element& element);
}