#include "ConfigMgr.h"

ConfigMgr::ConfigMgr()
{
	boost::filesystem::path current_path = boost::filesystem::current_path();
	boost::filesystem::path config_path = current_path / "config.ini";
	std::cout << "Config path : " << config_path << std::endl;

	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);

	for (const auto& [section_name, section_tree] : pt) {				//结构化绑定
		std::map<std::string, std::string> section_config;

		for (const auto& [key, value_node] : section_tree) {
			section_config[key] = value_node.get_value<std::string>();
		}

		_config_map[section_name]._section_datas = std::move(section_config);
	}

	for (const auto& [section_name, section_info] : _config_map) {
		std::cout << "[" << section_name << "]\n";
		for (const auto& [key, value] : section_info._section_datas) {
			std::cout << key << "=" << value << "\n";
		}
	}
}

ConfigMgr::ConfigMgr(const ConfigMgr& src)
{
	_config_map = src._config_map;
}

ConfigMgr::~ConfigMgr()
{
	_config_map.clear();
}

ConfigMgr& ConfigMgr::operator=(const ConfigMgr& src)
{
	if (&src == this) {
		return *this;
	}

	_config_map = src._config_map;
	return *this;
}

SectionInfo& ConfigMgr::operator[](const std::string& section) {
	return _config_map[section];
}
