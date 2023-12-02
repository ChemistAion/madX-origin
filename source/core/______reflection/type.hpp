#pragma once

namespace madX::Core
{
	//TODO: change it to Detail
    namespace Internal
    {
        class TypeNameProbe;
        constexpr std::string_view type_probe_("class madX::Core::Internal::TypeNameProbe");
    
        void TypeNameExtract(std::string_view& name) noexcept;
    }
    
    template <typename Type>
    constexpr auto TypeName() noexcept
    {
        std::string_view name(__FUNCSIG__);

        Internal::TypeNameExtract(name);
        return name;
    }
    
    ////////////////////////////////////////////////////////////////////////////////    

	using TypeInfoType = typename std::uint64_t;

    template <typename Type>
    class TypeInfo final
    {
    public:
        [[nodiscard]] static constexpr TypeInfoType Value() noexcept
        {
            constexpr TypeInfoType fnv1a_offset_ = 14695981039346656037ull;
            constexpr TypeInfoType fnv1a_prime_ = 1099511628211ull;
    
			TypeInfoType hash = fnv1a_offset_;
    
            for (const TypeInfoType element : TypeName<Type>())
            {
                hash = (hash ^ element) * fnv1a_prime_;
            }
    
            return hash;
        };
    };

	////////////////////////////////////////////////////////////////////////////////
	
	using TypeIndexType = typename std::size_t;

	template <typename...>
	class TypeIndex final
	{
		//TODO: no atomic here
		inline static std::atomic<TypeIndexType> identity_{};
	public:
		template <typename... Types>
		static const TypeIndexType Value() noexcept
		{
			static_assert(sizeof...(Types) > 0);

			static const TypeIndexType result = identity_++;
			return result;
		}
	};
}