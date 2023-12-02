export module madX.Core.Container.AlignedPtr;

import <cassert>;

import stl.core;
import stl.memory;

export namespace madX::Core
{
	namespace Detail
	{
		struct AlignedPtrDeleter
		{
			void operator()(void* p)
			{
				//_mm_free(p);
			}
		};
	}

	template <typename Type, std::size_t Align = 16>
	class AlignedPtr;

	template <typename Type, std::size_t Align>
	class AlignedPtr : public std::unique_ptr<Type, Detail::AlignedPtrDeleter>
	{
		std::size_t size_ = 0;

	public:
		AlignedPtr() = default;

		explicit AlignedPtr(std::size_t size)
		{
			Allocate(size);
		}

		void Allocate(std::size_t size)
		{
			assert(size_ == 0);

			this->reset(reinterpret_cast<Type*>(_mm_malloc(size * sizeof(Type), Align)));
			size_ = static_cast<std::size_t>(size * sizeof(Type));
		};

		uint32_t Size() const noexcept
		{
			return size_;
		}
	};

	template <typename Type, std::size_t Align>
	class AlignedPtr<Type[], Align> : public std::unique_ptr<Type[], Detail::AlignedPtrDeleter>
	{
		std::size_t size_ = 0;

	public:
		AlignedPtr() = default;

		explicit AlignedPtr(std::size_t size)
		{
			Allocate(size);
		}

		AlignedPtr(const AlignedPtr&) = delete;
		AlignedPtr& operator=(const AlignedPtr&) = delete;

		AlignedPtr(AlignedPtr&& other) noexcept = default;
		AlignedPtr& operator=(AlignedPtr&& other) noexcept = default;

		void Allocate(std::size_t size)
		{
			assert(size_ == 0);

			this->reset(reinterpret_cast<Type*>(_mm_malloc(size * sizeof(Type), Align)));
			size_ = static_cast<std::size_t>(size * sizeof(Type));
		};

		auto Size() const noexcept
		{
			return size_;
		}
	};
}