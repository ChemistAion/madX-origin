import <cassert>;

import stl.core;
import microsoft.windows;
import microsoft.directx;
import madX.Backend.Settings;

using namespace Microsoft::WRL;

import <winerror.h>;
import <combaseapi.h>;

namespace madX::Backend
{
	struct DescriptorHeaps
	{
		ID3D12DescriptorHeap* generic_ = nullptr;
		ID3D12DescriptorHeap* sampler_ = nullptr;

		DescriptorHeaps() = default;

		explicit DescriptorHeaps(ID3D12DescriptorHeap* generic, ID3D12DescriptorHeap* sampler) noexcept
			: generic_{ generic }
			, sampler_{ sampler }
		{
		}

		bool operator==(const DescriptorHeaps& rhs) const
		{
			return generic_ == rhs.generic_ && sampler_ == rhs.sampler_;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	class Processor;

	//TODO: template
	class HeapDescriptor final
	{
		std::uint32_t index_ = 0;
		const std::uint32_t count_ = 0;
		const std::uint32_t size_ = 0;
		const D3D12_DESCRIPTOR_HEAP_FLAGS flags_;
		ComPtr<ID3D12DescriptorHeap> heap_;
	public:
		//TODO: pair <count, type>
		HeapDescriptor
		(
			Processor* processor,
			std::uint32_t count,
			D3D12_DESCRIPTOR_HEAP_TYPE type
		);

		////////////////////////////////////////////////////////////////////////////////

		struct Element
		{
			D3D12_CPU_DESCRIPTOR_HANDLE cpu_ = { };
			D3D12_GPU_DESCRIPTOR_HANDLE gpu_ = { };

			std::uint32_t count_ = 0;
			std::uint32_t size_ = 0;

			ID3D12DescriptorHeap* heap_ = nullptr;

			////////////////////////////////////////////////////////////////////////////////

			bool IsValid() const noexcept
			{
				return heap_ && size_ != 0 && count_ != 0;
			}

			const D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU(std::uint64_t offest) const noexcept
			{
				assert(heap_ != nullptr);
				assert(cpu_.ptr != 0);
				assert(IsValid());

				auto offset = size_ * offest;
				return { cpu_.ptr + offset };
			}

			const D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(std::uint64_t offest) const noexcept
			{
				assert(heap_ != nullptr);
				assert(gpu_.ptr != 0);
				assert(IsValid());

				auto offset = size_ * offest;
				return { gpu_.ptr + offset };
			}

			ID3D12DescriptorHeap* GetDescriptorHeap() noexcept
			{
				return heap_;
			}
		};

		////////////////////////////////////////////////////////////////////////////////

		HeapDescriptor::Element Allocate(std::uint32_t count);
	};
}