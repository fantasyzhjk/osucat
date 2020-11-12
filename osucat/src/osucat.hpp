#pragma once
#ifndef OC_MAIN_HPP
#define OC_MAIN_HPP

using namespace std;
using namespace cqhttp_api;

#include "include/oppai-cpp/oppai.hpp"
#include "message.h"
#include "include/osu_api.hpp"
#include "badge.hpp"
#include "include/mysql.hpp"
#include "image.hpp"
#include "steamcat.hpp"
#include "addons.hpp"

namespace osucat {

	class Main {

	public:
		static void getcallcount(const Target tar) {
			Database db;
			db.Connect();
			send_message(tar, u8"猫猫从0.4版本开始，主要功能至今一共被调用了 " + std::to_string(db.Getcallcount()) + u8" 次。");
		}

		static void help(const Target tar) {
			string cmd = tar.message.substr(4);
			utils::string_trim(cmd);
			utils::string_replace(cmd, " ", "");
			//string dir = (string)OC_ROOT_PATH + "\\", fileStr;
			if (_stricmp(cmd.substr(0, 4).c_str(), "bind") == 0) {
				send_message(tar, u8"[CQ:image,file=osucat\\help\\绑定.png]");
				return;
			}
			if (_stricmp(cmd.substr(0, 4).c_str(), "info") == 0) {
				send_message(tar, u8"[CQ:image,file=osucat\\help\\个人信息.png]");
				return;
			}
			if (_stricmp(cmd.substr(0, 5).c_str(), "score") == 0) {
				send_message(tar, u8"[CQ:image,file=osucat\\help\\成绩.png]");
				return;
			}
			if (_stricmp(cmd.substr(0, 6).c_str(), "custom") == 0) {
				send_message(tar, u8"[CQ:image,file=osucat\\help\\自定义.png]");
				return;
			}
			if (_stricmp(cmd.substr(0, 5).c_str(), "other") == 0) {
				send_message(tar, u8"[CQ:image,file=osucat\\help\\其他.png]");
				return;
			}
			send_message(tar, u8"[CQ:image,file=osucat\\help\\帮助.png]");
			return;
		}

		static void about(const Target tar) {
			send_message(tar, u8"[CQ:image,file=osucat\\help\\about.png]");
		}

		static void contact(const Target tar) {
			string cmd = tar.message.substr(7);
			utils::string_trim(cmd);
			send_message(tar, u8"[CQ:at,qq=" + to_string(tar.user_id) + u8"] 你想传达的话已成功传达给麻麻了哦。");
			cqhttp_api::send_private_message(owner_userid, u8"收到来自用户 " + to_string(tar.user_id) + u8" 的消息：" + cmd);
		}

		static void ping(const Target tar) {
			vector<string> rtnmsg;
			rtnmsg.push_back("pong");
			rtnmsg.push_back("paw");
			rtnmsg.push_back("meow");
			rtnmsg.push_back("nyan");
			rtnmsg.push_back("owo");
			rtnmsg.push_back("qwq");
			rtnmsg.push_back(u8"喵");
			send_message(tar, rtnmsg[utils::randomNum(0, rtnmsg.size() - 1)]);
		}

		static void setid(const Target tar) {
			string cmd = utils::unescape(tar.message.substr(5));
			utils::string_trim(cmd);
			if (cmd.length() > 20) {
				send_message(tar, 所提供的参数超出长度限制);
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
					send_message(tar, u8"你的账户已与osu!id为 " + UI.username + u8" 的用户绑定过了哦。如需解绑请阅读帮助信息~");
				}
				else {
					send_message(tar, 所绑定的用户已被bacho封禁);
				}
				return;
			}
			else if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::std, &UI) == 0) {
				send_message(tar, 未从bancho检索到要查询的用户信息);
				return;
			}
			qq = db.osu_getqq(UI.user_id);
			if (qq != 0) {
				send_message(tar,
					u8"你想要绑定的账户已与qq号为 " + to_string(qq)
					+ u8" 的用户绑定过了哦。如果你认为是他人错误绑定了你的账户，请联系麻麻~");
				return;
			}
			char return_message[512];
			sprintf_s(return_message,
				512,
				u8"用户 %s 已成功绑定到此qq上~\n正在初始化数据，时间可能较长，请耐心等待。",
				UI.username);
			send_message(tar, return_message);
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
			info(tar);
		};

		static void unsetid(const Target tar) {
			string cmd = tar.message.substr(5);
			utils::string_trim(cmd);
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t uid = db.osu_getuserid(tar.user_id);
			string msg =
				u8"你确定要解除绑定吗？解除绑定后会清除所有绑定记录以及每日数据更新记录！\n如果要换绑的话，请使用!contact指令联系管理员，请不要随意解绑，被清除的每日数据记录是无法恢复的！\n如果确定要解绑，请使用命令!"
				u8"unset yes来进行确认！\n或回复!unset+任意内容中止操作。";
			if (uid == 0) {
				send_message(tar, u8"你还没有绑定...");
				return;
			}
			if (cmd.empty()) {
				if (db.osu_getunsetstatus(uid) == 0) {
					db.osu_changeunsetstatus(uid, true);
					send_message(tar, msg);
				}
				else {
					db.osu_changeunsetstatus(uid, false);
					send_message(tar, u8"解绑操作已中止。");
				}
			}
			else {
				if (cmd == "yes") {
					send_message(tar, u8"正在执行操作...");
					db.osu_deleteuser(uid);
					DeleteFileA(("/work/v1_cover/" + to_string(uid) + "jpg").c_str());
					send_message(tar, u8"已成功解绑。");
					osu_api::v1::user_info ui = { 0 };
					osu_api::v1::api::GetUser(uid, osu_api::v1::mode::std, &ui);
					cqhttp_api::send_group_message(management_groupid, u8"有1位用户解绑了他的osu!id,\nqq: " + to_string(tar.user_id)
						+ "\nosu!uid: " + to_string(uid) + "\n osu!username: " + ui.username);
				}
				else {
					db.osu_changeunsetstatus(uid, false);
					send_message(tar, u8"解绑操作已中止。");
				}
			}
		}

		static void info(const Target tar) {
			string cmd;
			if (_stricmp(tar.message.substr(0, 4).c_str(), "info") == 0) {
				cmd = tar.message.substr(4);
			}
			else if (_stricmp(cmd.substr(0, 6).c_str(), u8"因佛") == 0) {
				cmd = tar.message.substr(6);
			}
			utils::unescape(cmd);
			utils::string_trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			Database db;
			db.Connect();
			db.addcallcount();
			int temp, days = 0;
			float previousPP;
			osu_api::v1::mode gamemode;
			UserPanelData upd = { 0 };
			osu_api::v1::user_info previousUserInfo = { 0 };
			int64_t userid;
			string username = "";
			gamemode = osu_api::v1::mode::std;
			if (cmd.length() > 0) {
				if (cmd[0] == '#') {
					userid = db.osu_getuserid(tar.user_id);
					if (userid == 0) {
						send_message(tar, 用户没有绑定osu账号);
						return;
					}
					gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
					try {
						days = stoi(cmd.substr(cmd.find('#') + 1));
					}
					catch (std::exception) {
						days = 0;
					}
				}
				else if (cmd[0] == ':') {
					if (cmd.find('#') == string::npos) {
						userid = db.osu_getuserid(tar.user_id);
						if (userid == 0) {
							send_message(tar, 用户没有绑定osu账号);
							return;
						}
						else {
							try {
								temp = stoi(cmd.substr(cmd.length() - 1));
								if (temp < 4 && temp > -1) {
									gamemode = (osu_api::v1::mode)temp;
								}
								else {
									gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
								}
							}
							catch (std::exception) {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
							}
						}
					}
					else {
						userid = db.osu_getuserid(tar.user_id);
						if (userid == 0) {
							send_message(tar, 用户没有绑定osu账号);
							return;
						}
						else {
							try {
								temp = stoi(cmd.substr(cmd.find(':') + 1, 1));
								days = stoi(cmd.substr(cmd.find('#') + 1));
								if (days > 3651 || days < 0) days = 3650;
								if (temp < 4 && temp > -1) {
									gamemode = (osu_api::v1::mode)temp;
								}
								else {
									gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
								}
							}
							catch (std::exception) {
								days = 0;
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
							}
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							if (cmd.find('#') != string::npos) {
								temp = stoi(cmd.substr(cmd.find(':') + 1, 1));
								days = stoi(cmd.substr(cmd.find('#') + 1));
								if (days > 3651 || days < 0) days = 3650;
							}
							else {
								temp = stoi(cmd.substr(cmd.find(':') + 1));
							}
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (osu_api::v1::mode)temp;
						}
						else {
							days = 0;
							gamemode = osu_api::v1::mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
							try {
								if (cmd.find('#') != string::npos) {
									days = stoi(cmd.substr(cmd.find('#') + 1));
									if (days > 3651 || days < 0) days = 3650;
								}
							}
							catch (std::exception) {
								days = 0;
							}
						}
						else {
							if (cmd.find('#') != string::npos) {
								username = cmd.substr(0, cmd.find('#'));
								try {
									days = stoi(cmd.substr(cmd.find('#') + 1));
									if (days > 3651 || days < 0) days = 3650;
								}
								catch (std::exception) {
									days = 0;
								}
							}
							else {
								username = cmd;
								days = 0;
							}
						}
						if (username.empty()) {
							if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::std, &upd.user_info) == 0) {
								send_message(tar, 被查询的用户已被bancho封禁);
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(upd.user_info.user_id);
							}
						}
						else {
							if (username.length() > 20) {
								send_message(tar, 所提供的参数超出长度限制);
								return;
							}
							utils::string_trim(username);
							if (osu_api::v1::api::GetUser(username, osu_api::v1::mode::std, &upd.user_info) == 0) {
								send_message(tar, 未从bancho检索到要查询的用户信息);;
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(upd.user_info.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.osu_getuserid(tar.user_id);
				if (userid == 0) {
					send_message(tar, 用户没有绑定osu账号);
					return;
				}
				else {
					gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (osu_api::v1::api::GetUser(userid, gamemode, &upd.user_info) == 0) {
					send_message(tar, 被查询的用户已被bancho封禁);
					return;
				}
			}
			else {
				if (username.length() > 20) {
					send_message(tar, 所提供的参数超出长度限制);
					return;
				}
				utils::string_trim(username);
				if (osu_api::v1::api::GetUser(username, gamemode, &upd.user_info) == 0) {
					send_message(tar, 未从bancho检索到要查询的用户信息);
					return;
				}
			}
			// 剥离对pp+的依赖
			/*if (gamemode == mode::std) {
				if (db.GetUserPreviousPP(upd.user_info.user_id) != upd.user_info.pp) {
					upd.user_info.updatepplus = true;
				} else {
					upd.user_info.updatepplus = false;
					db.GetUserPreviousPPlus(upd.user_info.user_id, &upd.pplus_info);
				}
			}*/
			////
			db.osu_GetUserPreviousPPlus(upd.user_info.user_id, &upd.pplus_info);
			////
			upd.user_info.mode = gamemode;
			previousUserInfo.mode = gamemode;
			string fileStr;
			upd.badgeid = db.osu_getshownbadges(upd.user_info.user_id); //获取显示勋章
			if (db.osu_getqq(upd.user_info.user_id) != 0) {
				if (days > 0) {
					int dtmp = db.osu_GetUserData(upd.user_info.user_id, days, &previousUserInfo);
					if (dtmp == -1) {
						send_message(tar, u8"请求的日期没有数据...");
						return;
					}
					else if (dtmp == 0) {
						previousUserInfo.days_before = days;
						fileStr = "osucat\\" + infoPic_v1(upd, previousUserInfo, true);
					}
					else {
						previousUserInfo.days_before = dtmp;
						fileStr = "osucat\\" + infoPic_v1(upd, previousUserInfo, true, false);
					}
				}
				else {
					try {
						db.osu_GetUserData(upd.user_info.user_id, 0, &previousUserInfo);
						fileStr = "osucat\\" + infoPic_v1(upd, previousUserInfo, true);
					}
					catch (osucat::database_exception) {
						fileStr = "osucat\\" + infoPic_v1(upd, upd.user_info, true);
					}
				}
			}
			else {
				fileStr = "osucat\\" + infoPic_v1(upd, upd.user_info);
			}
			send_message(tar, u8"[CQ:image,file=" + fileStr + u8"]");
			utils::_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}

		// TODO: textinfo with data comparison

		static void recent(const Target tar) {
			string cmd;
			if (_stricmp(tar.message.substr(0, 6).c_str(), "recent") == 0) { cmd = tar.message.substr(6); }
			else if (_stricmp(tar.message.substr(0, 2).c_str(), "pr") == 0) { cmd = tar.message.substr(2); }
			else return;
			Database db;
			db.Connect();
			db.addcallcount();
			cmd = utils::unescape(cmd);
			utils::string_trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			ScorePanelData sp_data = { 0 };
			int temp;
			osu_api::v1::mode gamemode;
			gamemode = osu_api::v1::mode::std;
			string username = "";
			string beatmap;
			int64_t userid;
			char beatmap_url[512];
			if (cmd.length() > 0) {
				if (cmd[0] == ':') {
					userid = db.osu_getuserid(tar.user_id);
					if (userid == 0) {
						cqhttp_api::send_message(tar, 用户没有绑定osu账号);
						return;
					}
					else {
						try {
							if (utils::isNum(cmd.substr(cmd.length() - 1))) {
								temp = stoi(cmd.substr(cmd.length() - 1));
							}
							else {
								cqhttp_api::send_message(tar, 英文模式名提示);
								return;
							}
							if (temp < 4 && temp > -1) {
								gamemode = (osu_api::v1::mode)temp;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
							}
						}
						catch (std::exception) {
							gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							if (utils::isNum(cmd.substr(cmd.find(':') + 1))) {
								temp = stoi(cmd.substr(cmd.find(':') + 1));
							}
							else {
								cqhttp_api::send_message(tar, 英文模式名提示);
								return;
							}
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (osu_api::v1::mode)temp;
						}
						else {
							gamemode = osu_api::v1::mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
						}
						else {
							username = cmd;
						}
						if (username.empty()) {
							if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::std, &sp_data.user_info) == 0) {
								cqhttp_api::send_message(tar, 所绑定的用户已被bacho封禁);
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(sp_data.user_info.user_id);
							}
						}
						else {
							utils::string_trim(cmd);
							if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::std, &sp_data.user_info) == 0) {
								cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(sp_data.user_info.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.osu_getuserid(tar.user_id);
				if (userid == 0) {
					cqhttp_api::send_message(tar, 用户没有绑定osu账号);
					return;
				}
				else {
					gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (osu_api::v1::api::GetUser(userid, gamemode, &sp_data.user_info) == 0) {
					cqhttp_api::send_message(tar, u8"没有找到记录...");
					return;
				}
				if (osu_api::v1::api::GetUserRecent(userid, gamemode, &sp_data.score_info) == 0) {
					switch (gamemode) {
					case  osu_api::v1::mode::std:
						cqhttp_api::send_message(tar, u8"你最近在模式Standard上没有游玩记录~");
						return;
					case  osu_api::v1::mode::taiko:
						cqhttp_api::send_message(tar, u8"你最近在模式Taiko上没有游玩记录~");
						return;
					case  osu_api::v1::mode::ctb:
						cqhttp_api::send_message(tar, u8"你最近在模式Catch the Beat上没有游玩记录~");
						return;
					case  osu_api::v1::mode::mania:
						cqhttp_api::send_message(tar, u8"你最近在模式Mania上没有游玩记录~");
						return;
					}
				}
			}
			else {
				if (username.length() > 20) {
					cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
					return;
				}
				utils::string_trim(username);
				if (osu_api::v1::api::GetUser(username, gamemode, &sp_data.user_info) == 0) {
					cqhttp_api::send_message(tar, u8"没这个人或者TA根本不玩这个模式！");
					return;
				}
				if (osu_api::v1::api::GetUserRecent(username, gamemode, &sp_data.score_info) == 0) {
					switch (gamemode) {
					case  osu_api::v1::mode::std:
						cqhttp_api::send_message(tar, u8"TA最近在模式Standard上没有游玩记录~");
						return;
					case  osu_api::v1::mode::taiko:
						cqhttp_api::send_message(tar, u8"TA最近在模式Taiko上没有游玩记录~");
						return;
					case  osu_api::v1::mode::ctb:
						cqhttp_api::send_message(tar, u8"TA最近在模式Catch the Beat上没有游玩记录~");
						return;
					case  osu_api::v1::mode::mania:
						cqhttp_api::send_message(tar, u8"TA最近在模式Mania上没有游玩记录~");
						return;
					}
				}
			}
			sp_data.mode = gamemode;
			osu_api::v1::api::GetBeatmap(sp_data.score_info.beatmap_id, &sp_data.beatmap_info);
			sprintf_s(beatmap_url, OSU_FILE_URL "%lld", sp_data.score_info.beatmap_id);
			beatmap = NetConnection::HttpsGet(beatmap_url);
			if (beatmap.empty()) {
				cqhttp_api::send_message(tar, 获取谱面信息错误);
				return;
			}
			if (sp_data.mode == osu_api::v1::mode::std) {
				oppai pp;
				pp.read_data(beatmap);
				pp.mods((int)sp_data.score_info.mods);
				// if fc
				pp.accuracy_percent(-1);
				pp.n300((int)sp_data.score_info.n300);
				pp.n100((int)sp_data.score_info.n100);
				pp.n50((int)sp_data.score_info.n50);
				oppai_result t = pp.calc();
				sp_data.fc = (int)t.data.total_pp.value();
				// original
				pp.end((int)sp_data.score_info.n300 + (int)sp_data.score_info.n100 + (int)sp_data.score_info.n50
					+ (int)sp_data.score_info.nmiss);
				pp.nmiss((int)sp_data.score_info.nmiss);
				pp.combo((int)sp_data.score_info.combo);
				sp_data.pp_info = pp.calc();
				sp_data.pp_info.data.total_star = t.data.total_star;
				// reset
				pp.end(-1);
				pp.n300(-1);
				pp.n100(-1);
				pp.n50(-1);
				pp.nmiss(-1);
				pp.combo(-1);
				for (int i = 0; i < 5; ++i) {
					pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
					sp_data.confirmed_acc[i] = (int)(pp.calc().data.total_pp.value());
				}
				if (sp_data.pp_info.code == 0) {
					sp_data.pp_info.data.maxcombo = t.data.maxcombo;
					sp_data.pp_info.data.maxlength = t.data.length;
				}
			}
			else {
				if (sp_data.mode == osu_api::v1::mode::mania) {
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50.0 + sp_data.score_info.n100 * 100.0
						+ sp_data.score_info.nkatu * 200.0
						+ (sp_data.score_info.n300 + sp_data.score_info.ngeki) * 300.0;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.nkatu
							+ sp_data.score_info.n100 + sp_data.score_info.n300 + sp_data.score_info.ngeki);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == osu_api::v1::mode::taiko) {
					double n1, n2;
					n1 = ((double)sp_data.score_info.n100 + (double)sp_data.score_info.nkatu) * 0.5
						+ (double)sp_data.score_info.n300 + (double)sp_data.score_info.ngeki;
					n2 = (double)sp_data.score_info.nmiss + (double)sp_data.score_info.n100
						+ (double)sp_data.score_info.nkatu + (double)sp_data.score_info.n300
						+ (double)sp_data.score_info.ngeki;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == osu_api::v1::mode::ctb) {
					long n1, n2;
					n1 = sp_data.score_info.n50 + sp_data.score_info.n100 + sp_data.score_info.n300;
					n2 = sp_data.score_info.nkatu + sp_data.score_info.nmiss + sp_data.score_info.n50
						+ sp_data.score_info.n100 + sp_data.score_info.n300;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				sp_data.pp_info.data.title = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
				sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
				sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
				sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
				sp_data.pp_info.data.od = sp_data.beatmap_info.od;
				sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
				sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
				sp_data.pp_info.data.combo = sp_data.score_info.combo;
			}
			string fileStr = "osucat\\" + scorePic(sp_data);
			cqhttp_api::send_message(tar, u8"[CQ:image,file=" + fileStr + u8"]");
			db.osu_UpdatePPRecord(tar.user_id, sp_data.score_info.beatmap_id);
			utils::_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}

		static void bp(const Target tar) {
			string cmd = tar.message.substr(2);
			Database db;
			db.Connect();
			db.addcallcount();
			cmd = utils::unescape(cmd);
			utils::string_trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			ScorePanelData sp_data = { 0 };
			vector<osu_api::v1::score_info> SI;
			int temp;
			osu_api::v1::mode gamemode;
			gamemode = osu_api::v1::mode::std;
			string username = "";
			string beatmap;
			int64_t userid, bpnum;
			char beatmap_url[512];
			if (cmd.length() > 0) {
				if (cmd.find('#') != string::npos) {
					if (cmd.find(':') != string::npos) {
						try {
							if (cmd.find("at,qq=") != string::npos) {
								userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
								if (userid == 0) {
									cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
									return;
								}
							}
							else {
								username = cmd.substr(0, cmd.find('#'));
							}
						}
						catch (std::exception) {
							username = "%^%^%^!*(^&";
						}
						try {
							bpnum = stoll(cmd.substr(cmd.find('#') + 1, cmd.find(':')));
							bpnum > 100 ? bpnum = 100 : bpnum < 1 ? bpnum = 1 : bpnum = bpnum;
						}
						catch (std::exception) {
							bpnum = 1;
						}
						try {
							if (utils::isNum(cmd.substr(cmd.find(':') + 1))) {
								temp = stoi(cmd.substr(cmd.find(':') + 1));
							}
							else {
								cqhttp_api::send_message(tar, 英文模式名提示);
								return;
							}
							if (temp < 4 && temp > -1) {
								gamemode = (osu_api::v1::mode)temp;
							}
							else {
								gamemode = osu_api::v1::mode::std;
							}
						}
						catch (std::exception) {
							gamemode = osu_api::v1::mode::std;
						}
					}
					else {
						try {
							if (cmd.find("at,qq=") != string::npos) {
								userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
								if (userid == 0) {
									cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
									return;
								}
							}
							else {
								username = cmd.substr(0, cmd.find('#'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
						}
						catch (std::exception) {
							username = "%^%^%^!*(^&";
						}
						try {
							bpnum = stoll(cmd.substr(cmd.find('#') + 1));
							bpnum > 100 ? bpnum = 100 : bpnum < 1 ? bpnum = 1 : bpnum = bpnum;
						}
						catch (std::exception) {
							bpnum = 1;
						}
						gamemode = osu_api::v1::mode::std;
					}
				}
				else if (cmd.find(':') != string::npos) {
					if (cmd[0] == ':') {
						bpnum = 1;
						userid = db.osu_getuserid(tar.user_id);
						if (userid == 0) {
							cqhttp_api::send_message(tar, 用户没有绑定osu账号);
							return;
						}
						try {
							if (utils::isNum(cmd.substr(cmd.length() - 1))) {
								temp = stoi(cmd.substr(cmd.length() - 1));
							}
							else {
								cqhttp_api::send_message(tar, 英文模式名提示);
								return;
							}
							if (temp < 4 && temp > -1) {
								gamemode = (osu_api::v1::mode)temp;
							}
							else {
								gamemode = osu_api::v1::mode::std;
							}
						}
						catch (std::exception) {
							gamemode = (osu_api::v1::mode)temp;
						}
					}
					else if (cmd.find(':') != string::npos && cmd[0] != ':') {
						bpnum = 1;
						try {
							if (cmd.find("at,qq=") != string::npos) {
								userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
								if (userid == 0) {
									cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
									return;
								}
							}
							else {
								username = cmd.substr(0, cmd.find(':'));
							}
						}
						catch (std::exception) {
							username = "%^%^%^!*(^&";
						}
						try {
							temp = stoi(cmd.substr(cmd.length() - 1));
							if (temp < 4 && temp > -1) {
								gamemode = (osu_api::v1::mode)temp;
							}
							else {
								gamemode = osu_api::v1::mode::std;
							}
						}
						catch (std::exception) {
							gamemode = (osu_api::v1::mode)temp;
						}
					}
				}
				else {
					userid = db.osu_getuserid(tar.user_id);
					if (userid == 0) {
						cqhttp_api::send_message(tar, 用户没有绑定osu账号);
						return;
					}
					gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
					if (utils::isNum(cmd)) {
						bpnum = stoll(cmd);
						bpnum > 100 ? bpnum = 100 : bpnum < 1 ? bpnum = 1 : bpnum = bpnum;
					}
					else
						bpnum = 1;
				}
			}
			else {
				userid = db.osu_getuserid(tar.user_id);
				if (userid == 0) {
					cqhttp_api::send_message(tar, 用户没有绑定osu账号);
					return;
				}
				gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
				bpnum = 1;
			}
			if (username.empty()) {
				if (osu_api::v1::api::GetUser(userid, gamemode, &sp_data.user_info) == 0) {
					cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
					return;
				}
				if (osu_api::v1::api::GetUserBest(userid, bpnum, gamemode, SI) == 0) {
					cqhttp_api::send_message(tar, 没有查询到BP);
					return;
				}

			}
			else {
				if (username.length() > 20) {
					cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
					return;
				}
				utils::string_trim(username);
				if (osu_api::v1::api::GetUser(username, gamemode, &sp_data.user_info) == 0) {
					cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
					return;
				}
				if (osu_api::v1::api::GetUserBest(username, bpnum, gamemode, SI) == 0) {
					cqhttp_api::send_message(tar, 没有查询到他人BP);
					return;
				}
			}
			if (bpnum != SI.size()) {
				cqhttp_api::send_message(tar, 没有查询到BP);
				return;
			}
			sp_data.mode = gamemode;
			sp_data.score_info.achieved_timestamp = SI[bpnum - 1].achieved_timestamp;
			sp_data.score_info.beatmap_id = SI[bpnum - 1].beatmap_id;
			sp_data.score_info.score = SI[bpnum - 1].score;
			sp_data.score_info.combo = SI[bpnum - 1].combo;
			sp_data.score_info.n50 = SI[bpnum - 1].n50;
			sp_data.score_info.n100 = SI[bpnum - 1].n100;
			sp_data.score_info.n300 = SI[bpnum - 1].n300;
			sp_data.score_info.nkatu = SI[bpnum - 1].nkatu;
			sp_data.score_info.ngeki = SI[bpnum - 1].ngeki;
			sp_data.score_info.nmiss = SI[bpnum - 1].nmiss;
			sp_data.score_info.mods = SI[bpnum - 1].mods;
			sp_data.score_info.user_id = SI[bpnum - 1].user_id;
			sp_data.score_info.rank = SI[bpnum - 1].rank;
			sp_data.score_info.pp = SI[bpnum - 1].pp;
			sp_data.score_info.username = sp_data.user_info.username;
			osu_api::v1::api::GetBeatmap(sp_data.score_info.beatmap_id, &sp_data.beatmap_info);
			sprintf_s(beatmap_url, OSU_FILE_URL "%lld", sp_data.score_info.beatmap_id);
			beatmap = NetConnection::HttpsGet(beatmap_url);
			if (beatmap.empty()) {
				cqhttp_api::send_message(tar, 获取谱面信息错误);
				return;
			}
			if (sp_data.mode == osu_api::v1::mode::std) {
				oppai pp;
				pp.read_data(beatmap);
				pp.mods((int)sp_data.score_info.mods);
				// if fc
				pp.accuracy_percent(-1);
				pp.n300(sp_data.score_info.n300);
				pp.n100(sp_data.score_info.n100);
				pp.n50(sp_data.score_info.n50);
				oppai_result t = pp.calc();
				if (t.data.total_pp.has_value()) {
					sp_data.fc = t.data.total_pp.value();
					// original
					pp.nmiss(sp_data.score_info.nmiss);
					pp.combo(sp_data.score_info.combo);
					sp_data.pp_info = pp.calc();
					sp_data.pp_info.data.total_pp = sp_data.score_info.pp;
					sp_data.pp_info.data.total_star = t.data.total_star;
					// reset
					pp.end(-1);
					pp.n300(-1);
					pp.n100(-1);
					pp.n50(-1);
					pp.nmiss(-1);
					pp.combo(-1);
					for (int i = 0; i < 5; ++i) {
						pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
						sp_data.confirmed_acc[i] = (int)(pp.calc().data.total_pp.value());
					}
					if (sp_data.pp_info.code == 0) {
						sp_data.pp_info.data.maxcombo = t.data.maxcombo;
						sp_data.pp_info.data.maxlength = t.data.length;
					}
				}
				else {
					sp_data.fc = 0;
					sp_data.confirmed_acc[0] = 0;
					sp_data.confirmed_acc[1] = 0;
					sp_data.confirmed_acc[2] = 0;
					sp_data.confirmed_acc[3] = 0;
					sp_data.confirmed_acc[4] = 0;
					sp_data.pp_info.data.title = sp_data.beatmap_info.title;
					sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
					sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
					sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
					sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
					sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
					sp_data.pp_info.data.od = sp_data.beatmap_info.od;
					sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
					sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
					sp_data.pp_info.data.combo = sp_data.score_info.combo;
					sp_data.pp_info.data.total_pp = sp_data.score_info.pp;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50 + sp_data.score_info.n100 * 100 + sp_data.score_info.n300 * 300;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.n100
							+ sp_data.score_info.n300);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}

			}
			else {
				if (sp_data.mode == osu_api::v1::mode::mania) {
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50.0 + sp_data.score_info.n100 * 100.0
						+ sp_data.score_info.nkatu * 200.0
						+ (sp_data.score_info.n300 + sp_data.score_info.ngeki) * 300.0;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.nkatu
							+ sp_data.score_info.n100 + sp_data.score_info.n300 + sp_data.score_info.ngeki);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == osu_api::v1::mode::taiko) {
					double n1, n2;
					n1 = ((double)sp_data.score_info.n100 + (double)sp_data.score_info.nkatu) * 0.5
						+ (double)sp_data.score_info.n300 + (double)sp_data.score_info.ngeki;
					n2 = (double)sp_data.score_info.nmiss + (double)sp_data.score_info.n100
						+ (double)sp_data.score_info.nkatu + (double)sp_data.score_info.n300
						+ (double)sp_data.score_info.ngeki;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == osu_api::v1::mode::ctb) {
					long n1, n2;
					n1 = sp_data.score_info.n50 + sp_data.score_info.n100 + sp_data.score_info.n300;
					n2 = sp_data.score_info.nkatu + sp_data.score_info.nmiss + sp_data.score_info.n50
						+ sp_data.score_info.n100 + sp_data.score_info.n300;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				sp_data.pp_info.data.title = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
				sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
				sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
				sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
				sp_data.pp_info.data.od = sp_data.beatmap_info.od;
				sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
				sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
				sp_data.pp_info.data.combo = sp_data.score_info.combo;
				sp_data.pp_info.data.total_pp = sp_data.score_info.pp;
			}
			string fileStr = "osucat\\" + scorePic(sp_data);
			cqhttp_api::send_message(tar, u8"[CQ:image,file=" + fileStr + u8"]");
			db.osu_UpdatePPRecord(tar.user_id, sp_data.score_info.beatmap_id);
			utils::_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}

		static void score(const Target tar) {
			string cmd = tar.message.substr(5);
			utils::string_replace(cmd, " ", "");
			if (!utils::isNum(cmd)) {
				cqhttp_api::send_message(tar, u8"都说啦暂时不支持文字查询啦，请输入bid来查询~");
				return;
			}
			int64_t bid = stoll(cmd);
			Database db;
			string beatmap;
			char beatmap_url[512];
			ScorePanelData sp_data = { 0 };
			db.Connect();
			db.addcallcount();
			int64_t uid = db.osu_getuserid(tar.user_id);
			if (uid == 0) {
				cqhttp_api::send_message(tar, 用户没有绑定osu账号);
				return;
			}
			osu_api::v1::mode gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(uid);
			if (osu_api::v1::api::GetUser(uid, gamemode, &sp_data.user_info) == 0) {
				cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
				return;
			}
			if (osu_api::v1::api::GetUserScore(sp_data.user_info.user_id, gamemode, bid, &sp_data.score_info) == 0) {
				cqhttp_api::send_message(tar, 没有查询到成绩);
				return;
			}
			sp_data.mode = gamemode;
			sp_data.score_info.username = sp_data.user_info.username;
			osu_api::v1::api::GetBeatmap(sp_data.score_info.beatmap_id, &sp_data.beatmap_info);
			sprintf_s(beatmap_url, OSU_FILE_URL "%lld", sp_data.score_info.beatmap_id);
			beatmap = NetConnection::HttpsGet(beatmap_url);
			if (beatmap.empty()) {
				cqhttp_api::send_message(tar, 获取谱面信息错误);
				return;
			}
			if (sp_data.mode == osu_api::v1::mode::std) {
				oppai pp;
				pp.read_data(beatmap);
				pp.mods((int)sp_data.score_info.mods);
				// if fc
				pp.accuracy_percent(-1);
				pp.n300((int)sp_data.score_info.n300);
				pp.n100((int)sp_data.score_info.n100);
				pp.n50((int)sp_data.score_info.n50);
				oppai_result t = pp.calc();
				sp_data.fc = (int)t.data.total_pp.value();
				// original
				pp.end((int)sp_data.score_info.n300 + (int)sp_data.score_info.n100 + (int)sp_data.score_info.n50
					+ (int)sp_data.score_info.nmiss);
				pp.nmiss((int)sp_data.score_info.nmiss);
				pp.combo((int)sp_data.score_info.combo);
				sp_data.pp_info = pp.calc();
				sp_data.pp_info.data.total_star = t.data.total_star;
				// reset
				pp.end(-1);
				pp.n300(-1);
				pp.n100(-1);
				pp.n50(-1);
				pp.nmiss(-1);
				pp.combo(-1);
				for (int i = 0; i < 5; ++i) {
					pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
					sp_data.confirmed_acc[i] = (int)(pp.calc().data.total_pp.value());
				}
				if (sp_data.pp_info.code == 0) {
					sp_data.pp_info.data.maxcombo = t.data.maxcombo;
					sp_data.pp_info.data.maxlength = t.data.length;
				}
			}
			else {
				if (sp_data.mode == osu_api::v1::mode::mania) {
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50.0 + sp_data.score_info.n100 * 100.0
						+ sp_data.score_info.nkatu * 200.0
						+ (sp_data.score_info.n300 + sp_data.score_info.ngeki) * 300.0;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.nkatu
							+ sp_data.score_info.n100 + sp_data.score_info.n300 + sp_data.score_info.ngeki);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == osu_api::v1::mode::taiko) {
					double n1, n2;
					n1 = ((double)sp_data.score_info.n100 + (double)sp_data.score_info.nkatu) * 0.5
						+ (double)sp_data.score_info.n300 + (double)sp_data.score_info.ngeki;
					n2 = (double)sp_data.score_info.nmiss + (double)sp_data.score_info.n100
						+ (double)sp_data.score_info.nkatu + (double)sp_data.score_info.n300
						+ (double)sp_data.score_info.ngeki;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == osu_api::v1::mode::ctb) {
					long n1, n2;
					n1 = sp_data.score_info.n50 + sp_data.score_info.n100 + sp_data.score_info.n300;
					n2 = sp_data.score_info.nkatu + sp_data.score_info.nmiss + sp_data.score_info.n50
						+ sp_data.score_info.n100 + sp_data.score_info.n300;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				sp_data.pp_info.data.title = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
				sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
				sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
				sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
				sp_data.pp_info.data.od = sp_data.beatmap_info.od;
				sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
				sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
				sp_data.pp_info.data.combo = sp_data.score_info.combo;
			}
			string fileStr = "osucat\\" + scorePic(sp_data);
			cqhttp_api::send_message(tar, u8"[CQ:image,file=" + fileStr + u8"]");
			db.osu_UpdatePPRecord(tar.user_id, sp_data.beatmap_info.beatmap_id);
			utils::_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}

		static void update(const Target tar) {
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t uid = db.osu_getuserid(tar.user_id);
			if (uid == 0) {
				cqhttp_api::send_message(tar, 用户没有绑定osu账号);
				return;
			}
			osu_api::v1::user_info UI;
			osu_api::v1::mode mode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(uid);
			int8_t returnCode = osu_api::v1::api::GetUser(uid, mode, &UI);
			if (returnCode == 0) {
				cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
				return;
			}
			cqhttp_api::send_message(tar, u8"少女祈祷中...");
			DeleteFileA(("./work/avatar/" + to_string(UI.user_id) + ".png").c_str());
			vector<long> pp_plus;
			try {
				pp_plus = NetConnection::getUserPlusData(UI.user_id);
				osu_api::v1::pplus_info pi;
				pi.acc = pp_plus[0];
				pi.flow = pp_plus[1];
				pi.jump = pp_plus[2];
				pi.pre = pp_plus[3];
				pi.spd = pp_plus[4];
				pi.sta = pp_plus[5];
				db.osu_UpdatePPlus(UI.user_id, UI.pp, pi);
			}
			catch (std::exception) {
			}
			info(tar);
		}

		static void pp(const Target tar) {
			string cmd = tar.message.substr(2);
			utils::string_replace(cmd, " ", "");
			if (!utils::isNum(cmd)) {
				cqhttp_api::send_message(tar, u8"请键入正确的bid!");
				return;
			}
			string beatmap = NetConnection::HttpsGet(OSU_FILE_URL + cmd);
			if (beatmap.empty()) {
				cqhttp_api::send_message(tar, 获取谱面信息错误);
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			db.osu_UpdatePPRecord(tar.user_id, stoll(cmd));
			oppai pp;
			vector<float> out;
			char message[5120];
			pp.read_data(beatmap);
			oppai_result t = pp.calc();
			if (t.code == -4) {
				cqhttp_api::send_message(tar, u8"暂不支持除Standard模式以外的其它模式。");
				return;
			}
			for (int i = 0; i < 5; ++i) {
				pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
				oppai_result c = pp.calc();
				out.push_back((float)(c.data.total_pp.has_value() ? c.data.total_pp.value() : 0.0));
			}
			string title, artist, version;
			if (t.data.titleUnicode.length() > 2 && t.data.titleUnicode.length() < 128) {
				title = t.data.titleUnicode;
			}
			else {
				t.data.title.length() < 255 ? title = t.data.title : title = "太长了";
			}
			if (t.data.artistUnicode.length() > 2 && t.data.titleUnicode.length() < 128) {
				artist = t.data.artistUnicode;
			}
			else {
				t.data.artist.length() < 255 ? artist = t.data.artist : artist = "太长了";
			}
			t.data.map_version.length() < 255 ? version = t.data.map_version : version = "太长了";
			sprintf_s(message,
				5120,
				u8"%s - %s[%s]\n"
				u8"mapped by %s\n"
				"ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f\n"
				"Stars:%.2f* | No Mod | objects:%d\n\n"
				"100%% : %.2fpp\n"
				"99%% : %.2fpp\n"
				"98%% : %.2fpp\n"
				"97%% : %.2fpp\n"
				"95%% : %.2fpp\n\n%s",
				artist.c_str(),
				title.c_str(),
				version.c_str(),
				t.data.creator.c_str(),
				t.data.ar,
				t.data.od,
				t.data.cs,
				t.data.hp,
				t.data.bpm,
				t.data.total_star,
				t.data.n300,
				out[4],
				out[3],
				out[2],
				out[1],
				out[0],
				("https://osu.ppy.sh/b/" + cmd).c_str());
			cqhttp_api::send_message(tar, message);
		}

		static void ppwith(const Target tar) {
			string cmd = tar.message.substr(4);
			if (cmd.length() > 15) {
				cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
				return;
			}
			if (cmd.find("[CQ:") != string::npos) {
				cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t bid = db.osu_GetPPRecord(tar.user_id);
			if (bid == EOF) {
				cqhttp_api::send_message(tar, u8"你还没有查询过成绩，请先查询成绩后在来使用这条指令");
				return;
			}
			if (!Mod(cmd).isVaild()) {
				cqhttp_api::send_message(tar, u8"参数不正确");
				return;
			}
			string beatmap = NetConnection::HttpsGet(OSU_FILE_URL + to_string(bid));
			if (beatmap.empty()) {
				cqhttp_api::send_message(tar, 获取谱面信息错误);
				return;
			}
			oppai pp;
			vector<float> out;
			char message[5120];
			pp.read_data(beatmap);
			pp.mods(Mod(cmd).GetModNumber());
			oppai_result t = pp.calc();
			for (int i = 0; i < 5; ++i) {
				pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
				out.push_back((float)(pp.calc().data.total_pp.value()));
			}
			string modStr = Mod(cmd).GetModString();
			string title, artist, version;
			if (t.data.titleUnicode.length() > 2 && t.data.titleUnicode.length() < 128) {
				title = t.data.titleUnicode;
			}
			else {
				t.data.title.length() < 255 ? title = t.data.title : title = "太长了";
			}
			if (t.data.artistUnicode.length() > 2 && t.data.titleUnicode.length() < 128) {
				artist = t.data.artistUnicode;
			}
			else {
				t.data.artist.length() < 255 ? artist = t.data.artist : artist = "太长了";
			}
			t.data.map_version.length() < 255 ? version = t.data.map_version : version = "太长了";
			sprintf_s(message,
				5120,
				u8"%s - %s[%s]\n"
				u8"mapped by %s\n"
				"[ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f]\n"
				"%s | Stars:%.2f* | objects:%d\n\n"
				"100%% : %.2fpp\n"
				"99%% : %.2fpp\n"
				"98%% : %.2fpp\n"
				"97%% : %.2fpp\n"
				"95%% : %.2fpp\n\n%s",
				artist.c_str(),
				title.c_str(),
				version.c_str(),
				t.data.creator.c_str(),
				t.data.ar,
				t.data.od,
				t.data.cs,
				t.data.hp,
				t.data.bpm,
				modStr.length() == 0 ? "No Mod" : modStr.c_str(),
				t.data.total_star,
				t.data.n300,
				out[4],
				out[3],
				out[2],
				out[1],
				out[0],
				("https://osu.ppy.sh/b/" + to_string(bid)).c_str());
			cqhttp_api::send_message(tar, message);
		}

		static void rctpp(const Target tar) {
			Database db;
			db.Connect();
			db.addcallcount();
			string cmd = utils::unescape(tar.message.substr(5));
			utils::string_trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			ScorePanelData sp_data = { 0 };
			int temp;
			osu_api::v1::mode gamemode;
			gamemode = osu_api::v1::mode::std;
			string username = "";
			string beatmap;
			int64_t userid;
			char beatmap_url[512];
			if (cmd.length() > 0) {
				if (cmd[0] == ':') {
					userid = db.osu_getuserid(tar.user_id);
					if (userid == 0) {
						cqhttp_api::send_message(tar, 用户没有绑定osu账号);
						return;
					}
					else {
						try {
							if (utils::isNum(cmd.substr(cmd.length() - 1))) {
								temp = stoi(cmd.substr(cmd.length() - 1));
							}
							else {
								cqhttp_api::send_message(tar, 英文模式名提示);
								return;
							}
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (osu_api::v1::mode)temp;
						}
						else {
							gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							if (utils::isNum(cmd.substr(cmd.find(':') + 1))) {
								temp = stoi(cmd.substr(cmd.find(':') + 1));
							}
							else {
								cqhttp_api::send_message(tar, 英文模式名提示);
								return;
							}
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (osu_api::v1::mode)temp;
						}
						else {
							gamemode = osu_api::v1::mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
						}
						else {
							username = cmd;
						}
						if (username.empty()) {
							if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::std, &sp_data.user_info) == 0) {
								cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(sp_data.user_info.user_id);
							}
						}
						else {
							utils::string_trim(cmd);
							if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::std, &sp_data.user_info) == 0) {
								cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(sp_data.user_info.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.osu_getuserid(tar.user_id);
				if (userid == 0) {
					cqhttp_api::send_message(tar, 用户没有绑定osu账号);
					return;
				}
				else {
					gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (osu_api::v1::api::GetUser(userid, gamemode, &sp_data.user_info) == 0) {
					cqhttp_api::send_message(tar, u8"没有找到记录...");
					return;
				}
				if (osu_api::v1::api::GetUserRecent(userid, gamemode, &sp_data.score_info) == 0) {
					switch (gamemode) {
					case osu_api::v1::mode::std:
						cqhttp_api::send_message(tar, u8"你最近在模式Standard上没有游玩记录~");
						return;
					case osu_api::v1::mode::taiko:
						cqhttp_api::send_message(tar, u8"你最近在模式Taiko上没有游玩记录~");
						return;
					case osu_api::v1::mode::ctb:
						cqhttp_api::send_message(tar, u8"你最近在模式Catch the Beat上没有游玩记录~");
						return;
					case osu_api::v1::mode::mania:
						cqhttp_api::send_message(tar, u8"你最近在模式Mania上没有游玩记录~");
						return;
					}
				}
			}
			else {
				if (username.length() > 20) {
					"";
					cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
					return;
				}
				utils::string_trim(username);
				if (osu_api::v1::api::GetUser(username, gamemode, &sp_data.user_info) == 0) {
					cqhttp_api::send_message(tar, u8"没这个人或者TA根本不玩这个模式！");
					return;
				}
				if (osu_api::v1::api::GetUserRecent(username, gamemode, &sp_data.score_info) == 0) {
					switch (gamemode) {
					case osu_api::v1::mode::std:
						cqhttp_api::send_message(tar, u8"TA最近在模式Standard上没有游玩记录~");
						return;
					case osu_api::v1::mode::taiko:
						cqhttp_api::send_message(tar, u8"TA最近在模式Taiko上没有游玩记录~");
						return;
					case osu_api::v1::mode::ctb:
						cqhttp_api::send_message(tar, u8"TA最近在模式Catch the Beat上没有游玩记录~");
						return;
					case osu_api::v1::mode::mania:
						cqhttp_api::send_message(tar, u8"TA最近在模式Mania上没有游玩记录~");
						return;
					}
				}
			}
			sp_data.mode = gamemode;
			osu_api::v1::api::GetBeatmap(sp_data.score_info.beatmap_id, &sp_data.beatmap_info);
			sprintf_s(beatmap_url, OSU_FILE_URL "%lld", sp_data.score_info.beatmap_id);
			beatmap = NetConnection::HttpsGet(beatmap_url);
			if (beatmap.empty()) {
				cqhttp_api::send_message(tar, 获取谱面信息错误);
				return;
			}
			int second, minute, maxsecond, maxminute;
			if (sp_data.mode == osu_api::v1::mode::std) {
				oppai pp;
				pp.read_data(beatmap);
				pp.mods((int)sp_data.score_info.mods);
				// if fc
				pp.accuracy_percent(-1);
				pp.n300((int)sp_data.score_info.n300);
				pp.n100((int)sp_data.score_info.n100);
				pp.n50((int)sp_data.score_info.n50);
				oppai_result t = pp.calc();
				sp_data.fc = (int)t.data.total_pp.value();
				// original
				pp.end((int)sp_data.score_info.n300 + (int)sp_data.score_info.n100 + (int)sp_data.score_info.n50
					+ (int)sp_data.score_info.nmiss);
				pp.nmiss((int)sp_data.score_info.nmiss);
				pp.combo((int)sp_data.score_info.combo);
				sp_data.pp_info = pp.calc();
				sp_data.pp_info.data.total_star = t.data.total_star;
				// reset
				pp.end(-1);
				pp.n300(-1);
				pp.n100(-1);
				pp.n50(-1);
				pp.nmiss(-1);
				pp.combo(-1);
				for (int i = 0; i < 5; ++i) {
					pp.accuracy_percent((float)(i == 0 ? 95 : 96 + i));
					sp_data.confirmed_acc[i] = (int)(pp.calc().data.total_pp.value());
				}
				if (sp_data.pp_info.code == 0) {
					sp_data.pp_info.data.maxcombo = t.data.maxcombo;
					sp_data.pp_info.data.maxlength = t.data.length;
				}
				second = sp_data.pp_info.data.length / 1000 % 60;
				minute = sp_data.pp_info.data.length / 1000 / 60;
				maxsecond = sp_data.pp_info.data.maxlength / 1000 % 60;
				maxminute = sp_data.pp_info.data.maxlength / 1000 / 60;
			}
			else {
				if (sp_data.mode == osu_api::v1::mode::mania) {
					long n1, n2;
					n1 = sp_data.score_info.n50 * 50.0 + sp_data.score_info.n100 * 100.0
						+ sp_data.score_info.nkatu * 200.0
						+ (sp_data.score_info.n300 + sp_data.score_info.ngeki) * 300.0;
					n2 = 300
						* (sp_data.score_info.nmiss + sp_data.score_info.n50 + sp_data.score_info.nkatu
							+ sp_data.score_info.n100 + sp_data.score_info.n300 + sp_data.score_info.ngeki);
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == osu_api::v1::mode::taiko) {
					double n1, n2;
					n1 = ((double)sp_data.score_info.n100 + (double)sp_data.score_info.nkatu) * 0.5
						+ (double)sp_data.score_info.n300 + (double)sp_data.score_info.ngeki;
					n2 = (double)sp_data.score_info.nmiss + (double)sp_data.score_info.n100
						+ (double)sp_data.score_info.nkatu + (double)sp_data.score_info.n300
						+ (double)sp_data.score_info.ngeki;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				if (sp_data.mode == osu_api::v1::mode::ctb) {
					long n1, n2;
					n1 = sp_data.score_info.n50 + sp_data.score_info.n100 + sp_data.score_info.n300;
					n2 = sp_data.score_info.nkatu + sp_data.score_info.nmiss + sp_data.score_info.n50
						+ sp_data.score_info.n100 + sp_data.score_info.n300;
					sp_data.pp_info.data.maxcombo = sp_data.beatmap_info.maxcombo;
					sp_data.pp_info.data.accuracy = (double)n1 / (double)n2 * 100.0;
				}
				second = 0;
				minute = 0;
				maxsecond = 0;
				maxminute = 0;
				sp_data.pp_info.data.titleUnicode = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artistUnicode = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.title = sp_data.beatmap_info.title;
				sp_data.pp_info.data.artist = sp_data.beatmap_info.artist;
				sp_data.pp_info.data.creator = sp_data.beatmap_info.creator;
				sp_data.pp_info.data.total_star = sp_data.beatmap_info.stars;
				sp_data.pp_info.data.bpm = sp_data.beatmap_info.bpm;
				sp_data.pp_info.data.ar = sp_data.beatmap_info.ar;
				sp_data.pp_info.data.od = sp_data.beatmap_info.od;
				sp_data.pp_info.data.cs = sp_data.beatmap_info.cs;
				sp_data.pp_info.data.hp = sp_data.beatmap_info.hp;
				sp_data.pp_info.data.combo = sp_data.score_info.combo;
			}
			char message[5120];
			string modeStr;
			sp_data.mode == osu_api::v1::mode::std
				? modeStr = "Standard"
				: sp_data.mode == osu_api::v1::mode::taiko
				? modeStr = "Taiko"
				: sp_data.mode == osu_api::v1::mode::ctb
				? modeStr = "CtB"
				: sp_data.mode == osu_api::v1::mode::mania ? modeStr = "Mania" : modeStr = "Unknown";

			if (sp_data.mode == osu_api::v1::mode::std) {
				sprintf_s(message,
					5120,
					u8"%s's previous play - %s\n"
					u8"%s - %s[%s]\n"
					"mapped by %s\n"
					"[ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f]\n"
					"Stars:%.2f* | objects:%d | %d:%02d / %d:%02d\n"
					"300×%d | 100×%d | 50×%d | miss×%d\n"
					"combo %dx/%dx | acc %.2f%% | %s | %s\n"
					"%.0fpp | if fc %dpp\n\n"
					"100%% : %dpp\n"
					"99%% : %dpp\n"
					"98%% : %dpp\n"
					"95%% : %dpp\n%s",
					sp_data.user_info.username.c_str(),
					modeStr.c_str(),
					sp_data.pp_info.data.artistUnicode.c_str(),
					sp_data.pp_info.data.titleUnicode.c_str(),
					sp_data.pp_info.data.map_version.c_str(),
					sp_data.pp_info.data.creator.c_str(),
					sp_data.pp_info.data.ar,
					sp_data.pp_info.data.od,
					sp_data.pp_info.data.cs,
					sp_data.pp_info.data.hp,
					sp_data.pp_info.data.bpm,
					sp_data.pp_info.data.total_star,
					sp_data.pp_info.data.n300,
					minute,
					second,
					maxminute,
					maxsecond,
					sp_data.score_info.n300,
					sp_data.score_info.n100,
					sp_data.score_info.n50,
					sp_data.score_info.nmiss,
					sp_data.score_info.combo,
					sp_data.beatmap_info.maxcombo,
					sp_data.pp_info.data.accuracy,
					sp_data.score_info.mods == 0 ? "No Mod"
					: osu_api::v1::api::modParser(sp_data.score_info.mods).c_str(),
					sp_data.score_info.rank.c_str(),
					sp_data.pp_info.data.total_pp.has_value() ? sp_data.pp_info.data.total_pp.value() : 0.0,
					sp_data.fc,
					sp_data.confirmed_acc[4],
					sp_data.confirmed_acc[3],
					sp_data.confirmed_acc[2],
					sp_data.confirmed_acc[0],
					("https://osu.ppy.sh/b/" + to_string(sp_data.beatmap_info.beatmap_id)).c_str());
			}
			else {
				if (sp_data.mode == osu_api::v1::mode::ctb) {
					sprintf_s(
						message,
						5120,
						u8"%s's previous play - %s\n"
						u8"%s - %s[%s]\n"
						"mapped by %s\n"
						"[ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f]\n"
						"Stars:%.2f* | objects:%d | %d:%02d / %d:%02d\n"
						"combo %dx/%dx | acc %.2f%% | %s | %s\n\n%s",
						sp_data.user_info.username.c_str(),
						modeStr.c_str(),
						sp_data.pp_info.data.artistUnicode.length() > 3 ? sp_data.pp_info.data.artistUnicode.c_str()
						: sp_data.pp_info.data.artist.c_str(),
						sp_data.pp_info.data.titleUnicode.length() > 3 ? sp_data.pp_info.data.titleUnicode.c_str()
						: sp_data.pp_info.data.title.c_str(),
						sp_data.pp_info.data.map_version.c_str(),
						sp_data.pp_info.data.creator.c_str(),
						sp_data.pp_info.data.ar,
						sp_data.pp_info.data.od,
						sp_data.pp_info.data.cs,
						sp_data.pp_info.data.hp,
						sp_data.pp_info.data.bpm,
						sp_data.pp_info.data.total_star,
						sp_data.pp_info.data.n300,
						minute,
						second,
						maxminute,
						maxsecond,
						sp_data.score_info.combo,
						sp_data.beatmap_info.maxcombo,
						sp_data.pp_info.data.accuracy,
						sp_data.score_info.mods == 0 ? "No Mod"
						: osu_api::v1::api::modParser(sp_data.score_info.mods).c_str(),
						sp_data.score_info.rank.c_str(),
						("https://osu.ppy.sh/b/" + to_string(sp_data.beatmap_info.beatmap_id)).c_str());
				}
				else {
					sprintf_s(
						message,
						5120,
						u8"%s's previous play - %s\n"
						u8"%s - %s[%s]\n"
						"mapped by %s\n"
						"[ar%.1f | od%.1f | cs%.1f | hp%.1f | bpm%.2f]\n"
						"Stars:%.2f* | objects:%d | %d:%02d / %d:%02d\n"
						"combo %dx | acc %.2f%% | %s | %s\n\n%s",
						sp_data.user_info.username.c_str(),
						modeStr.c_str(),
						sp_data.pp_info.data.artistUnicode.length() > 3 ? sp_data.pp_info.data.artistUnicode.c_str()
						: sp_data.pp_info.data.artist.c_str(),
						sp_data.pp_info.data.titleUnicode.length() > 3 ? sp_data.pp_info.data.titleUnicode.c_str()
						: sp_data.pp_info.data.title.c_str(),
						sp_data.pp_info.data.map_version.c_str(),
						sp_data.pp_info.data.creator.c_str(),
						sp_data.pp_info.data.ar,
						sp_data.pp_info.data.od,
						sp_data.pp_info.data.cs,
						sp_data.pp_info.data.hp,
						sp_data.pp_info.data.bpm,
						sp_data.pp_info.data.total_star,
						sp_data.pp_info.data.n300,
						minute,
						second,
						maxminute,
						maxsecond,
						sp_data.score_info.combo,
						sp_data.pp_info.data.accuracy,
						sp_data.score_info.mods == 0 ? "No Mod"
						: osu_api::v1::api::modParser(sp_data.score_info.mods).c_str(),
						sp_data.score_info.rank.c_str(),
						("https://osu.ppy.sh/b/" + to_string(sp_data.beatmap_info.beatmap_id)).c_str());
				}
			}
			cqhttp_api::send_message(tar, message);
			db.osu_UpdatePPRecord(tar.user_id, sp_data.beatmap_info.beatmap_id);
		}

		static void setmode(const Target tar) {
			string cmd = tar.message.substr(7);
			utils::string_replace(cmd, " ", "");
			if (!utils::isNum(cmd)) {
				cqhttp_api::send_message(tar, 英文模式名提示);
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			if (db.osu_SetUserMainMode(tar.user_id, stoi(cmd)) == 0) {
				cqhttp_api::send_message(tar, u8"在执行操作时发生了错误..请稍后再试。");
				return;
			}
			switch (stoi(cmd)) {
			case 0:
				cqhttp_api::send_message(tar, u8"你的主要游戏模式已设置为Standard.");
				break;
			case 1:
				cqhttp_api::send_message(tar, u8"你的主要游戏模式已设置为Taiko.");
				break;
			case 2:
				cqhttp_api::send_message(tar, u8"你的主要游戏模式已设置为Catch the Beat.");
				break;
			case 3:
				cqhttp_api::send_message(tar, u8"你的主要游戏模式已设置为Mania.");
				break;
			default:
				cqhttp_api::send_message(tar, u8"发生了未知错误");
				break;
			}
		}

		static void bphead_tail(const Target tar) {
			string cmd = tar.message.substr(4);
			utils::string_trim(cmd);
			cmd = utils::unescape(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			int temp;
			osu_api::v1::mode gamemode;
			osu_api::v1::user_info UI = { 0 };
			vector<osu_api::v1::score_info> SI;
			int64_t userid;
			string username = "";
			Database db;
			db.Connect();
			db.addcallcount();
			if (cmd.length() > 0) {
				if (cmd[0] == ':') {
					userid = db.osu_getuserid(tar.user_id);
					if (userid == 0) {
						cqhttp_api::send_message(tar, 用户没有绑定osu账号);
						return;
					}
					else {
						try {
							temp = stoi(cmd.substr(cmd.length() - 1));
							if (temp < 4 && temp > -1) {
								gamemode = (osu_api::v1::mode)temp;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
							}
						}
						catch (std::exception) {
							gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							temp = stoi(cmd.substr(cmd.find(':') + 1));
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (osu_api::v1::mode)temp;
						}
						else {
							gamemode = osu_api::v1::mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
						}
						else {
							username = cmd;
						}
						if (username.empty()) {
							if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::std, &UI) == 0) {
								cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(UI.user_id);
							}
						}
						else {
							if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::std, &UI) == 0) {
								cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(UI.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.osu_getuserid(tar.user_id);
				if (userid == 0) {
					cqhttp_api::send_message(tar, 用户没有绑定osu账号);
					return;
				}
				else {
					gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (osu_api::v1::api::GetUser(userid, gamemode, &UI) == 0) {
					cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
					return;
				}
				if (osu_api::v1::api::GetUserBest(userid, 100, gamemode, SI) == 0) {
					cqhttp_api::send_message(tar, u8"多打一打这个模式再来使用此命令喔~\n?*??(ˊ?ˋ*)??*?");
					return;
				}
			}
			else {
				if (username.length() > 20) {
					cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
					return;
				}
				if (osu_api::v1::api::GetUserBest(username, 100, gamemode, SI) == 0) {
					cqhttp_api::send_message(tar, u8"多打一打这个模式再来使用此命令喔~\n?*??(ˊ?ˋ*)??*?");
					return;
				}
			}
			if (SI.size() < 100) {
				cqhttp_api::send_message(tar, u8"多打一打这个模式再来使用此命令喔~\n?*??(ˊ?ˋ*)??*?");
				return;
			}
			UI.mode = gamemode;
			string modestr;
			switch (UI.mode) {
			case 0:
				modestr = "Standard";
				break;
			case 1:
				modestr = "Taiko";
				break;
			case 2:
				modestr = "Ctb";
				break;
			case 3:
				modestr = "Mania";
				break;
			default:
				modestr = "Unknown";
				break;
			}
			char message[1024];
			double totalvalue = 0.0;
			for (int i = 0; i < 100; ++i) {
				totalvalue = totalvalue + SI[i].pp;
			}
			sprintf_s(message,
				1024,
				u8"%s - 模式 %s\n"
				u8"你的bp1有 %.2f pp，\n"
				u8"你的bp2有 %.2f pp，\n...\n"
				u8"你的bp99有 %.2f pp，\n"
				u8"你的bp100有 %.2f pp，\n"
				u8"你bp1与bp100相差了有 %.2f pp，\n"
				u8"你的bp榜上所有成绩的平均值是 %.2f pp。",
				UI.username.c_str(),
				modestr.c_str(),
				SI[0].pp,
				SI[1].pp,
				SI[98].pp,
				SI[99].pp,
				SI[0].pp - SI[99].pp,
				totalvalue / 100);
			cqhttp_api::send_message(tar, message);
		}

		static void ppvs(const Target tar) {
			string cmd = tar.message.substr(4);
			utils::string_trim(cmd);
			cmd = utils::unescape(cmd);
			if (cmd.length() < 1) {
				cqhttp_api::send_message(tar, u8"请提供要对比的玩家ID");
				return;
			}
			if (cmd.length() > 20) {
				cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
				return;
			}
			osu_api::v1::mode gamemode;
			UserPanelData UI1 = { 0 };
			UserPanelData UI2 = { 0 };
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t UserID = db.osu_getuserid(tar.user_id);
			if (UserID == 0) {
				cqhttp_api::send_message(tar, 用户没有绑定osu账号);
				return;
			}
			if (osu_api::v1::api::GetUser(UserID, osu_api::v1::mode::std, &UI1.user_info) == 0) {
				cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
				return;
			}
			if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::std, &UI2.user_info) == 0) {
				cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
				return;
			}
			float u1pp = db.osu_GetUserPreviousPP(UI1.user_info.user_id);
			float u2pp = db.osu_GetUserPreviousPP(UI2.user_info.user_id);
			UI1.user_info.updatepplus = false;
			UI2.user_info.updatepplus = false;
			//剥离对pp+的依赖
			/*if (u1pp == UI1.user_info.pp) {
				UI1.user_info.updatepplus = false;
				db.GetUserPreviousPPlus(UI1.user_info.user_id, &UI1.pplus_info);
			}
			else {
				UI1.user_info.updatepplus = true;
			}
			if (u2pp == UI2.user_info.pp) {
				UI2.user_info.updatepplus = false;
				db.GetUserPreviousPPlus(UI2.user_info.user_id, &UI2.pplus_info);
			}
			else {
				UI2.user_info.updatepplus = true;
			}*/
			string fileStr = "osucat\\" + ppvsimg(UI1, UI2);
			cqhttp_api::send_message(tar, u8"[CQ:image,file=" + fileStr + u8"]");
			utils::_DelayDelTmpFile(to_string(OC_ROOT_PATH) + "\\data\\images\\" + fileStr);
		}

		static void badgelist(const Target tar) {
			int64_t uid;
			Database db;
			db.Connect();
			db.addcallcount();
			uid = db.osu_getuserid(tar.user_id);
			if (uid == 0) {
				cqhttp_api::send_message(tar, 用户没有绑定osu账号);
				return;
			}
			string message;
			int i;
			vector<int> t = db.osu_GetBadgeList(uid);
			if (t.size() == 0) {
				cqhttp_api::send_message(tar, u8"你还没有获得过奖章。");
				return;
			}
			message = u8"你拥有 " + to_string(t.size()) + u8" 块勋章\n";
			for (i = 0; i < t.size(); ++i) {
				i == t.size() - 1 ? message += db.getBadgeStr(t[i]) : message += db.getBadgeStr(t[i]) + "\n";
			}
			cqhttp_api::send_message(tar, message);
		}

		static void occost(const Target tar) {
			string cmd = utils::unescape(tar.message.substr(6));
			utils::string_trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			Database db;
			db.Connect();
			db.addcallcount();
			string username = "";
			int64_t UserID;
			double a, c, z, p;
			UserPanelData UI = { 0 };
			if (cmd.length() > 1) {
				if (cmd.find("at,qq=") != string::npos) {
					UserID = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
					if (UserID == 0) {
						cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
						return;
					}
				}
				else {
					username = cmd;
				}
			}
			else {
				UserID = db.osu_getuserid(tar.user_id);
				if (UserID == 0) {
					cqhttp_api::send_message(tar, 用户没有绑定osu账号);
					return;
				}
			}
			if (username.empty()) {
				if (osu_api::v1::api::GetUser(UserID, osu_api::v1::mode::std, &UI.user_info) == 0) {
					cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
					return;
				}
			}
			else {
				if (username.length() > 20) {
					cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
					return;
				}
				if (osu_api::v1::api::GetUser(username, osu_api::v1::mode::std, &UI.user_info) == 0) {
					cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
					return;
				}
			}
			// 自PP+停止服务之后 均使用缓存读取内容
			//p = db.GetUserPreviousPP(UI.user_info.user_id);
			//if (UI.user_info.pp != p) {
			//    vector<long> pp_plus;
			//    try {
			//        pp_plus = NetConnection::getUserPlusData(UI.user_info.user_id);
			//        pplus_info pi;
			//        pi.acc = pp_plus[0];
			//        pi.flow = pp_plus[1];
			//        pi.jump = pp_plus[2];
			//        pi.pre = pp_plus[3];
			//        pi.spd = pp_plus[4];
			//        pi.sta = pp_plus[5];
			//        db.UpdatePPlus(UI.user_info.user_id, UI.user_info.pp, pi);
			//        p = UI.user_info.pp;
			//    } catch (std::exception) {
			//    }
			//}
			p = UI.user_info.pp;
			db.osu_GetUserPreviousPPlus(UI.user_info.user_id, &UI.pplus_info);
			z = 1.92 * pow(UI.pplus_info.jump, 0.953) + 69.7 * pow(UI.pplus_info.flow, 0.596)
				+ 0.588 * pow(UI.pplus_info.spd, 1.175) + 3.06 * pow(UI.pplus_info.sta, 0.993);
			a = pow(UI.pplus_info.acc, 1.2768) * pow(p, 0.88213);
			c = min(0.00930973 * pow(p / 1000, 2.64192) * pow(z / 4000, 1.48422), 7) + min(a / 7554280, 3);
			char message[512];
			sprintf_s(message, 512, u8"在猫猫杯S1中，%s 的cost为：%.2f", UI.user_info.username.c_str(), c);
			cqhttp_api::send_message(tar, message);
		}

		static void searchuid(const Target tar) {
			string cmd = utils::unescape(tar.message.substr(9));
			utils::string_trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			Database db;
			db.Connect();
			db.addcallcount();
			string username = "";
			int64_t UserID;
			double a, c, z, p;
			UserPanelData UI = { 0 };
			if (cmd.length() > 1) {
				if (cmd.find("at,qq=") != string::npos) {
					UserID = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
					if (UserID == 0) {
						cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
						return;
					}
				}
				else {
					username = cmd;
				}
			}
			else {
				UserID = db.osu_getuserid(tar.user_id);
				if (UserID == 0) {
					cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
					cqhttp_api::send_message(tar, 用户没有绑定osu账号);
					return;
				}
			}
			if (username.empty()) {
				if (osu_api::v1::api::GetUser(UserID, osu_api::v1::mode::std, &UI.user_info) == 0) {
					cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
					return;
				}
			}
			else {
				if (username.length() > 20) {
					cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
					return;
				}
				if (osu_api::v1::api::GetUser(username, osu_api::v1::mode::std, &UI.user_info) == 0) {
					cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
					return;
				}
			}
			cqhttp_api::send_message(tar, UI.user_info.username + u8" 的osu!uid为 " + to_string(UI.user_info.user_id));
		}

		static void bonuspp(const Target tar) {
			Database db;
			db.Connect();
			db.addcallcount();
			string cmd = utils::unescape(tar.message.substr(7));
			utils::string_trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			utils::string_replace(cmd, "[CQ:", "");
			vector<osu_api::v1::score_info> bp;
			osu_api::v1::user_info UI = { 0 };
			int temp;
			osu_api::v1::mode gamemode;
			int64_t userid;
			string username = "";
			if (cmd.length() > 0) {
				if (cmd[0] == ':') {
					userid = db.osu_getuserid(tar.user_id);
					if (userid == 0) {
						cqhttp_api::send_message(tar, 用户没有绑定osu账号);
						return;
					}
					else {
						try {
							temp = stoi(cmd.substr(cmd.length() - 1));
							if (temp < 4 && temp > -1) {
								gamemode = (osu_api::v1::mode)temp;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
							}
						}
						catch (std::exception) {
							gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
						}
					}
				}
				else if (cmd[0] != ':') {
					if (cmd.find(':') != string::npos) {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
						}
						else {
							try {
								username = cmd.substr(0, cmd.find(':'));
								if (username.length() < 1) username = "%^%^%^!*(^&";
							}
							catch (std::exception) {
								username = "%^%^%^!*(^&";
							}
						}
						try {
							temp = stoi(cmd.substr(cmd.find(':') + 1));
						}
						catch (std::exception) {
							temp = 0;
						}
						if (temp < 4 && temp > -1) {
							gamemode = (osu_api::v1::mode)temp;
						}
						else {
							gamemode = osu_api::v1::mode::std;
						};
					}
					else {
						if (cmd.find("at,qq=") != string::npos) {
							userid = db.osu_getuserid(stoll(utils::GetMiddleText(cmd, "=", "]")));
							if (userid == 0) {
								cqhttp_api::send_message(tar, 被查询的用户未绑定osu账号);
								return;
							}
						}
						else {
							username = cmd;
						}
						if (username.empty()) {
							if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::std, &UI) == 0) {
								cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(UI.user_id);
							}
						}
						else {
							utils::string_trim(cmd);
							if (osu_api::v1::api::GetUser(cmd, osu_api::v1::mode::std, &UI) == 0) {
								cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
								return;
							}
							else {
								gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(UI.user_id);
							}
						}
					}
				}
			}
			else {
				userid = db.osu_getuserid(tar.user_id);
				if (userid == 0) {
					cqhttp_api::send_message(tar, 用户没有绑定osu账号);
					return;
				}
				else {
					gamemode = (osu_api::v1::mode)db.osu_GetUserDefaultGameMode(userid);
				}
			}
			if (username.empty()) {
				if (osu_api::v1::api::GetUser(userid, gamemode, &UI) == 0) {
					cqhttp_api::send_message(tar, 被查询的用户已被bancho封禁);
					return;
				}
				if (osu_api::v1::api::GetUserBest(userid, 100, gamemode, bp) == 0) {
					cqhttp_api::send_message(tar, u8"你在此模式上还没有成绩呢。");
					return;
				}
			}
			else {
				if (username.length() > 20) {
					cqhttp_api::send_message(tar, 所提供的参数超出长度限制);
					return;
				}
				utils::string_trim(username);
				if (osu_api::v1::api::GetUser(username, gamemode, &UI) == 0) {
					cqhttp_api::send_message(tar, 未从bancho检索到要查询的用户信息);
					return;
				}
				if (osu_api::v1::api::GetUserBest(username, 100, gamemode, bp) == 0) {
					cqhttp_api::send_message(tar, u8"ta在此模式上还没有成绩呢。");
					return;
				}
			}
			double extraPolatePP;
			if (bp.size() < 100) {
				extraPolatePP = 0.0;
			}
			double scorepp = 0.0, _bonuspp = 0.0, pp = 0.0, sumOxy = 0.0, sumOx2 = 0.0, avgX = 0.0, avgY = 0.0,
				sumX = 0.0;
			for (int i = 0; i < bp.size(); ++i) {
				scorepp += bp[i].pp * pow(0.95, i);
			}
			vector<double> ys;
			for (int i = 0; i < bp.size(); i++) {
				ys.push_back(log10(bp[i].pp * pow(0.95, i)) / log10(100));
			}
#pragma region calculateLinearRegression
			for (int n = 1; n <= ys.size(); n++) {
				double weight = log1p(n + 1.0);
				sumX += weight;
				avgX += n * weight;
				avgY += ys[n - 1] * weight;
			}
			avgX /= sumX;
			avgY /= sumX;
			for (int n = 1; n <= ys.size(); n++) {
				sumOxy += (n - avgX) * (ys[n - 1] - avgY) * log1p(n + 1.0);
				sumOx2 += pow(n - avgX, 2.0) * log1p(n + 1.0);
			}
			double Oxy = sumOxy / sumX;
			double Ox2 = sumOx2 / sumX;
#pragma endregion
			double b[] = { avgY - (Oxy / Ox2) * avgX, Oxy / Ox2 };

			for (double n = 100; n <= UI.playcount; n++) {
				double val = pow(100.0, b[0] + b[1] * n);
				if (val <= 0.0) {
					break;
				}
				pp += val;
			}
			scorepp = scorepp + pp;
			_bonuspp = UI.pp - scorepp;
			int totalscores = UI.count_a + UI.count_s + UI.count_sh + UI.count_ss + UI.count_ssh;
			bool max;
			if (totalscores >= 25397 || _bonuspp >= 416.6667) {
				max = true;
			}
			else
				max = false;
			int rankedscores =
				max ? max(totalscores, 25397) : (int)round(log10(-(_bonuspp / 416.6667) + 1.0) / log10(0.9994));
			if (_isnan(scorepp) || _isnan(_bonuspp)) {
				scorepp = 0.0;
				_bonuspp = 0.0;
				rankedscores = 0;
			}
			string gamemodeStr;
			switch (gamemode) {
			case osu_api::v1::mode::std:
				gamemodeStr = "osu!Standard";
				break;
			case osu_api::v1::mode::taiko:
				gamemodeStr = "osu!Taiko";
				break;
			case osu_api::v1::mode::ctb:
				gamemodeStr = "osu!Catch the Beat";
				break;
			case osu_api::v1::mode::mania:
				gamemodeStr = "osu!Mania";
				break;
			default:
				gamemodeStr = "error";
				break;
			};
			char rtnmessage[1024];
			sprintf_s(rtnmessage,
				1024,
				u8"%s  (%s)\n总PP：%.2f\n原始PP：%.2f\nBonus PP：%.2f\n共计算出 %d 个被记录的ranked谱面成绩。",
				UI.username.c_str(),
				gamemodeStr.c_str(),
				UI.pp,
				scorepp,
				_bonuspp,
				rankedscores);
			cqhttp_api::send_message(tar, rtnmessage);
		}

		static void setbadge(const Target tar) {
			string cmd = tar.message.substr(8);
			int64_t uid;
			Database db;
			db.Connect();
			db.addcallcount();
			uid = db.osu_getuserid(tar.user_id);
			if (uid == 0) {
				cqhttp_api::send_message(tar, 用户没有绑定osu账号);
				return;
			}
			utils::string_trim(cmd);
			if (!utils::isNum(cmd)) {
				cqhttp_api::send_message(tar, u8"请提供ID");

				return;
			}
			int badgeID;
			try {
				badgeID = stoi(cmd);
			}
			catch (std::exception) {
				badgeID = 65536;
			}
			if (badgeID > 65535) {
				cqhttp_api::send_message(tar, u8"提供的ID有误");
				return;
			}
			badgeSystem::main bsm;
			vector<int> t = db.osu_GetBadgeList(uid);
			if (t.size() < 1) {
				cqhttp_api::send_message(tar, u8"你还没有获得任何奖章呢。");
				return;
			}
			for (int i = 0; i < t.size(); ++i) {
				if (badgeID == t[i]) {
					db.osu_setshownBadges(uid, badgeID);
					Badgeinfo bi = db.osu_getBadgeInfo(badgeID);
					cqhttp_api::send_message(tar, u8"你的主显奖章已设为 " + bi.name);
					return;
				}
			}
			cqhttp_api::send_message(tar, u8"你未拥有此奖章。");
		}

		static void setbanner_v1(const Target tar) {
			string cmd = tar.message.substr(9);
			if (cmd.find("[CQ:image,file=") == string::npos) {
				cqhttp_api::send_message(tar, 自定义Banner帮助);
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t UserID = db.osu_getuserid(tar.user_id);
			if (UserID == 0) {
				cqhttp_api::send_message(tar, 用户没有绑定osu账号);
				return;
			}
			int64_t QQ = db.osu_getqq(UserID);
			string picPath;
			picPath = utils::GetMiddleText(cmd, "[CQ:image,file=", ",url");
			cqhttp_api::PictureInfo picInfo = cqhttp_api::getImage(picPath);
			picPath = ".\\data\\cache\\" + picPath + "." + picInfo.format;
			if (!utils::fileExist(picPath)) {
				cqhttp_api::send_message(tar, 接收图片出错);
				return;
			}
			DrawableList dl;
			Image cover(picPath);
			Image coverRoundrect(Geometry(1200, 350), Color("none"));
			cover.resize(Geometry(1200, 1000000));
			cover.crop(Geometry(1200, 350));
			dl.clear();
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableRoundRectangle(0, 0, 1200, 350, 20, 20));
			coverRoundrect.draw(dl);
			coverRoundrect.composite(cover, 0, 0, InCompositeOp);
			coverRoundrect.quality(100);
			string filepath =
				".\\data\\images\\osucat\\custom\\banner_verify\\" + to_string(UserID) + ".jpg";
			coverRoundrect.write(filepath);
			//baiduaip::imageBase64(filepath);
			cqhttp_api::send_message(tar, 已上传待审核提示);
			cqhttp_api::send_group_message(management_groupid, u8"有一个新的banner被上传，操作者UID：" + to_string(UserID) + u8"\n操作者QQ：" + to_string(QQ)
				+ u8" ,请尽快审核哦。\r\nbanner内容：\r\n"
				+ "[CQ:image,file=" + filepath.substr(14) + "]");
		}

		static void setinfopanel_v1(const Target tar) {
			string cmd = tar.message.substr(12);
			if (cmd.find("[CQ:image,file=") == string::npos) {
				cqhttp_api::send_message(tar, 自定义InfoPanel帮助);
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t UserID = db.osu_getuserid(tar.user_id);
			if (UserID == 0) {
				cqhttp_api::send_message(tar, 用户没有绑定osu账号);
				return;
			}
			int64_t QQ = db.osu_getqq(UserID);
			string picPath;
			picPath = utils::GetMiddleText(cmd, "[CQ:image,file=", ",url");
			cqhttp_api::PictureInfo picInfo = cqhttp_api::getImage(picPath);
			picPath = ".\\data\\cache\\" + picPath + "." + picInfo.format;
			if (!utils::fileExist(picPath)) {
				cqhttp_api::send_message(tar, 接收图片出错);
				return;
			}
			DrawableList dl;
			Image infoPanel(picPath);
			if (infoPanel.size().height() != 857 || infoPanel.size().width() != 1200 || infoPanel.magick() != "PNG") {
				cqhttp_api::send_message(tar, InfoPanel数据错误提示);
				return;
			}
			if (!utils::copyFile(picPath,
				".\\data\\images\\osucat\\custom\\infopanel_verify\\"
				+ to_string(UserID) + ".png")) {
				cqhttp_api::send_message(tar, u8"发生了一个未知错误");
				return;
			}
			cqhttp_api::send_message(tar, 已上传待审核提示);
			cqhttp_api::send_group_message(management_groupid, u8"有一个新的InfoPanel被上传，操作者UID：" + to_string(UserID) + u8"\n操作者QQ：" + to_string(QQ)
				+ u8" ,请尽快审核哦。\r\nInfoPanel内容：\r\n"
				+ "[CQ:image,file=osucat\\custom\\infopanel_verify\\" + to_string(UserID) + ".png]");
		}

		static void resetbanner_v1(const Target tar) {
			Database db;
			db.Connect();
			db.addcallcount();
			string UID = to_string(db.osu_getuserid(tar.user_id));
			string picPath = "./work/v1_cover/" + UID + ".jpg";
			DeleteFileA(picPath.c_str());
			cqhttp_api::send_message(tar, u8"已成功重置你的banner。");
		}

		static void resetinfopanel_v1(const Target tar) {
			Database db;
			db.Connect();
			db.addcallcount();
			string UID = to_string(db.osu_getuserid(tar.user_id));
			string picPath = "./work/v1_infopanel/" + UID + ".png";
			DeleteFileA(picPath.c_str());
			cqhttp_api::send_message(tar, u8"已成功重置你的info面板。");
		}

		static void switchfunction(const Target tar, const GroupSender sdr) {
			string cmd = tar.message.substr(6);
			if (tar.type != Target::Type::GROUP) {
				cqhttp_api::send_message(tar, u8"此操作必须在群内完成。");
				return;
			}
			if (sdr.role == GroupSender::Role::MEMBER) {
				cqhttp_api::send_message(tar, u8"此操作需要管理员执行。");
				return;
			}
			cmd = utils::unescape(cmd);
			utils::string_trim(cmd);
			utils::string_replace(cmd, u8"：", ":");
			Database db;
			db.Connect();
			db.addcallcount();
			vector<string> temp = utils::string_split(cmd, ':');
			string 参数不正确 =
				u8"给定的参数不正确\n正确的参数例：{function}:{on/off}\n参数名：bp/rctpp/recent/entertainment/repeater",
				功能已开启 = u8"这项功能已经是启用状态了", 功能已关闭 = u8"这项功能已经处于禁用状态了",
				已提交 = u8"操作已成功提交。";
			if (temp.size() != 2) {
				cqhttp_api::send_message(tar, 参数不正确);
				return;
			}
			if (temp[0] == "bp") {
				if (temp[1] == "on") {
					if (db.isGroupEnable(tar.group_id, 1) == 1) {
						cqhttp_api::send_message(tar, 功能已开启);
						return;
					}
					db.changeGroupSettings(tar.group_id, 1, true);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				if (temp[1] == "off") {
					if (db.isGroupEnable(tar.group_id, 1) == 0) {
						cqhttp_api::send_message(tar, 功能已关闭);
						return;
					}
					db.changeGroupSettings(tar.group_id, 1, false);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				cqhttp_api::send_message(tar, 参数不正确);
				return;
			}
			if (temp[0] == "rctpp") {
				if (temp[1] == "on") {
					if (db.isGroupEnable(tar.group_id, 2) == 1) {
						cqhttp_api::send_message(tar, 功能已开启);
						return;
					}
					db.changeGroupSettings(tar.group_id, 2, true);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				if (temp[1] == "off") {
					if (db.isGroupEnable(tar.group_id, 2) == 0) {
						cqhttp_api::send_message(tar, 功能已关闭);
						return;
					}
					db.changeGroupSettings(tar.group_id, 2, false);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				cqhttp_api::send_message(tar, 参数不正确);
				return;
			}
			if (temp[0] == "recent") {
				if (temp[1] == "on") {
					if (db.isGroupEnable(tar.group_id, 3) == 1) {
						cqhttp_api::send_message(tar, 功能已开启);
						return;
					}
					db.changeGroupSettings(tar.group_id, 3, true);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				if (temp[1] == "off") {
					if (db.isGroupEnable(tar.group_id, 3) == 0) {
						cqhttp_api::send_message(tar, 功能已关闭);
						return;
					}
					db.changeGroupSettings(tar.group_id, 3, false);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				cqhttp_api::send_message(tar, 参数不正确);
				return;
			}
			if (temp[0] == "entertainment") {
				if (temp[1] == "on") {
					if (db.isGroupEnable(tar.group_id, 4) == 1) {
						cqhttp_api::send_message(tar, 功能已开启);
						return;
					}
					db.changeGroupSettings(tar.group_id, 4, true);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				if (temp[1] == "off") {
					if (db.isGroupEnable(tar.group_id, 4) == 0) {
						cqhttp_api::send_message(tar, 功能已关闭);
						return;
					}
					db.changeGroupSettings(tar.group_id, 4, false);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				cqhttp_api::send_message(tar, 参数不正确);
				return;
			}
			if (temp[0] == "repeater") {
				if (temp[1] == "on") {
					if (db.isGroupEnable(tar.group_id, 5) == 1) {
						cqhttp_api::send_message(tar, 功能已开启);
						return;
					}
					db.changeGroupSettings(tar.group_id, 5, true);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				if (temp[1] == "off") {
					if (db.isGroupEnable(tar.group_id, 5) == 0) {
						cqhttp_api::send_message(tar, 功能已关闭);
						return;
					}
					db.changeGroupSettings(tar.group_id, 5, false);
					cqhttp_api::send_message(tar, 已提交);
					return;
				}
				cqhttp_api::send_message(tar, 参数不正确);
				return;
			}
			cqhttp_api::send_message(tar, 参数不正确);
			return;
		}

		static void getbadgeinfo(const Target tar) {
			string cmd = tar.message.substr(12);
			utils::string_trim(cmd);
			int badge;
			try {
				badge = stoi(cmd);
			}
			catch (std::exception) {
				cqhttp_api::send_message(tar, u8"没有查询到此徽章的任何信息");
				return;
			}
			Database db;
			db.Connect();
			Badgeinfo bi = db.osu_getBadgeInfo(badge);
			if (bi.id != BADGENOTFOUND) {
				cqhttp_api::send_message(tar, "[CQ:image,file=osucat\\badges\\" + to_string(bi.id) + ".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name + u8")\n描述: " + bi.description);
			}
			else {
				cqhttp_api::send_message(tar, u8"没有查询到此徽章的任何信息");
			}
		}

		static void outputcallcount(const Target tar) {
			Database db;
			db.Connect();
			send_message(tar, u8"猫猫从0.4版本开始，主要功能至今一共被调用了 " + to_string(db.Getcallcount()) + u8" 次。");
		}
	};

	class Admin {

	public:

		static void adoptbanner_v1(const Target tar) {
			string UserID = tar.message.substr(11);
			utils::string_trim(UserID);
			string picPath = ".\\data\\images\\osucat\\custom\\banner_verify\\" + UserID + ".jpg";
			if (utils::fileExist(picPath) == true) {
				if (utils::copyFile(
					".\\data\\images\\osucat\\custom\\banner_verify\\" + UserID
					+ ".jpg",
					"./work/v1_cover/" + UserID + ".jpg")
					== true) {
					DeleteFileA(picPath.c_str());
					Database db;
					db.Connect();
					int64_t QQ = db.osu_getqq(stoll(UserID));
					send_private_message(QQ, u8"你上传的Banner通过审核啦，可以使用info指令查看~");
					send_message(tar, u8"ID：" + UserID + u8" ，已成功通知用户Banner已审核成功。");
					if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"ID：" + UserID + u8" ，已成功通知用户Banner已审核成功。"); }
				}
				else {
					send_message(tar, u8"在移动文件时发生了一个错误。");
				}
			}
			else {
				send_message(tar, u8"此用户的内容不在待审核清单内。");
			}
		}

		static void adoptinfopanel_v1(const Target tar) {
			string UserID = tar.message.substr(14);
			utils::string_trim(UserID);
			string picPath =
				".\\data\\images\\osucat\\custom\\infopanel_verify\\" + UserID + ".png";
			if (utils::fileExist(picPath) == true) {
				if (utils::copyFile(
					".\\data\\images\\osucat\\custom\\infopanel_verify\\"
					+ UserID + ".png",
					"./work/v1_infopanel/" + UserID + ".png")
					== true) {
					DeleteFileA(picPath.c_str());
					Database db;
					db.Connect();
					int64_t QQ = db.osu_getqq(stoll(UserID));
					send_private_message(QQ, u8"你上传的Info面板通过审核啦，可以使用info指令查看~");
					send_message(tar, u8"ID：" + UserID + u8" ，已成功通知用户InfoPanel已审核成功。");
					if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"ID：" + UserID + u8" ，已成功通知用户InfoPanel已审核成功。"); }
				}
				else {
					send_message(tar, u8"在移动文件时发生了一个错误。");
				}
			}
			else {
				send_message(tar, u8"此用户的内容不在待审核清单内。");
			}
		}

		static void rejectbanner_v1(const Target tar) {
			string UserID, Content, cmd = tar.message.substr(12);
			if (cmd.find('#') != string::npos) {
				string tmp = cmd;
				utils::string_trim(tmp);
				UserID = tmp.substr(0, tmp.find('#'));
				Content = tmp.substr(tmp.find("#") + 1);
				if (Content.length() < 1) {
					Content = u8"未提供详情。";
				}
			}
			else {
				UserID = cmd;
				utils::string_trim(UserID);
				Content = u8"未提供详情。";
			}
			string picPath = ".\\data\\images\\osucat\\custom\\banner_verify\\" + UserID + ".jpg";
			if (utils::fileExist(picPath) == true) {
				DeleteFileA(picPath.c_str());
				Database db;
				db.Connect();
				int64_t QQ = db.osu_getqq(stoll(UserID));
				send_private_message(QQ, u8"你上传的Banner已被管理员驳回，详情：" + Content);
				send_message(tar, u8"ID：" + UserID + u8" ，已成功通知用户Banner已被驳回。详情：" + Content);
				if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"ID：" + UserID + u8" ，已成功通知用户Banner已被驳回。详情：" + Content); }
			}
			else {
				send_message(tar, u8"此用户的内容不在待审核清单内。");
			}
		}

		static void rejectinfopanel_v1(const Target tar) {
			string UserID, Content, cmd = tar.message.substr(15);
			if (cmd.find('#') != string::npos) {
				string tmp = cmd;
				utils::string_trim(tmp);
				UserID = tmp.substr(0, tmp.find('#'));
				Content = tmp.substr(tmp.find("#") + 1);
				if (Content.length() < 1) {
					Content = u8"未提供详情。";
				}
			}
			else {
				UserID = cmd;
				utils::string_trim(UserID);
				Content = u8"未提供详情。";
			}
			string picPath = ".\\data\\images\\osucat\\custom\\infopanel_verify\\" + UserID + ".png";
			if (utils::fileExist(picPath) == true) {
				DeleteFileA(picPath.c_str());
				Database db;
				db.Connect();
				int64_t QQ = db.osu_getqq(stoll(UserID));
				Target activepushTar;
				send_private_message(QQ, u8"你上传的InfoPanel已被管理员驳回，详情：" + Content);
				send_message(tar, u8"ID：" + UserID + u8" ，已成功通知用户InfoPanel已被驳回。详情：" + Content);
				if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"ID：" + UserID + u8" ，已成功通知用户InfoPanel已被驳回。详情：" + Content); }
			}
			else {
				send_message(tar, u8"此用户的内容不在待审核清单内。");
			}
		}

		static void resetuser(const Target tar) {
			string UserID, Content, cmd = tar.message.substr(9);
			if (cmd.find('#') != string::npos) {
				string tmp = cmd;
				utils::string_trim(tmp);
				UserID = tmp.substr(0, tmp.find('#'));
				Content = tmp.substr(tmp.find("#") + 1);
				if (Content.length() < 1) {
					Content = "未提供详情。";
				}
			}
			else {
				UserID = cmd;
				utils::string_trim(UserID);
				Content = "未提供详情。";
			}
			Database db;
			db.Connect();
			db.addcallcount();
			int64_t QQ = db.osu_getqq(stoll(UserID));
			if (!QQ == 0) {
				try {
					DeleteFileA(("./work/v1_cover/" + UserID + ".jpg").c_str());
				}
				catch (exception) {
				}
				try {
					DeleteFileA(("./work/v1_infopanel/" + UserID + ".jpg").c_str());
				}
				catch (exception) {
				}
				send_private_message(QQ, u8"你的个人资料已被重置，详情：" + Content);
				send_message(tar, "ID：" + UserID + u8" ，已成功通知用户他的个人资料已被重置。详情：" + Content);
				if (tar.group_id != management_groupid) { send_group_message(management_groupid, "ID：" + UserID + u8" ，已成功通知用户他的个人资料已被重置。详情：" + Content); }
			}
			else {
				send_message(tar, u8"找不到此用户。");
			}
		}

		static void addbadge(const Target tar) {
			string args = tar.message.substr(8);
			utils::string_trim(args);
			int64_t uid;
			int badgeid;
			try {
				uid = stoll(args.substr(0, args.find(',')));
			}
			catch (std::exception) {
				send_message(tar, u8"参数错误");
				return;
			}
			try {
				badgeid = stoi(args.substr(args.find(',') + 1));
			}
			catch (std::exception) {
				send_message(tar, u8"参数错误");
				return;
			}
			if (badgeid == BADGENOTFOUND) {
				send_message(tar, u8"未找到此徽章");
				return;
			}
			Database db;
			db.Connect();
			db.addcallcount();
			string temp = "";
			vector<int> tmp = db.osu_GetBadgeList(uid);
			if (tmp.size() > 0) {
				for (size_t i = 0; i < tmp.size(); ++i) {
					temp += to_string(tmp[i]) + ",";
				}
				temp += to_string(badgeid);
			}
			else {
				temp = to_string(badgeid);
			}
			db.osu_addbadge(uid, temp);
			send_message(tar, u8"已成功提交。");
		}

		static void blockuser(const Target tar) {
			string cmd = tar.message.substr(5);
			utils::string_trim(cmd);
			try {
				Database db;
				db.Connect();
				if (db.add_blacklist(stoll(cmd))) {
					send_message(tar, u8"用户 " + cmd + u8" 已成功被列入黑名单");
					if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"用户 " + cmd + u8" 已成功被列入黑名单"); }
				}
				else {
					send_message(tar, u8"用户 " + cmd + u8" 已存在于黑名单中");
				}
			}
			catch (std::exception) {
				send_message(tar, u8"请提供纯数字qq，不要掺杂中文。");
			}
		}

		static void setnewbadge(const Target tar) {
			string cmd = tar.message.substr(11);
			if (cmd.find("CQ:image,file=") != string::npos) {
				vector<string> tmp;
				tmp = utils::string_split(cmd, '#');
				if (tmp.size() != 4) {
					send_message(tar, u8"参数不正确，请按照以下格式提交：\nbadge图像#英文名称#中文名称#详细信息");
					return;
				}
				Badgeinfo bi;
				bi.name = tmp[1];
				bi.name_chinese = tmp[2];
				bi.description = tmp[3];
				Database db;
				db.Connect();
				int* id;
				if (db.osu_setNewBadge(bi, id)) {
					string picPath;
					picPath = utils::GetMiddleText(cmd, "[CQ:image,file=", ",url");
					picPath = picPath.substr(picPath.find(',') + 6);
					PictureInfo p = getImage(picPath);
					picPath = ".\\data\\cache\\" + picPath + "." + p.format;
					if (!utils::copyFile(picPath, ".\\work\\badges\\" + to_string(*id) + ".png")) {
						DeleteFileA((".\\work\\badges\\" + to_string(*id) + ".png").c_str());
						send_message(tar, u8"移动文件失败");
						return;
					}
					if (!utils::copyFile(picPath, ".\\data\\images\\osucat\\badges\\" + to_string(*id) + ".png")) {
						DeleteFileA((".\\data\\images\\osucat\\badges\\" + to_string(*id) + ".png").c_str());
						send_message(tar, u8"移动文件失败");
						return;
					}
					string tmp = u8"已成功提交。\n[CQ:image,file=osucat\\badges\\" + to_string(bi.id) +
						".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name +
						u8")\n描述: " + bi.description;
					send_message(tar, tmp);
					if (tar.group_id != management_groupid) { send_group_message(management_groupid, tmp); }
					return;
				}
				else {
					send_message(tar, u8"添加badge失败");
					return;
				}
			}
			else {
				send_message(tar, u8"请随附badge图像");
			}
		}

		static void updatebadgeinfo(const Target tar) {
			string cmd = tar.message.substr(15);
			utils::string_trim(cmd);
			if (cmd.length() < 3) {
				send_message(tar, u8"请提供参数\n1=英文名称\n2=中文名称\n3=详细信息\n4=badge图像\n格式： 2#徽章ID#中文徽章名称");
				return;
			}
			vector<string>tmp = utils::string_split(cmd, '#');
			if (tmp.size() != 3) {
				send_message(tar, u8"提供了过多的参数！\n1=英文名称\n2=中文名称\n3=详细信息\n4=badge图像\n格式： 2#徽章ID#中文徽章名称");
				return;
			}
			if (!utils::isNum(tmp[0])) {
				send_message(tar, u8"参数1必须为数字！\n1=英文名称\n2=中文名称\n3=详细信息\n4=badge图像\n格式： 2#徽章ID#中文徽章名称");
				return;
			}
			if (!utils::isNum(tmp[1])) {
				send_message(tar, u8"参数2必须为数字！\n1=英文名称\n2=中文名称\n3=详细信息\n4=badge图像\n格式： 2#徽章ID#中文徽章名称");
				return;
			}
			Badgeinfo bi;
			bi.id = stoi(tmp[1]);
			Database db;
			db.Connect();
			if (tmp[0] == "1") {
				bi.name = tmp[2];
				if (db.osu_updatebadgeinfo(bi, 1)) {
					send_message(tar, u8"更新成功\n[CQ:image,file=osucat\\badges\\" + to_string(bi.id) + ".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name + u8")\n描述: " + bi.description);
					if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"badge名称已更新：\n[CQ:image,file=osucat\\badges\\" + to_string(bi.id) + ".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name + u8")\n描述: " + bi.description); }
					return;
				}
				else {
					send_message(tar, u8"更新失败");
					return;
				}
			}
			else if (tmp[0] == "2") {
				bi.name_chinese = tmp[2];
				if (db.osu_updatebadgeinfo(bi, 2)) {
					send_message(tar, u8"更新成功\n[CQ:image,file=osucat\\badges\\" + to_string(bi.id) + ".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name + u8")\n描述: " + bi.description);
					if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"badge名称(中文)已更新：\n[CQ:image,file=osucat\\badges\\" + to_string(bi.id) + ".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name + u8")\n描述: " + bi.description); }
					return;
				}
				else {
					send_message(tar, u8"更新失败");
					return;
				}
			}
			else if (tmp[0] == "3") {
				bi.description = tmp[2];
				if (db.osu_updatebadgeinfo(bi, 3)) {
					send_message(tar, u8"更新成功\n[CQ:image,file=osucat\\badges\\" + to_string(bi.id) + ".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name + u8")\n描述: " + bi.description);
					if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"badge详情已更新：\n[CQ:image,file=osucat\\badges\\" + to_string(bi.id) + ".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name + u8")\n描述: " + bi.description); }
					return;
				}
				else {
					send_message(tar, u8"更新失败");
					return;
				}
			}
			else if (tmp[0] == "4") {
				if (cmd.find("CQ:image,file=") == string::npos) {
					send_message(tar, u8"请随附更改后的badge图像");
					return;
				}
				string picPath = utils::GetMiddleText(cmd, "[CQ:image,file=", ",url");
				PictureInfo p = getImage(picPath);
				if (!utils::copyFile(picPath, ".\\work\\badges\\" + to_string(bi.id) + ".png")) {
					DeleteFileA((".\\work\\badges\\" + to_string(bi.id) + ".png").c_str());
					send_message(tar, u8"移动文件失败");
					return;
				}
				send_message(tar, u8"更新成功\n[CQ:image,file=osucat\\badges\\" + to_string(bi.id) + ".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name + u8")\n描述: " + bi.description);
				if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"badge图像已更新：\n[CQ:image,file=osucat\\badges\\" + to_string(bi.id) + ".png]\nID: " + to_string(bi.id) + u8"\n名称: " + bi.name_chinese + "(" + bi.name + u8")\n描述: " + bi.description); }
			}
		}

		static void removebottle(const Target tar) {
			string returnmsg = "null", cmd = tar.message.substr(8);
			utils::string_trim(cmd);
			utils::string_replace(cmd, u8"；", ";");
			if (cmd.find(";") != string::npos) {
				Database db;
				db.Connect();
				vector<string> splittmp;
				splittmp = utils::string_split(cmd, ';');
				for (int a = 0; a < splittmp.size(); ++a) {
					int i;
					if (splittmp[a].find('#') != string::npos) {
						i = stoi(splittmp[a].substr(0, splittmp[a].find('#')));
						returnmsg = splittmp[a].substr(splittmp[a].find('#') + 1);
					}
					else {
						try { i = stoi(splittmp[a]); }
						catch (std::exception) { send_message(tar, u8"参数错误"); }
					}
					json j = db.getBottleByID(i);
					addons::driftingBottle dfb;
					if (j.size() == 1) {
						dfb.nickname = j[0]["nickname"].get<std::string>();
						dfb.sender = stoll(j[0]["sender"].get<std::string>());
						dfb.sendTime = stoll(j[0]["sendtime"].get<std::string>());
						string tmp;
						returnmsg != "null" ?
							tmp = u8"你发于 " + utils::unixTime2Str(dfb.sendTime) + u8" 的消息已被管理员删除。\n详情：" + returnmsg : tmp = u8"你发于 " + utils::unixTime2Str(dfb.sendTime) + u8" 的消息已被管理员删除。";
						send_private_message(dfb.sender, tmp);
					}
					else { send_message(tar, u8"没有找到这个漂流瓶。"); }
					if (returnmsg == "null") {
						send_message(tar, u8"已移除ID为 " + to_string(i) + u8" 的漂流瓶"); Sleep(2000);
						if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"已移除ID为 " + to_string(i) + u8" 的漂流瓶"); }
					}
					else {
						send_message(tar, u8"已移除ID为 " + to_string(i) + u8" 的漂流瓶，并返回以下消息：" + returnmsg); Sleep(2000);
						if (tar.group_id != management_groupid) { send_group_message(management_groupid, u8"已移除ID为 " + to_string(i) + u8" 的漂流瓶"); }
					}
					db.writeBottle(osucat::addons::driftingBottleDBEvent::DELETEBOTTLE, i, 0, 0, "", "");
				}
			}
			else {
				int i;
				if (cmd.find('#') != string::npos) {
					i = stoi(cmd.substr(0, cmd.find('#')));
					returnmsg = cmd.substr(cmd.find('#') + 1);
				}
				else {
					try { i = stoi(cmd); }
					catch (std::exception) { send_message(tar, u8"参数错误"); return; }
				}
				Database db;
				db.Connect();
				json j = db.getBottleByID(i);
				addons::driftingBottle dfb;
				if (j.size() == 1) {
					dfb.nickname = j[0]["nickname"].get<std::string>();
					dfb.sender = stoll(j[0]["sender"].get<std::string>());
					dfb.sendTime = stoll(j[0]["sendtime"].get<std::string>());
					string tmp;
					returnmsg != "null" ?
						tmp = u8"你发于 " + utils::unixTime2Str(dfb.sendTime) + u8" 的消息已被管理员删除。\n详情：" + returnmsg : tmp = u8"你发于 " + utils::unixTime2Str(dfb.sendTime) + u8" 的消息已被管理员删除。";
					send_private_message(dfb.sender, tmp);
					Sleep(1000);
				}
				else { send_message(tar, u8"没有找到这个漂流瓶。"); return; }
				string tmp;
				returnmsg == "null" ?
					tmp = u8"已移除ID为 " + to_string(i) + u8" 的漂流瓶" : tmp = u8"已移除ID为 " + to_string(i) + u8" 的漂流瓶，并返回以下消息：" + returnmsg;
				send_message(tar, tmp);
				if (tar.group_id != management_groupid) { send_group_message(management_groupid, tmp); }
				db.writeBottle(osucat::addons::driftingBottleDBEvent::DELETEBOTTLE, i, 0, 0, "", "");
			}




		}

		static void cleartemp(const Target tar) {
			string cmd = tar.message; utils::string_trim(cmd);
			if (cmd.length() < 3) { send_message(tar, u8"请提供参数。"); return; }
			if (_stricmp(cmd.substr(0, 3).c_str(), "all") == 0) { system("del .\\work\\avatar\\*.png"); send_message(tar, u8"已成功提交。"); return; }
			if (_stricmp(cmd.substr(0, 6).c_str(), "avatar") == 0) { system("del .\\work\\avatar\\*.png"); send_message(tar, u8"已成功提交。"); return; }
			send_message(tar, u8"参数错误。");
		}
	};

	class System {

	public:

		static void _UpdateManually(Target tar) {
			char dugtmp[256];
			send_message(tar, u8"正在启动更新");
			if (tar.type == Target::Type::GROUP && tar.group_id != management_groupid)send_group_message(management_groupid, u8"管理员 " + to_string(tar.user_id) + u8" 手动发起了每日更新 正在启动更新");
			sprintf_s(dugtmp, u8"[%s] %s[updater]：启动更新", utils::unixTime2Str(time(NULL)).c_str(), output_prefix);
			cout << dugtmp << endl;
			auto start = chrono::system_clock::now();
			Database db;
			db.Connect();
			vector<int64_t> temp = db.GetUserSet();
			db.Close();
			char timeStr[30] = { 0 };
			time_t now = time(NULL);
			tm* tm_now = localtime(&now);
			strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_now);
			for (int i = 0; i < temp.size(); ++i) {
				thread DUP(bind(&dailyUpdatePoster, i, temp[i], timeStr));
				DUP.detach();
				Sleep(3000);
			}
			auto end = chrono::system_clock::now();
			auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
			string message = u8"已完成，一共更新了 " + to_string(temp.size()) + u8" 个用户的数据，共耗时 "
				+ utils::Duration2StrWithoutDAY(double(duration.count()) * chrono::microseconds::period::num
					/ chrono::microseconds::period::den);
			send_message(tar, message);
			if (tar.type == Target::Type::GROUP && tar.group_id != management_groupid)send_group_message(management_groupid, message);
			sprintf_s(dugtmp, u8"[%s] %s[updater]：%s", utils::unixTime2Str(time(NULL)).c_str(), output_prefix, message.c_str());
		}

		static void _DailyUpdate() {
			cout << u8"已成功创建osu!用户数据自动更新线程。" << endl;
			char dugtmp[256];
			while (true) {
				time_t now = time(NULL);
				tm* tm_now = localtime(&now);
				char timeC[16] = { 0 };
				strftime(timeC, sizeof(timeC), "%H", tm_now);
				if (to_string(timeC).find("04") == string::npos) { Sleep(1000 * 60); }
				else {
					send_group_message(management_groupid, u8"正在启动更新");
					try { system("del .\\work\\avatar\\*.png"); }
					catch (std::exception) {}
					sprintf_s(dugtmp, u8"[%s] %s[updater]：启动更新", utils::unixTime2Str(time(NULL)).c_str(), output_prefix);
					cout << dugtmp << endl;
					auto start = chrono::system_clock::now();
					Database db;
					db.Connect();
					vector<int64_t> temp = db.GetUserSet();
					db.Close();
					char timeStr[30] = { 0 };
					strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_now);
					for (int i = 0; i < temp.size(); ++i) {
						thread DUP(bind(&dailyUpdatePoster, i, temp[i], timeStr));
						DUP.detach();
						Sleep(3000);
					}
					auto end = chrono::system_clock::now();
					auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
					string message = u8"已完成，一共更新了 " + to_string(temp.size()) + u8" 个用户的数据，共耗时 "
						+ utils::Duration2StrWithoutDAY(double(duration.count()) * chrono::microseconds::period::num
							/ chrono::microseconds::period::den);
					send_group_message(management_groupid, message);
					sprintf_s(dugtmp, u8"[%s] %s[updater]：%s", utils::unixTime2Str(time(NULL)).c_str(), output_prefix, message.c_str());
					Sleep(1000 * 60 * 60 * 21);
				}
			}
		}

		static void dailyUpdatePoster(int id, int64_t userid, const string& timeStr) {
			osu_api::v1::user_info UI = { 0 };
			Database db;
			db.Connect();
			try {
				try {
					if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::std, &UI) != 0) {
						try {
							db.AddUserData(&UI, timeStr);
						}
						catch (osucat::database_exception) {
						}
					}
				}
				catch (osucat::NetWork_Exception) {
					try {
						if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::std, &UI) != 0) {
							try {
								db.AddUserData(&UI, timeStr);
							}
							catch (osucat::database_exception) {
							}
						}
					}
					catch (osucat::NetWork_Exception) {
						try {
							if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::std, &UI) != 0) {
								try {
									db.AddUserData(&UI, timeStr);
								}
								catch (osucat::database_exception) {
								}
							}
						}
						catch (osucat::NetWork_Exception) {
						}
					}
				}

				try {
					if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::taiko, &UI) != 0) {
						try {
							db.AddUserData(&UI, timeStr);
						}
						catch (osucat::database_exception) {
						}
					}
				}
				catch (osucat::NetWork_Exception) {
					try {
						if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::taiko, &UI) != 0) {
							try {
								db.AddUserData(&UI, timeStr);
							}
							catch (osucat::database_exception) {
							}
						}
					}
					catch (osucat::NetWork_Exception) {
						try {
							if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::taiko, &UI) != 0) {
								try {
									db.AddUserData(&UI, timeStr);
								}
								catch (osucat::database_exception) {
								}
							}
						}
						catch (osucat::NetWork_Exception) {
						}
					}
				}

				try {
					if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::ctb, &UI) != 0) {
						try {
							db.AddUserData(&UI, timeStr);
						}
						catch (osucat::database_exception) {
						}
					}
				}
				catch (osucat::NetWork_Exception) {
					try {
						if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::ctb, &UI) != 0) {
							try {
								db.AddUserData(&UI, timeStr);
							}
							catch (osucat::database_exception) {
							}
						}
					}
					catch (osucat::NetWork_Exception) {
						try {
							if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::ctb, &UI) != 0) {
								try {
									db.AddUserData(&UI, timeStr);
								}
								catch (osucat::database_exception) {
								}
							}
						}
						catch (osucat::NetWork_Exception) {
						}
					}
				}

				try {
					if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::mania, &UI) != 0) {
						try {
							db.AddUserData(&UI, timeStr);
						}
						catch (osucat::database_exception) {
						}
					}
				}
				catch (osucat::NetWork_Exception) {
					try {
						if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::mania, &UI) != 0) {
							try {
								db.AddUserData(&UI, timeStr);
							}
							catch (osucat::database_exception) {
							}
						}
					}
					catch (osucat::NetWork_Exception) {
						try {
							if (osu_api::v1::api::GetUser(userid, osu_api::v1::mode::mania, &UI) != 0) {
								try {
									db.AddUserData(&UI, timeStr);
								}
								catch (osucat::database_exception) {
								}
							}
						}
						catch (osucat::NetWork_Exception) {
						}
					}
				}
			}
			catch (std::exception& ex) {
				dailyUpdatePoster(id, userid, timeStr); return;//出错重试
			}
			char dugtmp[256];
			sprintf_s(dugtmp, u8"[%s] %s[updater]：(No.%d) 已成功更新用户 %lld 的数据。",
				utils::unixTime2Str(time(NULL)).c_str(), output_prefix,
				id,
				userid);
			cout << dugtmp << endl;
		}

		static void _CreateDUThread() {
			cout << u8"正在创建osu!用户数据自动更新线程..." << endl;
			thread dailyUpdateThread(bind(&_DailyUpdate));
			dailyUpdateThread.detach();
		}

		static void _AdminCheck() {
			cout << u8"正在更新管理员列表..." << endl;
			try {
				Database db;
				db.Connect();
				db.reloadAdmin();
			}
			catch (osucat::database_exception& ex) {
				if (ex.Code() == 1065) {
					cout << u8"管理员列表为空，手动设置管理员后重新更新" << endl;
				}
				else {
					cout << u8"无法连接至数据库，请检查数据库设置" << endl;
				}
			}
			cout << u8"已成功更新管理员列表" << endl;
		}

		static void _FolderCheck() {
			cout << u8"正在检查必要目录完整性..." << endl;
			if (!utils::isDirExist(".\\data")) {
				cout << "Folder 'data' does not exist, created." << endl;
				CreateDirectoryA(".\\data", NULL);
			}
			if (!utils::isDirExist(".\\data\\cache")) {
				cout << "Folder 'data\\cache' does not exist, created." << endl;
				CreateDirectoryA(".\\data\\cache", NULL);
			}
			if (!utils::isDirExist(".\\data\\images")) {
				cout << "Folder 'data\\images' does not exist, created." << endl;
				CreateDirectoryA(".\\data\\images", NULL);
			}
			if (!utils::isDirExist(".\\data\\images\\osucat")) {
				cout << "Folder 'data\\images\\osucat' does not exist, created." << endl;
				CreateDirectoryA(".\\data\\images\\osucat", NULL);
			}
			if (!utils::isDirExist(".\\data\\images\\osucat\\help")) {
				cout << "Folder 'data\\images\\osucat\\help' does not exist, created." << endl;
				CreateDirectoryA(".\\data\\images\\osucat\\help", NULL);
			}
			if (!utils::isDirExist(".\\data\\images\\osucat\\custom")) {
				cout << "Folder 'data\\images\\osucat\\custom' does not exist, created." << endl;
				CreateDirectoryA(".\\data\\images\\osucat\\custom", NULL);
			}
			if (!utils::isDirExist(".\\data\\images\\osucat\\custom\\banner_verify")) {
				cout << "Folder 'data\\images\\osucat\\banner_verify' does not exist, created." << endl;
				CreateDirectoryA(".\\data\\images\\osucat\\custom\\banner_verify", NULL);
			}
			if (!utils::isDirExist(".\\data\\images\\osucat\\custom\\infopanel_verify")) {
				cout << "Folder 'data\\images\\osucat\\infopanel_verify' does not exist, created." << endl;
				CreateDirectoryA(".\\data\\images\\osucat\\custom\\infopanel_verify", NULL);
			}
			if (!utils::isDirExist(".\\work")) {
				cout << "Folder 'work' does not exist, created." << endl;
				CreateDirectoryA(".\\work", NULL);
			}
			if (!utils::isDirExist(".\\work\\avatar")) {
				cout << "Folder 'work\\avatar' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\avatar", NULL);
			}
			if (!utils::isDirExist(".\\work\\background")) {
				cout << "Folder 'work\\background' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\background", NULL);
			}
			if (!utils::isDirExist(".\\work\\badges")) {
				cout << "Folder 'work\\badges' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\badges", NULL);
			}
			if (!utils::isDirExist(".\\work\\flags")) {
				cout << "Folder 'work\\flags' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\flags", NULL);
			}
			if (!utils::isDirExist(".\\work\\fonts")) {
				cout << "Folder 'fonts' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\fonts", NULL);
			}
			if (!utils::isDirExist(".\\work\\icons")) {
				cout << "Folder 'work\\icon' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\icons", NULL);
			}
			if (!utils::isDirExist(".\\work\\mode_icon")) {
				cout << "Folder 'work\\mode_icon' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\mode_icon", NULL);
			}
			if (!utils::isDirExist(".\\work\\mods")) {
				cout << "Folder 'work\\mods' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\mods", NULL);
			}
			if (!utils::isDirExist(".\\work\\ranking")) {
				cout << "Folder 'work\\ranking' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\ranking", NULL);
			}
			if (!utils::isDirExist(".\\work\\v1_cover")) {
				cout << "Folder 'work\\v1_cover' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\v1_cover", NULL);
			}
			if (!utils::isDirExist(".\\work\\v1_infopanel")) {
				cout << "Folder 'work\\v1_infopanel' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\v1_infopanel", NULL);
			}
			if (!utils::isDirExist(".\\work\\v2_background")) {
				cout << "Folder 'work\\v2_background' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\v2_background", NULL);
			}
			if (!utils::isDirExist(".\\work\\v2_infopanel")) {
				cout << "Folder 'work\\v2_infopanel' does not exist, created." << endl;
				CreateDirectoryA(".\\work\\v2_infopanel", NULL);
			}
		}

		static void _ConfigCheck() {
			ifstream configfile(".\\osucat_config.json");
			string config;
			if (configfile) {
				json j;
				try {
					ostringstream tmp;
					tmp << configfile.rdbuf();
					config = tmp.str();
					configfile.close();
					j = json::parse(config);
				}
				catch (std::exception& ex) {
					cout << ex.what() << endl;
					system("pause");
					exit(-1);
				}
				if (j["owner_userid"].get<string>() == "") { cout << u8"未找到配置项：owner user_id!" << endl; system("pause"); exit(-1001); }
				else owner_userid = stoll(j["owner_userid"].get<string>());
				if (j["management_groupid"].get<string>() == "") { cout << u8"未找到配置项：management_groupid" << endl; system("pause"); exit(-1001); }
				else management_groupid = stoll(j["management_groupid"].get<string>());
				if (j["osu_web_apikey"].get<string>() == "") { cout << u8"未找到配置项：osu_web_apikey" << endl; system("pause"); exit(-1001); }
				else sprintf_s(OC_OSU_API_KEY, "%s", j["osu_web_apikey"].get<string>().c_str());
				if (j["steam_web_apikey"].get<string>() == "") { cout << u8"未找到配置项：steam_web_apikey" << endl; system("pause"); exit(-1001); }
				else sprintf_s(SC_STEAM_API_KEY, "%s", j["steam_web_apikey"].get<string>().c_str());
				if (!utils::fileExist(".\\go-cqhttp.exe")) { cout << "Missing go-cqhttp file." << endl; cout << "Get from https://github.com/Mrs4s/go-cqhttp/releases" << endl; system("pause"); exit(-1003); }
				if (j["oc_server_host"].get<string>() == "") { cout << u8"未找到配置项：oc_server_host" << endl; system("pause"); exit(-1002); }
				else sprintf_s(OC_SERVER_HOST, "%s", j["oc_server_host"].get<string>().c_str());
				if (j["oc_server_port"].is_null()) { cout << u8"未找到配置项：oc_server_port" << endl; system("pause"); exit(-1002); }
				else OC_SERVER_PORT = j["oc_server_port"].get<int>();
				if (j["sql_user"].get<string>() == "") { cout << u8"未找到配置项：sql_user" << endl; system("pause"); exit(-1002); }
				else sprintf_s(OC_SQL_USER, "%s", j["sql_user"].get<string>().c_str());
				if (j["sql_host"].get<string>() == "") { cout << u8"未找到配置项：sql_host" << endl; system("pause"); exit(-1002); }
				else sprintf_s(OC_SQL_HOST, "%s", j["sql_host"].get<string>().c_str());
				if (j["sql_password"].get<string>() == "") { cout << u8"未找到配置项：sql_password" << endl; system("pause"); exit(-1002); }
				else sprintf_s(OC_SQL_PASSWORD, "%s", j["sql_password"].get<string>().c_str());
				if (j["sql_database"].get<string>() == "") { cout << u8"未找到配置项：sql_database" << endl; system("pause"); exit(-1002); }
				else sprintf_s(OC_SQL_DATABASE, "%s", j["sql_database"].get<string>().c_str());
				if (j["sql_port"].is_null()) { cout << u8"未找到配置项：sql_port" << endl; system("pause"); exit(-1002); }
				else OC_SQL_PORT = j["sql_port"].get<int>();
				if (j["oc_output_prefix"].is_null()) { cout << u8"未找到配置项：oc_output_prefix ，将使用默认配置项" << endl; sprintf_s(output_prefix, "%s", "[osucat]"); }
				else sprintf_s(output_prefix, "%s", j["oc_output_prefix"].get<string>());
			}
			else {
				cout << "Missing config file.\nInitializing config file..." << endl;
				ofstream writeconfig(".\\osucat_config.json");
				if (!writeconfig) {
					cout << "Unable to create config file.";
					exit(-1001);
				}
				json j;
				j["osu_web_apikey"] = "";
				j["steam_web_apikey"] = "";
				j["debugmode"] = false;
				j["oc_output_prefix"] = "[osucat]";
				j["oc_server_host"] = "127.0.0.1";
				j["oc_server_port"] = 6700;
				j["sql_host"] = "127.0.0.1";
				j["sql_user"] = "";
				j["sql_password"] = "";
				j["sql_database"] = "";
				j["sql_port"] = 3306;
				j["owner_userid"] = "";
				j["management_groupid"] = "";
				writeconfig << j.dump().c_str() << endl;
				writeconfig.close();
				cout << u8"配置文件已成功创建，稍后请请完成设置。";
				system("pause");
				exit(0);
			}
		}

	};

	class CmdParser {
	public:
		static void parser(Target tar, const GroupSender sdr) {
			if (tar.message[0] == '!' || tar.message.find(u8"！") == 0) {
				tar.message = tar.message[0] < 0 ? tar.message.substr(3) : tar.message.substr(1); //提前处理掉前缀
				try {
					Database db;
					db.Connect();
					if (db.is_Blocked(tar.user_id) == 1) return; //在黑名单内的用户被忽略
					if (_stricmp(tar.message.substr(0, 18).c_str(), u8"猫猫调用次数") == 0) {
						Main::outputcallcount(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 6).c_str(), "recent") == 0) {
						if (tar.type == Target::Type::GROUP) { if (db.isGroupEnable(tar.group_id, 3) == 0) return; }
						Main::recent(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 2).c_str(), "pr") == 0) {
						if (tar.type == Target::Type::GROUP) { if (db.isGroupEnable(tar.group_id, 3) == 0) return; }
						Main::recent(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 4).c_str(), "help") == 0) {
						Main::help(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 5).c_str(), "about") == 0) {
						Main::about(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 7).c_str(), "contact") == 0) {
						Main::contact(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 4).c_str(), "ping") == 0) {
						Main::ping(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 5).c_str(), "setid") == 0) {
						Main::setid(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 5).c_str(), "unset") == 0) {
						Main::unsetid(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 6).c_str(), u8"因佛") == 0) {
						Main::info(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 4).c_str(), "info") == 0) {
						Main::info(tar); return;
					}
					/*if ((_stricmp(tar.message.substr(0, 8).c_str(), "textinfo") == 0) || (_stricmp(tar.message.substr(0, 2).c_str(), "ti") == 0)) {
						if (_stricmp(msg.substr(0, 2).c_str(), "ti") == 0) {
							textinfo(msg.substr(2), tar, params);
						}
						else {
							textinfo(msg.substr(8), tar, params);
						}
						return true;
					}*/
					if (_stricmp(tar.message.substr(0, 4).c_str(), "bpme") == 0) { return; }
					if (_stricmp(tar.message.substr(0, 4).c_str(), "bpht") == 0) {
						Main::bphead_tail(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 2).c_str(), "bp") == 0) {
						if (tar.type == Target::Type::GROUP) { if (db.isGroupEnable(tar.group_id, 1) == 0) return; }
						Main::bp(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 5).c_str(), "score") == 0) {
						Main::score(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 6).c_str(), "update") == 0) {
						Main::update(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 7).c_str(), "setmode") == 0) {
						Main::setmode(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 5).c_str(), "rctpp") == 0) {
						if (tar.type == Target::Type::GROUP) { if (db.isGroupEnable(tar.group_id, 2) == 0) return; }
						Main::rctpp(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 4).c_str(), "ppvs") == 0) {
						Main::ppvs(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 2).c_str(), "pp") == 0) {
						Main::pp(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 4).c_str(), "with") == 0) {
						Main::ppwith(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 9).c_str(), "badgelist") == 0) {
						Main::badgelist(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 9).c_str(), "setbanner") == 0) {
						Main::setbanner_v1(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 12).c_str(), "setinfopanel") == 0) {
						Main::setinfopanel_v1(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 11).c_str(), "resetbanner") == 0) {
						Main::resetbanner_v1(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 14).c_str(), "resetinfopanel") == 0) {
						Main::resetinfopanel_v1(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 6).c_str(), "occost") == 0) {
						Main::occost(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 7).c_str(), "bonuspp") == 0) {
						Main::bonuspp(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 9).c_str(), "badgelist") == 0) {
						Main::badgelist(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 8).c_str(), "setbadge") == 0) {
						Main::setbadge(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 9).c_str(), "searchuid") == 0) {
						Main::searchuid(tar); return;
					}
					if (_stricmp(tar.message.substr(0, 6).c_str(), "switch") == 0) {
						Main::switchfunction(tar, sdr); return;
					}
					if (_stricmp(tar.message.substr(0, 12).c_str(), "getbadgeinfo") == 0) {
						Main::getbadgeinfo(tar); return;
					}
					for (int fi = 0; fi < adminlist.size(); ++fi) {
						if (tar.user_id == adminlist[fi].user_id) {
							if (_stricmp(tar.message.substr(0, 11).c_str(), "adoptbanner") == 0) {
								Admin::adoptbanner_v1(tar); return;
							}
							if (_stricmp(tar.message.substr(0, 14).c_str(), "adoptinfopanel") == 0) {
								Admin::adoptinfopanel_v1(tar); return;
							}
							if (_stricmp(tar.message.substr(0, 9).c_str(), "cleartemp") == 0) {
								if (adminlist[fi].role != 1) { send_message(tar, u8"权限不足"); return; }
								Admin::cleartemp(tar); return;
							}
							if (_stricmp(tar.message.substr(0, 12).c_str(), "rejectbanner") == 0) {
								Admin::rejectbanner_v1(tar); return;
							}
							if (_stricmp(tar.message.substr(0, 15).c_str(), "rejectinfopanel") == 0) {
								Admin::rejectinfopanel_v1(tar); return;
							}
							if (_stricmp(tar.message.substr(0, 9).c_str(), "resetuser") == 0) {
								if (adminlist[fi].role != 1) { send_message(tar, u8"权限不足"); return; }
								Admin::resetuser(tar); return;
							}
							if (_stricmp(tar.message.substr(0, 8).c_str(), "addbadge") == 0) {
								if (adminlist[fi].role != 1) { send_message(tar, u8"权限不足"); return; }
								Admin::addbadge(tar); return;
							}
							if (_stricmp(tar.message.substr(0, 11).c_str(), "dailyupdate") == 0) {
								if (adminlist[fi].role != 1) { send_message(tar, u8"权限不足"); return; }
								System::_UpdateManually(tar);
								return;
							}
							if (_stricmp(tar.message.substr(0, 5).c_str(), "block") == 0) {
								Admin::blockuser(tar); return;
							}
							if (_stricmp(tar.message.substr(0, 8).c_str(), "rmbottle") == 0) {
								Admin::removebottle(tar); return;
							}
							if (_stricmp(tar.message.substr(0, 11).c_str(), "setnewbadge") == 0) {
								if (adminlist[fi].role != 1) { send_message(tar, u8"权限不足"); return; }
								Admin::setnewbadge(tar); return;
							}
						}
					}
					if (_stricmp(tar.message.substr(0, 11).c_str(), "reloadadmin") == 0) {
						if (tar.user_id != owner_userid) { return; }
						try { db.reloadAdmin(); send_message(tar, u8"管理员列表已更新。"); }
						catch (osucat::database_exception) { send_message(tar, u8"更新失败."); }
					}
					steamcat::cmdParser::parse(tar, sdr); //steamcat
					if (tar.type == Target::Type::GROUP) if (db.isGroupEnable(tar.group_id, 4) == 0) return; //拦截娱乐模块
					osucat::addons::CmdParser::parser(tar, sdr);
				}
				catch (osucat::database_exception& ex) {
					send_message(tar, u8"访问数据库时出现了一个错误，请稍后重试...");
					char reportMsg[1024];
					sprintf_s(reportMsg,
						"[%s]\n"
						u8"Mysql出现错误\n"
						u8"错误代码：%d\n"
						u8"详细信息：%s\n",
						utils::unixTime2Str(time(NULL)).c_str(),
						ex.Code(),
						ex.Info().c_str()
					);
					send_private_message(owner_userid, reportMsg);
				}
				catch (osucat::NetWork_Exception& ex) {
					send_message(tar, u8"访问api时超时...请稍后重试...");
				}
				catch (std::exception& ex) {
					send_message(tar, u8"出现了一个未知错误，请稍后重试...");
					char reportMsg[1024];
					sprintf_s(reportMsg,
						"[%s]\n"
						u8"已捕获std::exception\n"
						u8"操作者：%lld\n"
						u8"触发指令：%s\n"
						u8"详细信息：%s\n",
						utils::unixTime2Str(time(NULL)).c_str(),
						tar.user_id,
						tar.message.c_str(),
						ex.what()
					);
					send_private_message(owner_userid, reportMsg);
				}
			}
		}
	};

	void echo(const Target tar, const GroupSender sdr) {
		send_message(tar, tar.message);
	}

}

#endif