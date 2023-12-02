#include "swapchain.hpp"

import madX.Backend.Processor;

//https://devblogs.microsoft.com/directx/dxgi-flip-model/
//https://bell0bytes.eu/fullscreen/
namespace madX::Backend
{
	void SwapChain::Create(ID3D12CommandQueue* command_queue, std::uint32_t width, std::uint32_t height)
	{
		assert(command_queue != nullptr);
		assert(window_);

		DXGI_SWAP_CHAIN_DESC1 desc = {};

		//	if (width_ == 0 || height_ == 0)
		//	{
		//		//width_ = window_->GetClientWidth();
		//		//height_ = window_->GetClientHeight();
		//	}
		//	
		//	
		//	desc.Width = width_; //(UINT) width_;
		//	desc.Height = height_; //(UINT) height_;

		assert(format_ != DXGI_FORMAT_UNKNOWN);

		//TODO: color/gamma scheme...
		switch (format_)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			assert(false);
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			assert(false);
			desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			break;
		default:
			desc.Format = format_;
		}

		desc.Stereo = FALSE;

		//TODO: constexpr desc.SampleDesc
		desc.Width = width;
		desc.Height = height;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = cEngine_BackBufferCount;
		//desc.Scaling = DXGI_SCALING_STRETCH;
		desc.Scaling = DXGI_SCALING_NONE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = 0; //DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

		////////////////////////////////////////////////////////////////////////////////

		ComPtr<IDXGISwapChain1> preliminary;	
			
		auto result = CreateDXGIFactory1(IID_PPV_ARGS(factory_.ReleaseAndGetAddressOf()));
		ThrowIfFailed(result, "[SwapChain::CreateSwapChain] Create IDXGIFactory6 failed");
			
		result = factory_->CreateSwapChainForHwnd
		(
			command_queue,
			window_,
			&desc,
			nullptr,
			nullptr,
			preliminary.ReleaseAndGetAddressOf()
		);
		
		ThrowIfFailed(result, "[SwapChain::CreateSwapChain] Create IDXGISwapChain1 failed");

		result = factory_->MakeWindowAssociation(window_, DXGI_MWA_NO_ALT_ENTER);
		ThrowIfFailed(result, "[SwapChain::CreateSwapChain] IDXGIFactory::MakeWindowAssociation failed");
		
		result = preliminary.As(&swapchain_);
		ThrowIfFailed(result, "[SwapChain::CreateSwapChain] Upgrade from IDXGISwapChain1 to IDXGISwapChain4 failed");

		//TODO: SetName

		////////////////////////////////////////////////////////////////////////////////

		D3D12_DESCRIPTOR_HEAP_DESC heap_desc;
		heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heap_desc.NumDescriptors = cEngine_BackBufferCount;
		heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heap_desc.NodeMask = processor_->GetNode();

		auto device = processor_->GetDevice();
		result = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(heap_.ReleaseAndGetAddressOf()));
		ThrowIfFailed(result, "[SwapChain::CreateSwapChain] CreateDescriptorHeap failed");
	}

	void SwapChain::InitializeResources()
	{
		//TODO: improve me
		assert(heap_);

		auto device = processor_->GetDevice();
		const auto size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (std::uint32_t index = 0; index < cEngine_BackBufferCount; ++index)
		{
			//ComPtr<ID3D12Resource> resource;

			auto& view = bb_view_[index];

			view = heap_->GetCPUDescriptorHandleForHeapStart();
			view.ptr += (SIZE_T)(size * index);

			auto result = swapchain_->GetBuffer(index, IID_PPV_ARGS(bb_resource_[index].ReleaseAndGetAddressOf()));
			ThrowIfFailed(result, "[SwapChain::InitializeSwapChain] GetBuffer failed");

			//D3D12_RESOURCE_DESC desc = resource->GetDesc();

			D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
			rtv_desc.Format = format_;
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtv_desc.Texture2D.MipSlice = 0;
			rtv_desc.Texture2D.PlaneSlice = 0;

			device->CreateRenderTargetView(bb_resource_[index].Get(), nullptr, view);
		}
	}

	SwapChain::SwapChain
	(
		Processor* processor,
		const SwapChainDesc& desc
	)
		: window_{ desc.window_ }
		, fence_{ processor }
	    , processor_{ processor }
	{
		auto logger = Core::Logger::GetInstance();
		logger->Info("Creating SwapChain: " + desc.name_);

		Create(processor->GetGraphicsQueue(), desc.width_, desc.height_);
		InitializeResources();

		//TODO: viewports!!!
	}

	void SwapChain::Resize(std::uint32_t width, std::uint32_t height)
	{
		{
			DXGI_SWAP_CHAIN_DESC desc;
			auto result = swapchain_->GetDesc(&desc);
			ThrowIfFailed(result, "[SwapChain::Resize] SwapChain::GetDesc failed");

			if (desc.BufferDesc.Width == width && desc.BufferDesc.Height == height)
			{
				return;
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		auto command_queue = processor_->GetGraphicsQueue();
		fence_.WaitGPU(command_queue);

		for (std::uint32_t index = 0; index < cEngine_BackBufferCount; ++index)
		{
			bb_resource_[index].Reset();
		}

		auto result = swapchain_->ResizeBuffers(cEngine_BackBufferCount, width, height, format_, 0);
		ThrowIfFailed(result, "[SwapChain::Resize] SwapChain::ResizeBuffers failed");

		InitializeResources();
	}

	void SwapChain::Present(Frame frame)
	{
		auto result = swapchain_->Present(0, 0);
		ThrowIfFailed(result, "[SwapChain::Present] Present failed");

		fence_.Signal(processor_->GetGraphicsQueue(), frame);
	}

	void SwapChain::Wait(Frame frame)
	{
		while (false == fence_.WaitCPU(cEngine_BackBufferCount - 1))
		{
			std::this_thread::yield();
		}
	}
}