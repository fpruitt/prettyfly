#pragma once
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

class Config
{
  public:
    static Config& instance()
    {
        static Config config;
        return config;
    }

    void load(const std::string& filename)
    {
        std::ifstream file(filename);

        if (!file.is_open())
        {
#ifdef PF_DEBUG
            assert(false && "Config file not found");
#else
            std::cerr << "[Config] Failed to open: " << filename << "\n";
#endif
            return;
        }

        std::string currentSection;
        std::string line;

        while (std::getline(file, line))
        {
            line = trim(line);

            // Skip empty lines and comments
            if (line.empty() || line[0] == ';' || line[0] == '#')
            {
                continue;
            }

            // Section header
            if (line[0] == '[' && line.back() == ']')
            {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }

            // Key=value pair
            auto delimPos = line.find('=');
            if (delimPos == std::string::npos)
            {
                continue;
            }

            std::string key = trim(line.substr(0, delimPos));
            std::string value = trim(line.substr(delimPos + 1));

            // Strip inline comments
            auto commentPos = value.find(';');
            if (commentPos != std::string::npos)
            {
                value = trim(value.substr(0, commentPos));
            }

            m_values[makeKey(currentSection, key)] = value;
        }

        std::cout << "[Config] Loaded: " << filename << " (" << m_values.size() << " values)\n";
    }

    std::string getString(const std::string& section, const std::string& key,
                          const std::string& defaultValue = "") const
    {
        return get(section, key, defaultValue);
    }

    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const
    {
        std::string val = get(section, key, std::to_string(defaultValue));
        try
        {
            return std::stoi(val);
        }
        catch (...)
        {
            return handleBadValue<int>(section, key, defaultValue);
        }
    }

    float getFloat(const std::string& section, const std::string& key, float defaultValue = 0.0f) const
    {
        std::string val = get(section, key, std::to_string(defaultValue));
        try
        {
            return std::stof(val);
        }
        catch (...)
        {
            return handleBadValue<float>(section, key, defaultValue);
        }
    }

    bool getBool(const std::string& section, const std::string& key, bool defaultValue = false) const
    {
        std::string val = get(section, key, defaultValue ? "true" : "false");
        if (val == "true" || val == "1" || val == "yes")
        {
            return true;
        }
        if (val == "false" || val == "0" || val == "no")
        {
            return false;
        }
        return handleBadValue<bool>(section, key, defaultValue);
    }

    bool hasKey(const std::string& section, const std::string& key) const
    {
        return m_values.count(makeKey(section, key)) > 0;
    }

  private:
    Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::unordered_map<std::string, std::string> m_values;

    static std::string makeKey(const std::string& section, const std::string& key)
    {
        return section + "." + key;
    }

    std::string get(const std::string& section, const std::string& key, const std::string& defaultValue) const
    {
        auto it = m_values.find(makeKey(section, key));

        if (it == m_values.end())
        {
#ifdef PF_DEBUG
            std::cerr << "[Config] Missing key: [" << section << "] " << key << "\n";
            assert(false && "Missing config key — see stderr for details");
#endif
            return defaultValue;
        }

        return it->second;
    }

    template <typename T> T handleBadValue(const std::string& section, const std::string& key, T defaultValue) const
    {
#ifdef PF_DEBUG
        std::cerr << "[Config] Bad value for key: [" << section << "] " << key << "\n";
        assert(false && "Bad config value — see stderr for details");
#endif
        return defaultValue;
    }

    static std::string trim(const std::string& str)
    {
        size_t start = str.find_first_not_of(" \t\r\n");
        size_t end = str.find_last_not_of(" \t\r\n");
        return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }
};
