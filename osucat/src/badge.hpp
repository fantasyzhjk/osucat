#pragma once
#ifndef OC_BADGE_HPP
#define OC_BADGE_HPP
#define BADGENOTFOUND 65536

namespace osucat::badgeSystem {
	class main {
	public:
		std::vector<int> badgeParser(std::string badgeStr) {
			std::string word;
			std::vector<int> rtn;
			for (size_t i = 0; i < badgeStr.size(); ++i) {
				char c = badgeStr[i];
				if (c == ',') {
					rtn.push_back(stoi(word));
					word.clear();
				}
				else {
					word += c;
				}
				if (i == badgeStr.size() - 1) rtn.push_back(std::stoi(word));
			}
			return rtn;
		}
	};
}
#endif