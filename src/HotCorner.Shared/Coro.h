#pragma once
#include <async/task_completion_source.h>
#include <memory>
#include <utility>


namespace async {
	template<typename TResult>
	class tcs_task {
		using tcs = async::task_completion_source<TResult>;
		using tcs_ptr = std::shared_ptr<tcs>;

		tcs_ptr m_ptr;

	public:
		inline constexpr tcs_task(tcs_ptr&& ptr) noexcept : m_ptr(ptr) {}

		inline async::task<TResult> operator co_await() const noexcept {
			return m_ptr->task();
		}
	};
}

