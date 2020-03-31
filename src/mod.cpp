#include <il2cpp/il2cpp_binding.h>
#include <il2cpp/il2cpp_context.h>
#include <il2cpp/il2cpp_types.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "json.hpp"
using json = nlohmann::json;

struct Vector3 {
	float x, y, z;
};

struct Quat {
	float x, y, z, w;
};

bool nextPopupIsScore = false;
uint32_t nextMaxScore = 1;

struct Color {
	Color(uint32_t hexValue) {
		r = ((hexValue >> 16) & 0xFF);
		g = ((hexValue >> 8) & 0xFF);
		b = ((hexValue) & 0xFF);
	}

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a = 255;
};

std::vector<std::pair<float, Color>> colors;

extern "C" __declspec(dllexport) void registerHooks(il2cpp_binding &bindingCtx) {

	//Hook when we hit a target, so we know what high score to look for
	bindingCtx.bindClassFunction("", "Target", "CompleteTarget", InvokeTime::Before, [](const MethodInvocationContext& ctx, ThisPtr ths) -> void {
		nextPopupIsScore = true;

		auto targetClass = ctx.getGlobalContext().getClass("", "Target");
		auto cue = targetClass->field<internal::Il2CppObject>(ths, "mCue");
		int32_t behavior = cue.getClass()->field<int32_t>(cue, "behavior");

		if (behavior == 0 || behavior == 1 || behavior == 2 || behavior == 4) {
			nextMaxScore = 2000;
		}
		else if (behavior == 3) {
			nextMaxScore = 3000;
		}
		else if (behavior == 5) {
			nextMaxScore = 125;
		}
		else if (behavior == 6) {
			nextMaxScore = 2000;
		}
		else {
			nextMaxScore = 0;
		}
		
	});

	//Hook after creating the text popup, so we can grab it and change its color
	bindingCtx.bindClassFunction("", "TextPopupPool", "CreatePopup", InvokeTime::Before, [](const MethodInvocationContext& ctx, ThisPtr ths, Vector3 pos, Quat rot, Vector3 scale, internal::Il2CppString text, internal::Il2CppString extraText) -> void {
		auto &&gtx = ctx.getGlobalContext();

		if (nextPopupIsScore) {
			nextPopupIsScore = false;

			if (nextMaxScore == 0 || !text || gtx.getStringLength(text) <= 0) {
				return;
			}
				auto textChars = gtx.getStringChars(text);
				
			std::vector<char> scoreChar;
			scoreChar.resize(gtx.getStringLength(text));
			for (size_t i = 0; i < scoreChar.size(); ++i) {
				scoreChar[i] = static_cast<char>(textChars[i]);
			}

			int score = atoi(scoreChar.data());
			if (score < 0) {
				return;
			}

			float scorePercent = score / (float)nextMaxScore;
			if (scorePercent < 0 || scorePercent > 1) {
				return;
			}

			auto idx = ths.field<int32_t>("mIndex").get();

			internal::Il2CppObject arr = internal::Il2CppObject{ ths.field<void*>("mPopups").get() };
			auto popups = il2cppapi::Array<internal::Il2CppObject>(arr.ptr);
			auto popup = il2cppapi::Object(popups[idx], gtx.getClass("", "TextPopup"));
			auto textMesh = il2cppapi::Object(popup.field<internal::Il2CppObject>("text").get(), gtx.getClass("TMPro", "TextMeshPro"));

			std::optional<Color> foundColor;

			for (auto &&p : colors) {
				if (scorePercent <= p.first) {
					foundColor = p.second;
					break;
				}
			}

			if (!foundColor) {
				return;
			}

			uint32_t color = 0;

			// < 40%, color red
			if (scorePercent < 0.4f) {
				color = 0xff0000;
			}
			//40% - 70% green
			else if (scorePercent >= 0.4f && scorePercent < 0.7f) {
				color = 0x00FF4E;
			}
			//70% - 90% cyan
			else if (scorePercent >= 0.7f && scorePercent < 0.9f) {
				color = 0x00FFDE;
			}
			//90% - 100% gold
			else if (scorePercent >= 0.9f && scorePercent <= 1.0f) {
				color = 0xFFD800;
			}

			textMesh.method<void(Color)>("SetFaceColor")(textMesh, foundColor.value());
		}
	});


	if (!std::filesystem::exists("Mods/Config/HitScoreVisualizer.json")) {
		std::filesystem::create_directories("Mods/Config");
		std::ofstream baseConfigFile("Mods/Config/HitScoreVisualizer.json");

		json baseConfig = {
			{
				{ "cutoff", 0.4 },
				{ "color", "FF00FF" },
			},
			{
				{ "cutoff", 0.7 },
				{ "color", "00FF4E" },
			},
			{
				{ "cutoff", 0.9 },
				{ "color", "00FFDE" },
			},
			{
				{ "cutoff", 1.0 },
				{ "color", "FFD800" },
			}
		};

		baseConfigFile << std::setw(4) << baseConfig;
	}

	std::ifstream t("Mods/Config/HitScoreVisualizer.json");
	json jsonConfig;
	t >> jsonConfig;

	for(auto &&p : jsonConfig) {
		if (p.contains("cutoff") && p.contains("color")) {
			char*end;
			std::string hexV = p["color"];
			uint32_t h = strtoul(hexV.c_str(), &end, 16);
			colors.emplace_back(std::make_pair(p["cutoff"], Color(h)));
		}
	}

	std::sort(colors.begin(), colors.end(), [](const std::pair<float, Color> &lhs, const std::pair<float, Color> &rhs) {
		return lhs.first < rhs.first;
	});
}