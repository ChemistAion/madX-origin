#pragma once

//#include "panel/panel_base.hpp"

namespace madX::Root::Component
{
	class Camera
	{
	protected:
		XMVECTOR eye_;
		XMVECTOR target_;
		XMVECTOR up_;

		const float fov_ = 50.0f;
		float aspect_ratio_;
		const float near_clip_ = 0.1f;
		const float far_clip_ = 1000.0f;

		XMMATRIX view_;
		XMMATRIX projection_;

	public:
		XMVECTOR direction_delta_ = g_XMZero;
		XMVECTOR rotation_delta_ = g_XMZero;

		Camera() = default;
		virtual ~Camera()
		{
		}

		////////////////////////////////////////////////////////////////////////////////

		XMVECTOR Forward() const noexcept
		{
			return XMVector3Normalize(XMVectorSubtract(target_, eye_));
		}

		const XMFLOAT4X4 GetView() const noexcept
		{
			XMFLOAT4X4 output;
			XMStoreFloat4x4(&output, view_);
			return output;
		}

		const XMMATRIX& GetViewMatrix() const noexcept
		{
			return view_;
		}

		const XMFLOAT4X4 GetProjection() const noexcept
		{
			XMFLOAT4X4 output;
			XMStoreFloat4x4(&output, projection_);
			return output;
		}

		const XMFLOAT4X4 GetInverseProjection() const noexcept
		{
			XMFLOAT4X4 output;
			XMMATRIX inverse = XMMatrixInverse(nullptr, projection_);
			XMStoreFloat4x4(&output, inverse);
			return output;
		}

		const XMMATRIX& GetProjectionMatrix() const noexcept
		{
			return projection_;
		}

		const XMFLOAT4X4 GetViewProjection() const noexcept
		{
			XMMATRIX vp = view_ * projection_;

			XMFLOAT4X4 output;
			XMStoreFloat4x4(&output, vp);
			return output;
		}

		const XMFLOAT4X4 GetInverseViewProjection() const noexcept
		{
			XMFLOAT4X4 output;
			XMMATRIX inverse = XMMatrixInverse(nullptr, view_ * projection_);
			XMStoreFloat4x4(&output, inverse);
			return output;
		}

		////////////////////////////////////////////////////////////////////////////////

		XMFLOAT4 GetPosition() const noexcept
		{
			XMFLOAT4 output;
			XMStoreFloat4(&output, eye_);
			return output;
		}

		XMFLOAT4 GetDirection() const noexcept
		{
			XMFLOAT4 output;
			//XMStoreFloat4(&output, XMVector3Normalize(XMVectorSubtract(eye_, target_)));
			//XMStoreFloat4(&output, target_);
			XMStoreFloat4(&output, Forward());
			return output;
		}

		////////////////////////////////////////////////////////////////////////////////

		//void Update(float width, float height, float delta_time);
	};

	class CameraInfinity final : public Camera
	{
	public:
		CameraInfinity();

		void Update(float width, float height, float delta_time);
	};
}