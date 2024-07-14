#include "pch.h"
#include "lazy_file_sink.h"
#include "Logging.h"
#include <fileapi.h>

namespace winrt::HotCorner::Logging {
	template<typename Mutex>
	lazy_sink_state lazy_file_sink<Mutex>::state() {
		std::scoped_lock guard(this->mutex_);

		if (m_openTried) {
			return m_fileHandle
				? lazy_sink_state::opened
				: lazy_sink_state::failed;
		}
		else {
			return lazy_sink_state::nothing_logged;
		}
	}

	template<typename Mutex>
	void lazy_file_sink<Mutex>::sink_it_(const spdlog::details::log_msg& msg) {
		open();

		if (m_fileHandle != INVALID_HANDLE_VALUE) {
			spdlog::memory_buf_t formatted;
			this->formatter_->format(msg, formatted);

			write(formatted);
		}
	}

	template<typename Mutex>
	void lazy_file_sink<Mutex>::flush_() {
		if (m_fileHandle != INVALID_HANDLE_VALUE) {
			if (!FlushFileBuffers(m_fileHandle)) {
				SPDLOG_LAST_ERROR(spdlog::level::trace, "Failed to flush log file");
			}
		}
	}

	template<typename Mutex>
	void lazy_file_sink<Mutex>::open() {
		if (!std::exchange(m_openTried, true)) {
			CREATEFILE2_EXTENDED_PARAMETERS params{
				.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS),
				.dwFileAttributes = FILE_ATTRIBUTE_NORMAL,
				.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN
			};

			m_fileHandle = CreateFile2(
				m_path.c_str(),
				GENERIC_WRITE,
				FILE_SHARE_READ,
				CREATE_ALWAYS,
				&params
			);

			if (m_fileHandle != INVALID_HANDLE_VALUE) {
				write("\xEF\xBB\xBF");
			}
			else {
				SPDLOG_LAST_ERROR(spdlog::level::err, "Failed to create log file. Logs won't be available during this session.");
			}
		}
	}

	template<typename Mutex>
	void lazy_file_sink<Mutex>::write(std::string_view thing) {
		DWORD bytesWritten;
		if (WriteFile(m_fileHandle, thing.data(), static_cast<DWORD>(thing.size()), &bytesWritten, nullptr)) {
			if (bytesWritten != thing.size()) [[unlikely]] {
				spdlog::trace("Wrote less characters than there is in log entry?");
			}
		}
		else {
			SPDLOG_LAST_ERROR(spdlog::level::trace, "Failed to write log entry to file");
		}
	}

	// We don't actually use the singlethreaded sink.
	template class lazy_file_sink<std::mutex>;
}