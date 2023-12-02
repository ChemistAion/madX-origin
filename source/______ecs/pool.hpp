#pragma once

#include <type_traits>	

namespace madX::ECS
{
    namespace Detail
    {
        template<typename Key, typename Type, typename = std::void_t<>>
        struct Pool
        {
            using PoolType = Core::SparseMap<Key, Type>;
        };

        template<typename Key, typename Type>
        struct Pool<Key, const Type>
        {
            using PoolType = std::add_const_t<typename Pool<Key, std::remove_const_t<Type>>::PoolType>;
        };
    }

    template<typename Key, typename Type>
    using PoolType = typename Detail::Pool<Key, Type>::PoolType;
}