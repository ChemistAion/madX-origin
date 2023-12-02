#include "_editor.hpp"

#include "camera.hpp"

namespace madX::Editor::Frontend
{
	Camera::Camera()
	{
		position_ = Im3d::Vec3(0.0f, 2.0f, 3.0f);
		direction_ = Im3d::Normalize(Im3d::Vec3(0.0f, -0.5f, -1.0f));
		
		fov_deg_ = 50.0f;
		fov_rad_ = Im3d::Radians(fov_deg_);
	}

	void Camera::Update(const float aspect_ratio)
	{
		fov_rad_ = Im3d::Radians(fov_deg_);
		float n = 0.1f;
		float f = 500.0f;
		float a = aspect_ratio; //(float)m_width / (float)m_height;
		float scale = std::tanf(fov_rad_ * 0.5f) * n;
		float viewZ = -1.0f;

		float r = a * scale;
		float l = -r;
		float t = scale;
		float b = -t;

		proj_ = Im3d::Mat4
		(
			2.0f * n / (r - l),
			0.0f,
			-viewZ * (r + l) / (r - l),
			0.0f,
			
			0.0f,
			2.0f * n / (t - b),
			-viewZ * (t + b) / (t - b),
			0.0f,
			
			0.0f,
			0.0f,
			viewZ,
			-n,

			0.0f,
			0.0f,
			viewZ,
			0.0f
		);

		world_ = Im3d::LookAt(position_, position_ + direction_ * viewZ);
		view_ = Im3d::Inverse(world_);
		view_proj_ = proj_ * view_;
	}
}
