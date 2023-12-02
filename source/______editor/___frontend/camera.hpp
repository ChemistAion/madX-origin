#pragma once

#include "im3d.h"
#include "im3d_math.h"

namespace madX::Editor::Frontend
{
	class Camera final
	{
		Im3d::Vec3 position_;
		Im3d::Vec3 direction_;
		
		float fov_deg_;
		float fov_rad_;

		Im3d::Mat4 world_;
		Im3d::Mat4 view_;
		Im3d::Mat4 proj_;
		Im3d::Mat4 view_proj_;

	public:
		Camera();
		void Update(const float aspect_ratio);

		////////////////////////////////////////////////////////////////////////////////

		auto& GetPosition() noexcept
		{
			return position_;
		}

		auto& GetDirection() noexcept
		{
			return direction_;
		}

		auto& GetWorld() noexcept
		{
			return world_;
		}

		auto& GetView() noexcept
		{
			return view_;
		}

		const auto& GetProjection() const noexcept
		{
			return proj_;
		}

		const auto& GetViewProjection() const noexcept
		{
			return view_proj_;
		}

		void BuildView() noexcept
		{
			world_ = Im3d::LookAt(position_, position_ - direction_);
			view_ = Im3d::Inverse(world_);
		}
	};
}