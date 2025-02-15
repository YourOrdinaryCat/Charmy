#pragma once
#include <Unknwn.h>
#include <winrt/base.h>

namespace winrt::server {
	template<typename TClass>
	struct class_factory : winrt::implements<class_factory<TClass>, IClassFactory> {
		constexpr class_factory() noexcept = default;

		HRESULT __stdcall CreateInstance(
			IUnknown* pUnkOuter,
			REFIID iid,
			void** ppvObject) noexcept final
		{
			*ppvObject = nullptr;
			if (pUnkOuter)
				return CLASS_E_NOAGGREGATION;

			return winrt::make_self<TClass>().as(iid, ppvObject);
		}

		HRESULT __stdcall LockServer(int) noexcept final {
			return E_NOTIMPL;
		}
	};

	template<typename TClass, typename TContext>
	class class_factory_with_context : public winrt::implements<
		class_factory_with_context<TClass, TContext>, IClassFactory>
	{
		TContext m_context;

	public:
		constexpr class_factory_with_context(TContext context) noexcept :
			m_context(context)
		{
		}

		HRESULT __stdcall CreateInstance(
			IUnknown* pUnkOuter,
			REFIID iid,
			void** ppvObject) noexcept final
		{
			*ppvObject = nullptr;
			if (pUnkOuter)
				return CLASS_E_NOAGGREGATION;

			return winrt::make_self<TClass>(m_context).as(iid, ppvObject);
		}

		HRESULT __stdcall LockServer(int) noexcept final {
			return E_NOTIMPL;
		}
	};

	template<typename TClass>
	inline DWORD register_class(
		CLSCTX context = CLSCTX_LOCAL_SERVER,
		REGCLS flags = REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED)
	{
		DWORD cookie{};
		const auto result = CoRegisterClassObject(
			__uuidof(TClass),
			winrt::make<class_factory<TClass>>().get(),
			context,
			flags,
			&cookie
		);

		winrt::check_hresult(result);
		return cookie;
	}

	template<typename TClass, typename TContext>
	inline DWORD register_class(
		TContext arg,
		CLSCTX context = CLSCTX_LOCAL_SERVER,
		REGCLS flags = REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED)
	{
		constexpr guid iid{ winrt::guid_of<TClass>() };
		DWORD cookie{};

		winrt::check_hresult(CoRegisterClassObject(
			iid,
			winrt::make<class_factory_with_context<TClass, TContext>>(arg).get(),
			context,
			flags,
			&cookie
		));
		return cookie;
	}

	inline void unregister_class(DWORD cookie) {
		winrt::check_hresult(CoRevokeClassObject(cookie));
	}

	inline void resume_class_objects() {
		winrt::check_hresult(CoResumeClassObjects());
	}
}
