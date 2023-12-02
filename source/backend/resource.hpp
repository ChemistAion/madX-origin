import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Core.Align;
import madX.Backend.Extensions;

using namespace Microsoft::WRL;

namespace madX::Backend
{
	enum class ResourceBind
	{
		None,
		VertexBuffer,
		IndexBuffer,
		UniformBuffer,
		ShaderResource,
		RenderTarget,
		RenderTargetUnorderedAccess,
		DepthStencil,
		UnorderedAccess,
		IndirectDraw,
		StreamOutput
	};

	enum class ResourceType
	{
		Undefined,
		Buffer,
		Texture
	};

	//TODO: change it to bool: static/dynamic
	enum class ResourceUsage
	{
		Default,
		Static,
		Dynamic,
		Staging
	};
	
	enum class ResourceView
	{
		Undefined,
		ShaderResource,
		RenderTarget,
		DepthStencil,
		UnorderedAccess,
		UnorderedAccessNSV
	};

	enum class ResourceDimension
	{
		Undefined,
		Buffer,
		Texture1D,
		Texture1D_Array,
		Texture2D,
		Texture2D_Array,
		Texture3D,
		TextureCube,
		TextureCube_Array
	};

	////////////////////////////////////////////////////////////////////////////////

	//TODO: should be based on CD3DX12_RESOURCE_BARRIER
	struct ResourceBarrierDesc
	{
		class Resource* resource_ = nullptr;
		D3D12_RESOURCE_STATES before_ = D3D12_RESOURCE_STATE_INVALID;
		D3D12_RESOURCE_STATES after_ = D3D12_RESOURCE_STATE_INVALID;

		D3D12_RESOURCE_BARRIER_FLAGS transition_ = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		ResourceBarrierDesc() = default;

		ResourceBarrierDesc
		(
			Resource* resource,
			D3D12_RESOURCE_STATES before,
			D3D12_RESOURCE_STATES after
		) noexcept
			: resource_{ resource }
			, before_{ before }
			, after_{ after }
		{
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	class Resource
	{
	private:
		D3D12_RESOURCE_STATES state_ = D3D12_RESOURCE_STATE_INVALID;
	protected:
		ComPtr<ID3D12Resource> resource_;
	public:
		Resource() = default;

		auto GetResource()
		{
			return resource_.Get();
		}

		void SetResourceState(D3D12_RESOURCE_STATES state) noexcept
		{
			assert(state != D3D12_RESOURCE_STATE_INVALID);
			state_ = state;
		}
		
		auto GetResourceState() const noexcept
		{
			return state_;
		}

		bool IsResourceStateValid() const noexcept
		{
			return state_ != D3D12_RESOURCE_STATE_INVALID;
		}

		bool CheckResourceState(D3D12_RESOURCE_STATES state) const
		{
			assert(state == 0 || Core::IsSingleBit(state));
			assert(IsResourceStateValid());

			if (state != D3D12_RESOURCE_STATE_COMMON)
			{
				[[likely]] return (state_ & state) == state;
			}
			else
			{			
				[[unlikely]] return state_ == state;
			}
		}

		bool CheckTransitionState(D3D12_RESOURCE_STATES state) const
		{
			return IsResourceStateValid() && (state == 0 || Core::IsSingleBit(state)) && !CheckResourceState(state);
		}
	};
}