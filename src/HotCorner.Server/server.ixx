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

	template<std::size_t Index, typename... Types>
	void register_classes_impl(std::array<DWORD, sizeof...(Types)>& cookies) {
		if constexpr (Index < sizeof...(Types)) {
			using Type = nth_type<Index, Types...>;

			DWORD cookie{};
			const auto result = register_class<Type>(
				co_registration_context<Type>(),
				co_registration_flags<Type>(),
				&cookie
			);

			winrt::check_hresult(result);
			cookies[Index] = cookie;

			register_classes_impl<Index + 1, Types...>(cookies);
		}
	}

	export template<typename... Types>
		std::array<DWORD, sizeof...(Types)> register_classes()
	{
		std::array<DWORD, sizeof...(Types)> cookies{};
		register_classes_impl<0, Types...>(cookies);
		return cookies;
	}

	template<std::size_t Index, std::size_t CookieCount>
	void unregister_classes_impl(const std::array<DWORD, CookieCount>& cookies) {
		if constexpr (Index < CookieCount) {
			const auto result = CoRevokeClassObject(cookies[Index]);
			winrt::check_hresult(result);

			unregister_classes_impl<Index + 1>(cookies);
		}
	}

	export template<std::size_t CookieCount>
		void unregister_classes(const std::array<DWORD, CookieCount>& cookies)
	{
		unregister_classes_impl<0>(cookies);
	}
}
