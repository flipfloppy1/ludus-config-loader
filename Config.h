#pragma once

#include "nlohmann/json.h"
#include <unordered_map>
#include "Log.h"
#include <fstream>

// Ludus Config Loader
namespace LCL
{
	

	enum ConfigValue
	{
		VNC_MODE,
		SENSITIVITY,
		TERMINAL_MODE,
		TEXT_COLOR,
		BUTTON_TINT,
		BG_TINT
	};

	struct ConfigLoc
	{
		std::string filepath;
		std::string jsonName;
	};

	static std::string controlsPath = "json/settings/controls.json";

	static std::string uiPath = "json/settings/ui.json";

	static std::unordered_map<ConfigValue, ConfigLoc> configLocations
	{
		{VNC_MODE,ConfigLoc{controlsPath, "vncMode"}},
		{SENSITIVITY, ConfigLoc{controlsPath, "mouseSensitivity"}},
		{TERMINAL_MODE, ConfigLoc{uiPath, "terminalMode"}},
		{TEXT_COLOR, ConfigLoc{uiPath, "textColor"}},
		{BUTTON_TINT, ConfigLoc{uiPath, "buttonTint"}},
		{BG_TINT, ConfigLoc{uiPath, "backgroundTint"}}
	};

	static void SaveJson(const char* filename, nlohmann::json jsonObject)
	{
		if (std::filesystem::exists(filename))
		{
			std::string message = "Overwriting JSON file \'";
			message.append(filename).append("\'");

			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::INFO, message.c_str());
		}
		else
		{
			std::string message = "JSON file \'";
			message.append(filename).append("\' does not exist and will be created");

			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::INFO, message.c_str());
		}
		std::ofstream file(filename);
		file << jsonObject;
		file.flush();
		file.close();
	}
	static nlohmann::json ReadJson(const char* filename)
	{
		std::ifstream jsonFile(filename);
		std::string data;
		if (!jsonFile.is_open())
		{
			std::string message = "JSON file \'";
			message.append(filename).append("\' could not be opened, returning null object");

			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
			data = "{}";
			return nlohmann::json::parse(data);
		};

		std::stringstream stream;
		stream << jsonFile.rdbuf();
		data = stream.str();

		stream.clear();
		jsonFile.close();
		return nlohmann::json::parse(data);
	}
	static void SaveValue(ConfigLoc configLoc, nlohmann::json newValue)
	{
		nlohmann::json settings = ReadJson(configLoc.filepath.c_str());
		if (!settings.contains("settings"))
		{
			std::string message = "Config file \'";
			message.append(configLoc.filepath).append("\' did not contain base element \'settings\' or the file did not exist, creating..");
			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::WARNING, message.c_str());
			settings["settings"].push_back(nlohmann::json::object({ std::pair<std::string, nlohmann::json>(configLoc.jsonName, newValue) }));
			SaveJson(configLoc.filepath.c_str(), settings);
			return;
		}
		if (!settings.at("settings").is_array())
		{
			settings.clear();
			settings["settings"].push_back(nlohmann::json::object({ std::pair<std::string, nlohmann::json>(configLoc.jsonName, newValue) }));
			SaveJson(configLoc.filepath.c_str(), settings);
			return;
		}
		for (nlohmann::json& object : settings.at("settings"))
		{
			if (object.contains(configLoc.jsonName))
			{
				object.at(configLoc.jsonName) = newValue;
				SaveJson(configLoc.filepath.c_str(), settings);
				return;
			}
		}

		std::string message = "Element \'";
		message.append(configLoc.jsonName).append("\' in file \'").append(configLoc.filepath).append("\' did not exist, creating..");
		LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::INFO, message.c_str());
		settings["settings"].push_back(nlohmann::json::object({ std::pair<std::string, nlohmann::json>(configLoc.jsonName, newValue) }));
		SaveJson(configLoc.filepath.c_str(), settings);
	}
	static void SaveValue(ConfigValue valueEnum, nlohmann::json newValue)
	{
		SaveValue(configLocations.at(valueEnum), newValue);
	}
	static nlohmann::json GetFloat(ConfigLoc configLoc)
	{
		nlohmann::json settings = ReadJson(configLoc.filepath.c_str());
		if (!settings.contains("settings"))
		{
			std::string message = "Config file \'";
			message.append(configLoc.filepath).append("\' did not contain base element \'settings\' or the file did not exist, returning 1.0f");
			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
			return 1.0f;
		}
		if (!settings.at("settings").is_array())
		{
			std::string message = "Element \'settings\' in config file \'";
			message.append(configLoc.filepath).append("\' was not an array, returning 1.0f");
			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
			return 1.0f;
		}
		for (nlohmann::json& object : settings.at("settings"))
		{
			if (object.contains(configLoc.jsonName))
			{
				if (object.at(configLoc.jsonName).is_number())
				{
					return object.at(configLoc.jsonName);
				}
				else
				{
					std::string message = "Element \'";
					message.append(configLoc.jsonName).append("\' in file \'").append(configLoc.filepath).append("\' was not a float, returning 1.0f");
					LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
					return 1.0f;
				}
			}
		}

		std::string message = "Config file \'";
		message.append(configLoc.filepath).append("\' did not contain element \'").append(configLoc.jsonName).append("\', returning 1.0f");
		LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
		return 1.0f;
	}
	static nlohmann::json GetFloat(ConfigValue valueEnum)
	{
		return GetFloat(configLocations.at(valueEnum));
	}
	static bool GetBool(ConfigLoc configLoc)
	{
		nlohmann::json settings = ReadJson(configLoc.filepath.c_str());
		if (!settings.contains("settings"))
		{
			std::string message = "Config file \'";
			message.append(configLoc.filepath).append("\' did not contain base element \'settings\' or the file did not exist, returning false");
			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
			return false;
		}
		if (!settings.at("settings").is_array())
		{
			std::string message = "Element \'settings\' in config file \'";
			message.append(configLoc.filepath).append("\' was not an array, returning false");
			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
			return false;
		}
		for (nlohmann::json& object : settings.at("settings"))
		{
			if (object.contains(configLoc.jsonName))
			{
				if (object.at(configLoc.jsonName).is_boolean())
				{
					return object.at(configLoc.jsonName);
				}
				else
				{
					std::string message = "Element \'";
					message.append(configLoc.jsonName).append("\' in file \'").append(configLoc.filepath).append("\' was not boolean, returning false");
					LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
					return false;
				}
			}
		}
		std::string message = "Config file \'";
		message.append(configLoc.filepath).append("\' did not contain element \'").append(configLoc.jsonName).append("\', returning false");
		LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
		return false;
	}
	static bool GetBool(ConfigValue valueEnum)
	{
		return GetBool(configLocations.at(valueEnum));
	}
	static std::string GetString(ConfigLoc configLoc)
	{
		nlohmann::json settings = ReadJson(configLoc.filepath.c_str());
		if (!settings.contains("settings"))
		{
			std::string message = "Config file \'";
			message.append(configLoc.filepath).append("\' did not contain base element \'settings\' or the file did not exist, returning empty string");
			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
			return "";
		}
		if (!settings.at("settings").is_array())
		{
			std::string message = "Element \'settings\' in config file \'";
			message.append(configLoc.filepath).append("\' was not an array, returning empty string");
			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
			return "";
		}
		for (nlohmann::json& object : settings.at("settings"))
		{
			if (object.contains(configLoc.jsonName))
			{
				if (object.at(configLoc.jsonName).is_string())
				{
					return object.at(configLoc.jsonName);
				}
				else
				{
					std::string message = "Element \'";
					message.append(configLoc.jsonName).append("\' in file \'").append(configLoc.filepath).append("\' was not string, returning empty string");
					LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
					return "";
				}
			}
		}
		std::string message = "Config file \'";
		message.append(configLoc.filepath).append("\' did not contain element \'").append(configLoc.jsonName).append("\', returning empty string");
		LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
		return "";
	}
	static std::string GetString(ConfigValue valueEnum)
	{
		return GetString(configLocations.at(valueEnum));
	}
	static nlohmann::json GetJson(ConfigLoc configLoc)
	{
		nlohmann::json settings = ReadJson(configLoc.filepath.c_str());
		if (!settings.contains("settings"))
		{
			std::string message = "Config file \'";
			message.append(configLoc.filepath).append("\' did not contain base element \'settings\' or the file did not exist, returning empty string");
			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
			return "";
		}
		if (!settings.at("settings").is_array())
		{
			std::string message = "Element \'settings\' in config file \'";
			message.append(configLoc.filepath).append("\' was not an array, returning empty string");
			LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
			return "";
		}
		for (nlohmann::json& object : settings.at("settings"))
		{
			if (object.contains(configLoc.jsonName))
			{
				return object.at(configLoc.jsonName);
			}
		}
		std::string message = "Config file \'";
		message.append(configLoc.filepath).append("\' did not contain element \'").append(configLoc.jsonName).append("\', returning null JSON object");
		LLG::Log(LLG::LogType::JSON, LLG::LogSeverity::MINOR_ERROR, message.c_str());
		return nlohmann::json();
	}
	static nlohmann::json GetJson(ConfigValue valueEnum)
	{
		return GetJson(configLocations.at(valueEnum));
	}
}