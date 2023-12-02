import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Core.Logger;
import madX.Backend.Extensions;
import madX.Backend.Fence;
import madX.Backend.Device;
import madX.Backend.HeapDescriptor;
import madX.Backend.HeapUpload;
import madX.Backend.PoolStatic;
import madX.Backend.PoolDynamic;
import madX.Backend.Commands;
import madX.Backend.Buffer;
import madX.Backend.Texture;
import madX.Backend.Shader;
import madX.Backend.Pipeline;
import madX.Backend.SwapChain;

using namespace Microsoft::WRL;

namespace madX::Backend
{
	class Pipeline;
	class SwapChain;
	struct SwapChainDesc;
	struct PipelineLayoutDesc;

	class Processor final : public Device
	{
		Commands commands_;

		//additional heap for UAVs with GPU shader-visible and CPU non-shader-visible
		HeapDescriptor heaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES + 1];
		
		PoolDynamic dynamic_;
		
		PoolStatic static_;
		HeapUpload upload_;
		
		//Context context_;	

		Frame frame_;

//		//TODO: change to concept with args...
//		//template <typename Object, typename ObjectDesc, typename ObjectCtor>
//		//void CreateObject(const char* name, const ObjectDesc& desc, std::shared_ptr<Object>& object, ObjectCtor construct)
		template <typename Construct>
		auto CreateObject(Construct construct)
		{
			try
			{
				return construct();
			}
		
			//TODO: extend this
			catch (const std::runtime_error& error)
			{
				auto logger = Core::Logger::GetInstance();
				logger->Critical("[Processor ::CreateObject]: " + std::string(error.what()));
			}
		}
	public:
		//TODO: explicit Processor (Adapter& adapter);
		Processor ();

		Processor (const Processor &) = delete;
		Processor & operator=(const Processor &) = delete;

		Processor (Processor &&) = delete;
		Processor & operator=(Processor &&) = delete;

		////////////////////////////////////////////////////////////////////////////////

		std::shared_ptr<Buffer> CreateBuffer(const BufferDesc& desc)
		{
			std::shared_ptr<Buffer> buffer;
		
			CreateObject([&]() { buffer = std::make_shared<Buffer>(this, desc); });
			return buffer;
		}
		
		std::shared_ptr<Texture> CreateTexture(const TextureDesc& desc, const TextureData* data)
		{
			std::shared_ptr<Texture> texture;
		
			CreateObject([&]() { texture = std::make_shared<Texture>(this, desc, data); });
			return texture;
		}
		
		//std::shared_ptr<Texture> CreateTexture(const TextureDesc& desc, ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
		
		std::shared_ptr<Shader> CreateShader(const ShaderDesc& desc)
		{
			std::shared_ptr<Shader> shader;
		
			CreateObject([&]() { shader = std::make_shared<Shader>(this, desc); });
			return shader;
		}
		
		std::shared_ptr<Pipeline> CreatePipeline(const PipelineDesc& desc)
		{
			std::shared_ptr<Pipeline> pipeline;
		
			CreateObject([&]() { pipeline = std::make_shared<Pipeline>(this, desc); });	
			return pipeline;
		}

		std::shared_ptr<SwapChain> CreateSwapChain(const SwapChainDesc& desc)
		{
			std::shared_ptr<SwapChain> swapchain;

			CreateObject([&]() { swapchain = std::make_shared<SwapChain>(this, desc); });
			return swapchain;
		}

		////////////////////////////////////////////////////////////////////////////////

		//	Context& GetContext()
		//	{
		//		return context_;
		//	}

		//	Commands& GetCommands()
		//	{
		//		return commands_;
		//	}

		ID3D12GraphicsCommandList2* GetNewCommandList()
		{
			return commands_.GetNewCommandList();
		}

		ID3D12GraphicsCommandList2* GetCurrentCommandList() noexcept
		{
			return commands_.GetCurrentCommandList();
		}

		auto& GetHeapUpload()
		{
			return upload_;
		}

		////////////////////////////////////////////////////////////////////////////////

		HeapDescriptor::Element AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, std::uint32_t count)
		{
			assert(heap_type >= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && heap_type <= D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
			return heaps_[heap_type].Allocate(count);
		}

		Allocation AllocateDynamicBuffer(std::uint32_t size)
		{
			return dynamic_.Allocate(size);
		}

		Allocation AllocateStaticBuffer(std::uint32_t size)
		{
			return static_.Allocate(size);
		}

		void UploadStaticData(ID3D12GraphicsCommandList* command_list)
		{
			assert(command_list);
			static_.UploadData(command_list);
		}

		void FreeUploadStaticHeap()
		{
			static_.Free();
		}

		////////////////////////////////////////////////////////////////////////////////

		void BeginFrame();

		void EndFrame()
		{
			++frame_;
		}

		Frame GetFrame() const noexcept
		{
			return frame_;
		}
	};
}
