#pragma once
#ifndef OC_MAIN_HPP
#define OC_MAIN_HPP

#include "message.h"
#include "exception.hpp"
#include "include/winhttp.hpp"
#include "include/osu_api.hpp"
#include "include/mysql.hpp"

using namespace std;
using namespace cqhttp_api;

namespace osucat {
	class CmdParser {





	};


	static class Main {
		void help(Target tar) {
			string cmd = tar.message.substr(4);
			utils::string_trim(cmd);
			utils::string_replace(cmd, " ", "");
			//string dir = (string)OC_ROOT_PATH + "\\", fileStr;
			if (_stricmp(cmd.substr(0, 4).c_str(), "bind") == 0) {
				cqhttp_api::send_message(tar, u8"[CQ:image,file=osucat\\help\\绑定.png]");
				return;
			}
			if (_stricmp(cmd.substr(0, 4).c_str(), "info") == 0) {
				cqhttp_api::send_message(tar, u8"[CQ:image,file=osucat\\help\\个人信息.png]");
				return;
			}
			if (_stricmp(cmd.substr(0, 5).c_str(), "score") == 0) {
				cqhttp_api::send_message(tar, u8"[CQ:image,file=osucat\\help\\成绩.png]");
				return;
			}
			if (_stricmp(cmd.substr(0, 6).c_str(), "custom") == 0) {
				cqhttp_api::send_message(tar, u8"[CQ:image,file=osucat\\help\\自定义.png]");
				return;
			}
			if (_stricmp(cmd.substr(0, 5).c_str(), "other") == 0) {
				cqhttp_api::send_message(tar, u8"[CQ:image,file=osucat\\help\\其他.png]");
				return;
			}
			cqhttp_api::send_message(tar, u8"[CQ:image,file=osucat\\help\\帮助.png]");
			return;
		}
		void about(Target tar) {
			cqhttp_api::send_message(tar, u8"[CQ:image,file=osucat\\help\\about.png]");
		}
		void contact(Target tar) {
			string cmd = tar.message.substr(7);
			utils::string_trim(cmd);
			cqhttp_api::send_message(tar, u8"[CQ:at,qq=" + to_string(tar.user_id) + u8"] 你想传达的话已成功传达给麻麻了哦。");
			cqhttp_api::send_private_message(owner_userid, u8"收到来自用户 " + to_string(tar.user_id) + u8" 的消息：" + cmd);
		}
		void ping(Target tar) {
			vector<string> rtnmsg;
			rtnmsg.push_back("pong");
			rtnmsg.push_back("paw");
			rtnmsg.push_back("meow");
			rtnmsg.push_back("nyan");
			rtnmsg.push_back("owo");
			rtnmsg.push_back("qwq");
			rtnmsg.push_back("喵");
			cqhttp_api::send_message(tar, rtnmsg[utils::randomNum(0, rtnmsg.size() - 1)]);
		}
		void setid(Target tar) {
			string cmd = utils::unescape(tar.message.substr(5));
			utils::string_trim(cmd);
			if (cmd.length() > 20) {
				cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
				return;
			}
			int64_t qq, userid;
			osu_api::v1::user_info UI = { 0 };
			Database db;
			db.Connect();
			db.addcallcount();
			userid = db.osu_getuserid(tar.user_id);
			if (userid != 0) {
				if (!osu_api::v1::api::GetUser(userid, osu_api::v1::mode::std, &UI) == 0) {
					cqhttp_api::send_message(tar, u8"你的账户已与osu!id为 " + UI.username + u8" 的用户绑定过了哦。如需解绑请阅读帮助信息~");
				}
				else {
					cqhttp_api::send_message(tar, 所绑定的用户已被bacho封禁);
				}
				return;
			}
			if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::std, &UI) == 0) {
				cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
				return;
			}
			qq = db.osu_getqq(UI.user_id);
			if (qq != 0) {
				cqhttp_api::send_message(tar,
					u8"你想要绑定的账户已与qq号为 " + to_string(qq)
					+ u8" 的用户绑定过了哦。如果你认为是他人错误绑定了你的账户，请联系麻麻~");
				return;
			}
			if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::std, &UI) == 0) {
				cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
				return;
			}
			char return_message[512];
			sprintf_s(return_message,
				512,
				u8"用户 %s 已成功绑定到此qq上~\n正在初始化数据，时间可能较长，请耐心等待。",
				UI.username);
			cqhttp_api::send_message(tar, return_message);
			cqhttp_api::send_group_message(management_groupid, u8"有1位用户绑定了他的osu!id,\nqq: " + to_string(tar.user_id)
				+ "\nosu!username: " + UI.username + "\nosu!uid: " + to_string(UI.user_id));
			time_t now = time(NULL);
			tm* tm_now = localtime(&now);
			char timeStr[30] = { 0 };
			strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_now);
			db.UserSet(UI.user_id, tar.user_id);
			db.AddUserData(&UI, timeStr);
			if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::taiko, &UI) != 0) {
				db.AddUserData(&UI, timeStr);
			}
			if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::ctb, &UI) != 0) {
				db.AddUserData(&UI, timeStr);
			}
			if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::mania, &UI) != 0) {
				db.AddUserData(&UI, timeStr);
			}
			//info(tar);
		};




	};









	void echo(const Target tar, const GroupSender sdr) {
		send_message(tar, tar.message);
	}

}

#endif