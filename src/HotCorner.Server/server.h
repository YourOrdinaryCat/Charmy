#pragma once
#include <Unknwn.h>
#include <winrt/base.h>

namespace winrt::server {
	template<typename T>
	struct class_factory : winrt::implements<class_factory<T>, IClassFactory> {
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
	inline DWORD register_class(
		CLSCTX context = CLSCTX_LOCAL_SERVER,
		REGCLS flags = REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED)
	{
		DWORD cookie{};
		const auto result = CoRegisterClassObject(
			__uuidof(T),
			winrt::make<class_factory<T>>().get(),
			context,
			flags,
			&cookie
		);

		winrt::check_hresult(result);
		return cookie;
	}

	inline void unregister_class(DWORD cookie) {
		winrt::check_hresult(CoRevokeClassObject(cookie));
	}

	inline void resume_class_objects() {
		winrt::check_hresult(CoResumeClassObjects());
	}
}
