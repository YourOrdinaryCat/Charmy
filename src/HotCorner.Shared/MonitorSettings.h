#pragma once
#include "CornerAction.h"
#include "JsonHelper.h"
#include <string_view>

namespace winrt::HotCorner::Settings {
	struct MonitorSettings final {
		constexpr MonitorSettings() noexcept = default;
		constexpr MonitorSettings(std::wstring_view id, std::wstring_view name) noexcept :
			Id(id),
			DisplayName(name)
		{ }

		constexpr MonitorSettings(std::wstring_view id, std::wstring_view name, bool enabled, CornerAction tla, CornerAction tra, CornerAction bla, CornerAction bra, uint32_t tld, uint32_t trd, uint32_t bld, uint32_t brd) noexcept :
			Id(id),
			DisplayName(name),
			Enabled(enabled),
			TopLeftAction(tla),
			TopRightAction(tra),
			BottomLeftAction(bla),
			BottomRightAction(bra),
			TopLeftDelay(tld),
			TopRightDelay(trd),
			BottomLeftDelay(bld),
			BottomRightDelay(brd)
		{ }

		std::wstring Id{};
		std::wstring DisplayName{};

		bool Enabled = true;

		CornerAction TopLeftAction = CornerAction::TaskView;
		CornerAction TopRightAction = CornerAction::None;
		CornerAction BottomLeftAction = CornerAction::None;
		CornerAction BottomRightAction = CornerAction::None;

		uint32_t TopLeftDelay = 0;
		uint32_t TopRightDelay = 0;
		uint32_t BottomLeftDelay = 0;
		uint32_t BottomRightDelay = 0;

		inline void SetDelay(uint32_t delay) noexcept {
			TopLeftDelay = delay;
			TopRightDelay = delay;
			BottomLeftDelay = delay;
			BottomRightDelay = delay;
		}

		template<typename Writer>
		inline void Serialize(Writer& writer) const {
			Json::KeyValuePair(writer, IdKey, Id);
			Json::KeyValuePair(writer, DisplayNameKey, DisplayName);
			Json::KeyValuePair(writer, EnabledKey, Enabled);

			Json::MappedKVP(writer, TopLeftActionKey, CornerActionMapping, TopLeftAction);
			Json::MappedKVP(writer, TopRightActionKey, CornerActionMapping, TopRightAction);
			Json::MappedKVP(writer, BottomLeftActionKey, CornerActionMapping, BottomLeftAction);
			Json::MappedKVP(writer, BottomRightActionKey, CornerActionMapping, BottomRightAction);

			Json::KeyValuePair(writer, TopLeftDelayKey, TopLeftDelay);
			Json::KeyValuePair(writer, TopRightDelayKey, TopRightDelay);
			Json::KeyValuePair(writer, BottomLeftDelayKey, BottomLeftDelay);
			Json::KeyValuePair(writer, BottomRightDelayKey, BottomRightDelay);
		}

		inline void Deserialize(const Json::value_t& obj) {
			Json::AssertMatch(Json::type::kObjectType, obj.GetType());
			for (auto member = obj.MemberBegin(); member != obj.MemberEnd(); ++member) {
				const auto key = Json::GetStringView(member->name);

				if (key == IdKey) {
					Id = Json::GetStringView(member->value);
				}
				else if (key == DisplayNameKey) {
					DisplayName = Json::GetStringView(member->value);
				}
				else if (key == EnabledKey) {
					Json::ReadValue(member->value, Enabled);
				}
				else if (key == TopLeftActionKey) {
					Json::ReadMappedValue(member->value, CornerActionMapping, TopLeftAction);
				}
				else if (key == TopRightActionKey) {
					Json::ReadMappedValue(member->value, CornerActionMapping, TopRightAction);
				}
				else if (key == BottomLeftActionKey) {
					Json::ReadMappedValue(member->value, CornerActionMapping, BottomLeftAction);
				}
				else if (key == BottomRightActionKey) {
					Json::ReadMappedValue(member->value, CornerActionMapping, BottomRightAction);
				}
				else if (key == TopLeftDelayKey) {
					Json::ReadValue(member->value, TopLeftDelay);
				}
				else if (key == TopRightDelayKey) {
					Json::ReadValue(member->value, TopRightDelay);
				}
				else if (key == BottomLeftDelayKey) {
					Json::ReadValue(member->value, BottomLeftDelay);
				}
				else if (key == BottomRightDelayKey) {
					Json::ReadValue(member->value, BottomRightDelay);
				}
			}
		}

	private:
		static constexpr std::wstring_view IdKey = L"id";
		static constexpr std::wstring_view DisplayNameKey = L"display_name";
		static constexpr std::wstring_view EnabledKey = L"enabled";

		static constexpr std::wstring_view TopLeftActionKey = L"top_left_action";
		static constexpr std::wstring_view TopRightActionKey = L"top_right_action";
		static constexpr std::wstring_view BottomLeftActionKey = L"bottom_left_action";
		static constexpr std::wstring_view BottomRightActionKey = L"bottom_right_action";

		static constexpr std::wstring_view TopLeftDelayKey = L"top_left_delay";
		static constexpr std::wstring_view TopRightDelayKey = L"top_right_delay";
		static constexpr std::wstring_view BottomLeftDelayKey = L"bottom_left_delay";
		static constexpr std::wstring_view BottomRightDelayKey = L"bottom_right_delay";

		static constexpr std::array<std::wstring_view, 5> CornerActionMapping = {
			L"none",
			L"task_view",
			L"start",
			L"search",
			L"go_to_desktop"
		};
	};
}