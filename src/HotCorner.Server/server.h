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
	inline void register_class(
		DWORD* cookie,
		CLSCTX context = CLSCTX_LOCAL_SERVER,
		REGCLS flags = REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED)
	{
		const auto result = CoRegisterClassObject(
			__uuidof(T),
			winrt::make<class_factory<T>>().get(),
			context,
			flags,
			cookie
		);

		winrt::check_hresult(result);
	}

	inline void unregister_class(DWORD cookie) {
		const auto result = CoRevokeClassObject(cookie);
		winrt::check_hresult(result);
	}

	inline hresult resume_class_objects() {
		return CoResumeClassObjects();
	}
}
