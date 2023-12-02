#include "_root.hpp"

//https://stackoverflow.com/questions/1222340/aspect-ratios-how-to-go-about-them-d3d-viewport-setup
//http://learnwebgl.brown37.net/08_projections/projections_perspective.html

namespace madX::Root::Component
{
	CameraInfinity::CameraInfinity()
	  : Camera()
	{
		//eye_ = XMVectorSet(0, 5, -1, 1);
		//eye_ = XMVectorSet(1, 1, 1, 1);
		eye_ = XMVectorSet(3, 1, 0, 1);
		target_ = XMVectorSet(0, 0, 0, 1);
		up_ = XMVectorSet(0, 1, 0, 0);
	}

	void CameraInfinity::Update(float width, float height, float delta_time)
	{
		XMVECTOR rotation_delta = XMVectorScale(rotation_delta_, XM_PI / 3000.0f);
		
		////	{
		////		XMMATRIX rotation = XMMatrixRotationY(XMVectorGetY(rotation_delta));
		////	
		////		eye_ = target_ + XMVector3TransformCoord(eye_ - target_, rotation);
		////		up_ = XMVector3TransformNormal(up_, rotation);
		////	}

		{
			XMMATRIX yaw = XMMatrixRotationAxis(up_, XMVectorGetY(rotation_delta));	
			target_ = eye_ + XMVector3TransformNormal(target_ - eye_, yaw);
		}

		{
			XMVECTOR right = XMVector3Cross(-(target_ - eye_), up_);
			XMMATRIX pitch = XMMatrixRotationAxis(right, XMVectorGetX(rotation_delta));

			target_ = eye_ + XMVector3TransformNormal(target_ - eye_, pitch);
			//up_ = XMVector3TransformNormal(up_, rotation);
		}

		////////////////////////////////////////////////////////////////////////////////

		{
			XMVECTOR dir = XMVectorScale(direction_delta_, delta_time);
			XMVECTOR forward = Forward();
			XMVECTOR right = XMVector3Cross(-forward, up_);
			XMVECTOR translation = XMVectorGetX(dir) * right + XMVectorGetY(dir) * up_ + XMVectorGetZ(dir) * forward;
			eye_ += translation;
			target_ += translation;
		}
			
		////////////////////////////////////////////////////////////////////////////////

		view_ = XMMatrixLookAtLH(eye_, target_, up_);

		if (const auto aspect_ratio = width / height; aspect_ratio != aspect_ratio_)
		{
			const float fieldOfView = XMConvertToRadians(fov_);
			//projection_ = XMMatrixPerspectiveFovLH(fieldOfView, aspect_ratio_ = aspect_ratio, near_clip_, far_clip_);
			projection_ = XMMatrixPerspectiveFovLH(fieldOfView, aspect_ratio_ = aspect_ratio, far_clip_, near_clip_);
		}
	}
}

//	void CameraInfinity::Update(float width, float height, float delta_time)
//	{
//		const auto aspect_ratio = width / height;
//	
//		XMVECTOR velocity = XMVector3Normalize(direction_delta_);
//		//velocity = XMVectorScale(velocity, 1.0);
//		XMVECTOR translate = XMVectorScale(velocity, delta_time);
//	
//		XMVECTOR rotate = XMVectorScale(rotation_delta_, XM_PI / 3000.0f);
//		XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(rotate);
//	
//		XMVECTOR worldUp = XMVector3TransformCoord(g_XMIdentityR1, rotation);
//		XMVECTOR worldAhead = XMVector3TransformCoord(g_XMIdentityR2, rotation);
//		XMVECTOR worldTranslate = XMVector3TransformCoord(translate, rotation);
//		eye_ = XMVectorAdd(eye_, worldTranslate);
//		target_ = XMVectorAdd(eye_, worldAhead);
//		view_ = XMMatrixLookAtLH(eye_, target_, worldUp);
//	
//		if (aspect_ratio != aspect_ratio_)
//		{
//			const float fieldOfView = XMConvertToRadians(fov_);
//			projection_ = XMMatrixPerspectiveFovLH(fieldOfView, aspect_ratio_ = aspect_ratio, near_clip_, far_clip_);
//		}
//	}