export module madX.Core.Container.Ring;

import <cassert>;

import stl.core;

export namespace madX::Core
{
	//TODO: concept/template...
	//TODO: is numeric

	template <typename Type>
	class Ring
	{
		Type total_ {}; //TODO: as a embedded value

		Type head_ {};
		Type size_ {};
	public:
		Ring() = default;

		Ring(Type total)
			: total_ { total }
		{
		}

		[[nodiscard]] auto GetHead() const noexcept
		{
			return head_;
		}

		[[nodiscard]] auto GetSize() const noexcept
		{
			return size_;
		}

		[[nodiscard]] auto GetTail() const noexcept
		{
			return (head_ + size_) % total_;
		}

		////////////////////////////////////////////////////////////////////////////////

		//TODO: check it
		Type Padding(Type size) const noexcept
		{
			if (auto tail = GetTail(); (tail + size) > total_) [[unlikely]]
			{			
				return total_ - tail;
			}
					
			return {};
		}

		//TODO: check it
		bool Allocate(Type size, Type& offset)
		{
			if (size_ + size <= total_) [[likely]]
			{
				offset = GetTail();

				size_ += size;
				return true;
			}
			
			//TODO: throw...
			assert(false);
			return false;		
		}

		bool Free(Type size) noexcept
		{
			if (size_ >= size) [[likely]]
			{				
				head_ = (head_ + size) % total_;
				size_ -= size;

				return true;
			}

			assert(false);
			return false;
		}
	};
}