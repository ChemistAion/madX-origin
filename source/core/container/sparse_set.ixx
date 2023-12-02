export module madX.Core.Container.SparseSet;

import stl.core;

export namespace madX::Core
{
    template <typename Key>
    class SparseSet
    {
        std::vector<Key> packed_;
        std::vector<std::size_t> sparse_;
    public:
        using Iterator = typename std::vector<Key>::iterator;
        using ConstIterator = typename std::vector<Key>::const_iterator;

        Iterator begin() noexcept
        {
            return packed_.begin();
        }

        Iterator end() noexcept
        {
            return packed_.end();
        }

        ConstIterator begin() const noexcept
        {
            return packed_.begin();
        }

        ConstIterator end() const noexcept
        {
            return packed_.end();
        }

        ConstIterator cbegin() const noexcept
        {
            return packed_.cbegin();
        }

        ConstIterator cend() const noexcept
        {
            return packed_.cend();
        }

        ////////////////////////////////////////////////////////////////////////////////

        SparseSet() = default;

        SparseSet(const SparseSet&) = default;
        SparseSet& operator=(const SparseSet&) = default;

        SparseSet(SparseSet&&) noexcept = default;
        SparseSet& operator=(SparseSet&&) noexcept = default;

        virtual ~SparseSet() = default;

        ////////////////////////////////////////////////////////////////////////////////

        [[nodiscard]] constexpr std::size_t Size() const noexcept
        {
            return packed_.size();
        }

        constexpr void Reserve(const std::size_t capacity)
        {
            packed_.reserve(capacity);
        }

        [[nodiscard]] constexpr std::size_t Capacity() const noexcept
        {
            return packed_.capacity();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return packed_.empty();
        }

		//[[nodiscard]] const Type* Data() const noexcept
		//{
		//    return packed_.data();
		//}

        constexpr void Clear() noexcept
        {
            packed_.clear();
        }

        ////////////////////////////////////////////////////////////////////////////////

        [[nodiscard]] constexpr std::size_t Index(const Key key) const
        {
            assert(Contains(key));
			return sparse_[key];
        }

        [[nodiscard]] constexpr bool Contains(const Key key) const noexcept
        {
			return
                key < sparse_.size() &&
                sparse_[key] < packed_.size() &&
                packed_[sparse_[key]] == key;
        }

		//TODO: sort???
        [[nodiscard]] ConstIterator Find(const Key key) const noexcept
        {
            return Contains(key)
                ? begin() + sparse_[key]
                : end();
        }

        void Emplace(const Key key)
        {
            assert(!Contains(key));

            if (key >= sparse_.size())
            {
                //TODO: check max size vs number of types
				//TODO: uniform <key>, next/++key etc.
                sparse_.resize(std::max(sparse_.size() * 2u, std::size_t(key + 1u)));
            }

            packed_.push_back(key);
            sparse_[key] = packed_.size() - 1u;
        }

        void Erase(const Key key)
        {
            assert(Contains(key));

            const auto packed_index = sparse_[key];

            if (packed_index != (packed_.size() - 1u))
            {
                auto element = std::move(packed_.back());
                packed_[packed_index] = std::move(element);

                sparse_[packed_[packed_index]] = packed_index;
            }

            packed_.pop_back();
        }
	
		const auto GetOrder() const noexcept
		{
			std::vector<std::size_t> order(packed_.size());
			std::iota(order.begin(), order.end(), 0);
			std::sort(order.begin(), order.end(), [&](const std::size_t a, const std::size_t b) { return packed_[a] < packed_[b]; });
			
			return order;
		}
    };
}

////////////////////////////////////////////////////////////////////////////////

//		void Sort()
//		{
//			std::vector<std::size_t> order(packed_.size());
//			std::iota(order.begin(), order.end(), 0);
//			std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) { return packed_[a] < packed_[b]; });
//
//			decltype(packed_) packed(packed_.size());
//			decltype(sparse_) sparse(sparse_.size());
//
//			for (std::size_t index = 0; const auto from : order)
//			{
//				sparse[packed_[from]] = index;
//				packed[index] = packed_[from];
//				++index;
//			}
//			//std::for_each(order.begin(), order.end(), [this](const std::size_t index) {})
//
//			packed_ = std::move(packed);
//			sparse_ = std::move(sparse);
//
//			return;
//		}