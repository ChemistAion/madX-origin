import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Backend.Settings;
import madX.Backend.Exception;
import madX.Backend.Fence;

using namespace Microsoft::WRL;

namespace madX::Backend
{
	class Window;
	class Processor;
	class Context;

	struct SwapChainDesc final
	{
		std::uint32_t width_ = 0;
		std::uint32_t height_ = 0;

		DXGI_FORMAT format_ = DXGI_FORMAT_R8G8B8A8_UNORM;

		//float depth_ = 1.0f;
		//std::uint8_t stencil_ = 0;
		
		HWND window_ = NULL;
		//std::shared_ptr<Core::Window> window_;

		std::string name_;
	};

	class SwapChain final
	{
		ComPtr<IDXGISwapChain4> swapchain_;
		ComPtr<IDXGIFactory6> factory_;
		ComPtr<ID3D12DescriptorHeap> heap_;

		Processor* processor_ = nullptr;

		Fence fence_;

		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, cEngine_BackBufferCount> bb_view_;
		std::array<ComPtr<ID3D12Resource>, cEngine_BackBufferCount> bb_resource_;
		HWND window_ = NULL;

		void Create(ID3D12CommandQueue*, std::uint32_t, std::uint32_t);
		void InitializeResources();

		//STATE
		//std::uint32_t width_ = 0;
		//std::uint32_t height_ = 0;
		DXGI_FORMAT format_ = DXGI_FORMAT_R8G8B8A8_UNORM;

		//std::string name_;
	public:
		explicit SwapChain(Processor* processor, const SwapChainDesc& desc);

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		SwapChain(SwapChain&&) = default;
		SwapChain& operator=(SwapChain&&) = default;

		////////////////////////////////////////////////////////////////////////////////

		void Resize(std::uint32_t width, std::uint32_t height);

		const UINT GetBackBufferIndex() const noexcept
		{
			assert(swapchain_);
			return swapchain_->GetCurrentBackBufferIndex();
		}

		ID3D12Resource* GetBackBufferResource()
		{
			const auto index = GetBackBufferIndex();
			return bb_resource_[index].Get();

		//	ComPtr<ID3D12Resource> resource;
		//	
		//	auto index = GetBackBufferIndex();
		//	
		//	auto result = swapchain_->GetBuffer(index, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));
		//	ThrowIfFailed(result, "[SwapChain::GetBackBufferResource] GetBuffer failed");
		//	
		//	return resource.Get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE* GetBackBufferHandle()
		{
			const auto index = GetBackBufferIndex();
			return &bb_view_[index];
		}

		void Present(Frame frame);
		void Wait(Frame frame);

		//	auto GetWidth() const noexcept
		//	{
		//		return width_;
		//	}
		//	
		//	auto GetHeight() const noexcept
		//	{
		//		return height_;
		//	}
	};
}
