#pragma once
#include <filesystem>
#include <mutex>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>
#include <string_view>
#include <type_traits>

namespace winrt::HotCorner::Logging {
	enum class lazy_sink_state {
		opened = 0,
		nothing_logged = 1,
		failed = 2
	};

	template<typename Mutex>
	class lazy_file_sink final : public spdlog::sinks::base_sink<Mutex> {
		bool m_openTried = false;
		HANDLE m_fileHandle = INVALID_HANDLE_VALUE;
		std::filesystem::path m_path;

		void open();
		void write(std::string_view thing);

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override;
		void flush_() override;

	public:
		constexpr lazy_file_sink(std::filesystem::path path) noexcept :
			m_path(std::move(path)) { }

		~lazy_file_sink() noexcept {
			if (m_fileHandle != INVALID_HANDLE_VALUE) {
				CloseHandle(m_fileHandle);
			}
		}

		lazy_sink_state state();
		constexpr const std::filesystem::path& file() const noexcept { return m_path; }
	};

	using lazy_file_sink_mt = lazy_file_sink<std::mutex>;
	using lazy_file_sink_st = lazy_file_sink<spdlog::details::null_mutex>;
}