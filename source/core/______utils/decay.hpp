#pragma once

namespace madX::Core
{
	namespace Detail
	{
		template <class Type>
		struct DecayReferenceWrapper
		{
			using type = Type;
		};

		template <class Type>
		struct DecayReferenceWrapper<std::reference_wrapper<Type>>
		{
			using type = Type;
		};
	}

	template <class Type>
	using DecayReferenceWrapperType = typename Detail::DecayReferenceWrapper<Type>::type;

	template <class Type>
	using DecayReferenceType = DecayReferenceWrapperType<std::remove_reference_t<Type>>;

	template <class Type>
	using DecayType = DecayReferenceType<std::remove_cv_t<Type>>;
}