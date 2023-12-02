#pragma once

namespace madX::Core
{
	namespace Detail
	{
		struct NoneSuch
		{
			NoneSuch() = delete;
			~NoneSuch() = delete;

			NoneSuch(NoneSuch const&) = delete;
			void operator=(NoneSuch const&) = delete;
		};
	}
		
	template <class Default, class, template <class...> class Archetype, class... Arguments>
	struct Detector
	{
		using value_t = std::false_type;
		using type = Default;
	};
	
	template <class Default, template <class...> class Archetype, class... Arguments>
	struct Detector<Default, std::void_t<Archetype<Arguments...>>, Archetype, Arguments...>
	{
		using value_t = std::true_type;
		using type = Archetype<Arguments...>;
	};

	template <template <class...> class Archetype, class... Arguments>
	using IsDetected = typename Detector<Detail::NoneSuch, void, Archetype, Arguments...>::value_t;
}