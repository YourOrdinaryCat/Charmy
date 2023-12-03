module;
#include "pch.h"

export module server;

namespace server {
	export template<typename T>
		struct class_factory : winrt::implements<class_factory<T>, IClassFactory>
	{
		class_factory() = default;

		HRESULT __stdcall CreateInstance(
			IUnknown* pUnkOuter,
			REFIID iid,
			void** ppvObject) noexcept final
		{
			*ppvObject = nullptr;
			if (pUnkOuter)
				return CLASS_E_NOAGGREGATION;

			return winrt::make<T>().as(iid, ppvObject);
		}

		HRESULT __stdcall LockServer(int) noexcept final {
			return E_NOTIMPL;
		}
	};

	std::atomic_uint64_t clients;

	template<auto Start, auto End, auto Inc, class F>
	constexpr void constexpr_for(F&& f) {
		if constexpr (Start < End) {
			f(std::integral_constant<decltype(Start), Start>());
			constexpr_for<Start + Inc, End, Inc>(f);
		}
	}

	template<typename T>
	concept has_co_registration_context = std::is_same_v<decltype(T::CoRegistrationContext()), CLSCTX>;

	template<typename T>
	constexpr CLSCTX co_registration_context() {
		if constexpr (has_co_registration_context<T>)
			return T::CoRegistrationContext();
		return CLSCTX_LOCAL_SERVER;
	}

	template<typename T>
	concept has_co_registration_flags = std::is_same_v<decltype(T::CoRegistrationFlags()), REGCLS>;

	template<typename T>
	constexpr REGCLS co_registration_flags() {
		if constexpr (has_co_registration_flags<T>)
			return T::CoRegistrationFlags();
		return REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED;
	}

	template<int N, typename... Ts>
	using nth_type = typename std::tuple_element<N, std::tuple<Ts...>>::type;

	export template<typename T>
		HRESULT register_class(
			CLSCTX context,
			REGCLS flags,
			DWORD* cookie) noexcept
	{
		return CoRegisterClassObject(
			__uuidof(T),
			winrt::make<class_factory<T>>().get(),
			context,
			flags,
			cookie
		);
	}

	export template<typename... Types>
		std::array<DWORD, sizeof...(Types)> register_classes()
	{
		std::array<DWORD, sizeof...(Types)> cookies{};
		constexpr_for<0, sizeof...(Types), 1>([&cookies](auto i)
			{
				using type = nth_type<i, Types...>;

				DWORD cookie;
				const auto result = register_class<type>(
					co_registration_context<type>(),
					co_registration_flags<type>(),
					&cookie
				);

				winrt::check_hresult(result);
				cookies[i] = cookie;
			}
		);
		return cookies;
	}

	export template<std::size_t N>
		void unregister_classes(const std::array<DWORD, N>& cookies)
	{
		constexpr_for<0, N, 1>([&cookies](auto i)
			{
				const auto result = CoRevokeClassObject(cookies[i]);
				winrt::check_hresult(result);
			}
		);
	}

	export uint64_t client_count() noexcept {
		return clients;
	}

	export uint64_t add_ref() noexcept {
		clients = CoAddRefServerProcess();
		return clients;
	}

	export uint64_t release_ref() noexcept {
		clients = CoReleaseServerProcess();
		return clients;
	}
}
