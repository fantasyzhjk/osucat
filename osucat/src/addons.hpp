#pragma once
#ifndef OC_ADDONS_HPP
#define OC_ADDONS_HPP

#include "games/hangman.hpp"

namespace osucat::addons {

	struct driftingBottle
	{
		int id;
		int pickcount;
		string msg;
		int64_t sender;
		string nickname;
		int sendTime;
	};

	class Main {
	public:
		static void imagemonitor(const GroupSender sdr, const Target tar) {
			if (tar.message.find("[CQ:image") != string::npos) {
				char reportMsg[1024];
				sprintf_s(reportMsg,
					"[%s]\n"
					u8"用户 %s(%lld) 上传了图片,消息内容如下：\n%s",
					utils::unixTime2Str(time(NULL)).c_str(),
					sdr.nickname.c_str(),
					tar.user_id,
					tar.message.c_str()
				);
				send_group_message(management_groupid, reportMsg);
			}
		}

		static void roll(const Target tar) {
			string cmd = tar.message.substr(4);
			cmd = utils::cqunescape(cmd);
			if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"不想理你..."); return; }
			if (cmd.length() > 199) { send_message(tar, u8"你想干嘛..."); return; }
			utils::string_trim(cmd);
			int64_t value;
			if (cmd.empty()) { send_message(tar, to_string(utils::randomNum(1, 100))); return; }
			if (utils::isNum(cmd)) {
				try {
					value = stoll(cmd);
					if (value < 1) value = 100;
				}
				catch (std::exception) {
					value = 100;
				}
				send_message(tar, to_string(utils::randomNum(1, value)));
				return;
			}
			vector<string> temp = utils::string_split(cmd, ' ');
			if (temp.size() > 1) {
				send_message(tar, u8"当然是" + temp[utils::randomNum(1, (int)temp.size()) - 1] + u8"咯");
			}
			else
				send_message(tar, u8"需要两个或以上的参数啦..你给的不够喵...");
		}

		static void chp(const Target tar) {
			send_message(tar, NetConnection::HttpsGet("https://chp.shadiao.app/api.php"));
		}

		static void sleep(const Target tar) {
			switch (utils::randomNum(1, 4)) {
			case 1:
				send_message(tar, u8"[CQ:record,file=osucat\\你怎么睡得着的.mp3]");
				break;
			case 2:
				send_message(tar, u8"[CQ:record,file=osucat\\睡不着啊 硬邦邦.mp3]");
				break;
			case 3:
				send_message(tar, u8"[CQ:record,file=osucat\\丁丁-睡不着啊.mp3]");
				break;
			case 4:
				send_message(tar, u8"[CQ:record,file=osucat\\小乐乐-睡觉啦不要聊天啦.mp3]");
				break;
			default:
				break;
			}
		}

		static void marketingGenerator(const Target tar) {
			string cmd = tar.message.substr(18);
			cmd = utils::cqunescape(cmd);
			if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"不想理你..."); return; }
			if (cmd.length() > 400) { send_message(tar, u8"太长了！"); return; }
			utils::string_trim(cmd);
			std::vector<std::string> temp = utils::string_split(cmd, '#');
			if (temp.size() != 3) { send_message(tar, u8"参数有误，生成失败。\n参数格式为 主体#事件#事件的另一种说法"); return; }
			std::string 主体 = temp[0], 事件 = temp[1], 事件的另一种说法 = temp[2];
			char message[3072];
			sprintf_s(message,
				3072,
				u8"%s%s是怎么回事呢？%s相信大家都很熟悉，但是%s%s"
				u8"是怎么回事呢，下面就让小编带大家一起了解下吧。"
				u8"%s%s，其实就是%s，大家可能会很惊讶%s怎么"
				u8"%s呢？但事实就是这样，小编也感到非常惊讶。"
				u8"这就是关于%s%s的事情了，大家有什么想法呢，欢迎在评论区和小编一起讨论哦~",
				主体.c_str(),
				事件.c_str(),
				主体.c_str(),
				主体.c_str(),
				事件.c_str(),
				主体.c_str(),
				事件.c_str(),
				事件的另一种说法.c_str(),
				主体.c_str(),
				事件.c_str(),
				主体.c_str(),
				事件.c_str());
			send_message(tar, message);
		}

		static void randEvents(const Target tar) {
			string cmd = tar.message;
			try {
				cmd = utils::cqunescape(cmd);
				if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"不想理你..."); return; }
				if (cmd.length() > 1024) { send_message(tar, u8"你想干嘛..."); return; }
				utils::string_trim(cmd);
				utils::string_replace(cmd, u8"我", u8"{@@}");
				utils::string_replace(cmd, u8"你", u8"我");
				utils::string_replace(cmd, u8"{@@}", u8"你");
				if (cmd.substr(cmd.length() - 3, 3) == u8"？" || cmd.substr(cmd.length() - 1, 1) == "?")
					cmd = cmd.substr(cmd.length() - 3, 3) == u8"？" ? cmd.substr(0, cmd.length() - 3)
					: cmd.substr(0, cmd.length() - 1);
				if (cmd.substr(cmd.length() - 3, 3) == "吗" || cmd.substr(cmd.length() - 3, 3) == "呢")
					cmd = cmd.substr(0, cmd.length() - 3);
				if (cmd.find(u8"还是") != string::npos) {
					string str1, str2;
					str1 = cmd.substr(0, cmd.find(u8"还是"));
					str2 = cmd.substr(cmd.find(u8"还是") + 6);
					if (str1.empty() || str2.empty()) return;
					switch (utils::randomNum(1, 2)) {
					case 1:
						send_message(tar, str1);
						break;
					case 2:
						send_message(tar, str2);
						break;
					}
					return;
				}
				if (cmd.find(u8"不") != string::npos) {
					if ((cmd[cmd.find(u8"不") - 1] & 0x80) != 0) {
						//是汉字
						if (cmd.substr(cmd.find(u8"不") - 3, 3) == cmd.substr(cmd.find(u8"不") + 3, 3)) {
							//do something here
							string str1 = cmd.substr(0, cmd.find(u8"不") - 3), str2 = cmd.substr(cmd.find(u8"不") + 3);
							switch (utils::randomNum(1, 2)) {
							case 1:
								send_message(tar, str1 + str2);
								break;
							case 2:
								send_message(tar, str1 + u8"不" + str2);
								break;
							}
						}
					}
					else {
						//不是汉字
						if (cmd.substr(0, cmd.find(u8"不")) == cmd.substr(cmd.find(u8"不") + 3)) {
							//do something here
							string str1 = cmd.substr(0, cmd.find(u8"不")), str2 = cmd.substr(cmd.find(u8"不") + 3);
							switch (utils::randomNum(1, 2)) {
							case 1:
								send_message(tar, str2);
								break;
							case 2:
								send_message(tar, u8"不" + str2);
								break;
							}
						}
					}
				}
			}
			catch (std::exception& ex) {
				cout << ex.what() << endl;
			}
		}

		static void nbnhhsh(const Target tar) {
			string cmd = tar.message.substr(7);
			cmd = utils::cqunescape(cmd);
			utils::string_trim(cmd);
			if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"不想理你..."); return; }
			if (cmd.length() > 199) { send_message(tar, u8"太长了！"); return; }
			if (cmd == "") { send_message(tar, u8"你都不告诉我你要翻译啥（恼）"); return; }
			json jp;
			jp["text"] = cmd;
			try {
				string tmp1, tmp2 = NetConnection::HttpsPost("https://lab.magiconch.com/api/nbnhhsh/guess", jp).substr(1);
				tmp2 = tmp2.substr(0, tmp2.length() - 1);
				try {
					json j = json::parse(tmp2)["trans"];
					for (int ii = 1, i = 0; i < j.size(); ++i) {
						tmp2 = "\n[" + to_string(ii) + "] " + j[i].get<string>();
						if (!utils::forbiddenWordsLibrary(j[i].get<string>())) {
							if (ii < 10) {
								tmp1 += "\n/" + to_string(ii) + "/  " + j[i].get<string>();
							}
							else if (ii > 10) {
								break;
							}
							else {
								tmp1 += "\n/" + to_string(ii) + "/ " + j[i].get<string>();
							}
							++ii;
						}
					}
					if (tmp1 != "") {
						char trntmp[8192];
						sprintf_s(trntmp,
							u8"以下是\"%s\"的人话：%s",
							cmd.c_str(),
							tmp1.c_str());
						send_message(tar, trntmp);
					}
					else { send_message(tar, u8"没有找到这堆缩写的翻译;w;"); }
				}
				catch (json::exception) { send_message(tar, u8"没有找到这堆缩写的翻译;w;"); }
			}
			catch (osucat::NetWork_Exception) { send_message(tar, u8"获取翻译时超时...请稍后再试..."); }
		}

		static void tgrj(const Target tar) {
			send_message(tar, NetConnection::HttpsGet("https://api.moebot.im/tg/"));
		}

		static void driftingBottleVoid(bool ThrowOrPick, const Target tar, const GroupSender senderinfo) {
			string cmd = tar.message.substr(12);
			Database db;
			db.Connect();
			json j = db.getBottles();
			if (ThrowOrPick) {
				int throwcount = 0;
				for (int i = 0; i < j.size(); ++i) { if (stoll(j[i]["sender"].get<std::string>()) == tar.user_id) { ++throwcount; } }
				if (throwcount > 20) { send_message(tar, u8"你已经扔了20个瓶子出去了...休息一下再扔吧...");				return; }
				cmd = utils::cqunescape(cmd);
				utils::string_trim(cmd);
				if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"不想理你...");			return; }
				if (cmd.length() == 0) { send_message(tar, u8"不如写点什么再扔" + utils::random_numofchars(u8".", 3, 8) + u8"?");					return; }
				if (cmd.length() > 5000) { send_message(tar, u8"太长了！"); return; }
				time_t timetmp = time(NULL);
				db.setBottleRemaining(2, tar.user_id);
				db.writeBottle(driftingBottleDBEvent::WRITEIN, 0, timetmp, tar.user_id, senderinfo.nickname, utils::ocescape(cmd));
				db.addPickThrowCount(false);
				char reportMsg[6000];
				string s_bottle = utils::randomNum(1, 100) > 50 ? +u8"漂流瓶" : +u8"瓶子";
				Sleep(100);
				string s_yours = utils::randomNum(1, 100) > 50 ? +u8"你的" : +u8"君の";
				Sleep(100);
				string s_sendout = utils::randomNum(1, 100) > 50 ? +u8"漂往远方" : +u8"流向远方";
				if (cmd.find("[CQ:image") != string::npos) {
					send_message(tar, s_yours + s_bottle + u8"已经" + s_sendout + utils::random_numofchars(u8".", 3, 8));
					sprintf_s(reportMsg,
						"[%s]\n"
						u8"用户 %s(%lld) 在漂流瓶内上传了图片\n漂流瓶ID: %d\n消息内容如下：\n%s",
						utils::unixTime2Str(time(NULL)).c_str(),
						senderinfo.nickname.c_str(),
						tar.user_id,
						db.getBottleID(tar.user_id, utils::ocescape(cmd), timetmp),
						tar.message.c_str()
					);
					send_group_message(management_groupid, reportMsg);
				}
				else {
					send_message(tar, s_yours + s_bottle + u8"已经" + s_sendout + utils::random_numofchars(u8".", 3, 8));
					sprintf_s(reportMsg,
						"[%s]\n"
						u8"用户 %s(%lld) 上传了漂流瓶\n漂流瓶ID: %d\n消息内容如下：\n%s",
						utils::unixTime2Str(time(NULL)).c_str(),
						senderinfo.nickname.c_str(),
						tar.user_id,
						db.getBottleID(tar.user_id, utils::ocescape(cmd), timetmp),
						tar.message.c_str()
					);
					send_group_message(management_groupid, reportMsg);
				}
			}
			else {
				int a = db.getUserBottleRemaining(tar.user_id);
				if (a > 0) {
					if (j.size() != 0) {
						string s_contentis = utils::randomNum(1, 100) > 50 ? +u8"瓶子里的内容是" : +u8"瓶の中身は";
						db.setBottleRemaining(3, tar.user_id);
						int tempi = utils::randomNum(0, j.size() - 1);
						for (int i = 0; i < j.size(); ++i) {
							if (stoll(j[tempi]["sender"].get<std::string>()) != tar.user_id) {
								break;
							}
							else { Sleep(314); tempi = utils::randomNum(0, j.size() - 1); }
						}
						driftingBottle dfb;
						dfb.msg = utils::ocunescape(j[tempi]["message"].get<std::string>());
						dfb.nickname = j[tempi]["nickname"].get<std::string>();
						dfb.sender = stoll(j[tempi]["sender"].get<std::string>());
						dfb.sendTime = stoi(j[tempi]["sendtime"].get<std::string>());
						dfb.id = stoi(j[tempi]["id"].get<std::string>());
						dfb.pickcount = stoi(j[tempi]["pickcount"].get<std::string>());
						db.addPickThrowCount(true);
						db.writeBottle(osucat::addons::driftingBottleDBEvent::ADDCOUNTER, dfb.id, 0, 0, "", "");
						char tempm[6000];
						sprintf_s(tempm,
							u8"这是来自 %s(%lld) 的漂流瓶%s\n"
							u8"ID:%d 发于 %s\n"
							u8"%s%s\n%s",
							dfb.nickname.c_str(),
							dfb.sender, utils::random_numofchars(u8".", 3, 8).c_str(), dfb.id,
							utils::unixTime2StrChinese(dfb.sendTime).c_str(), s_contentis.c_str(), utils::random_numofchars(u8".", 3, 8).c_str(),
							dfb.msg.c_str());
						send_message(tar, tempm);
						if (db.RemoveBottle(floor((time(NULL) - stoll(j[tempi]["sendtime"].get<std::string>())) / 86400), dfb.id)) {
							if (dfb.pickcount == 0) {
								sprintf_s(tempm,
									u8"你发于 %s\n"
									u8"的内容为%s%s的消息已经被 %s(%lld) 捞起来了%s\n",
									utils::unixTime2StrChinese(dfb.sendTime).c_str(),
									utils::random_numofchars(u8".", 3, 8).c_str(), dfb.msg.c_str(),
									senderinfo.nickname.c_str(), tar.user_id, utils::random_numofchars(u8".", 3, 8).c_str());
							}
							else {
								sprintf_s(tempm,
									u8"你发于 %s\n"
									u8"的内容为%s%s的消息已经被 %s(%lld) 捞起来了....\n在此之前你的瓶子还被阅读了 %d 次%s",
									utils::unixTime2StrChinese(dfb.sendTime).c_str(),
									utils::random_numofchars(u8".", 3, 8).c_str(), dfb.msg.c_str(),
									senderinfo.nickname.c_str(), tar.user_id, dfb.pickcount,
									utils::random_numofchars(u8".", 3, 8).c_str());
							}
							Sleep(3000);
							send_private_message(dfb.sender, tempm);
						}
					}
					else {
						send_message(tar, u8"还没有人丢过漂流瓶呢" + utils::random_numofchars(u8".", 3, 8));
					}
				}
				else {
					vector<string> insufficientchance;
					insufficientchance.push_back(u8"你当前没有捡瓶子的机会了，扔<一个>漂流瓶可以获得<两次>打捞的机会，或每日<首次>使用漂流瓶也可获得<10次>免费打捞的机会~");
					insufficientchance.push_back(u8"You don't have a chance to pick up any drift bottle right now! Throw a bottle to get 2 picks up chances, or you can also get 10 free times to pick up the bottle by using the drifting bottle for the first time every day!");
					insufficientchance.push_back(u8"あなたは今漂流瓶を拾う機会がありませんでした。また漂流瓶を得る方法はこちら：1つの漂流瓶を捨てると2回の拾う機会を得ることができます。あるいは毎日の初めて漂流瓶を拾うと10回無料で機会を得ることができます。");
					insufficientchance.push_back(u8"У тебя сейчас нет возможности подобрать дрифт бутылку! Брось бутылку, чтобы получить 2 возможности подобрать, или ты также можешь получить 10 бесплатных подборов бутылки используя дрифт бутылку раз в каждый день");
					insufficientchance.push_back(u8"Bạn hết lượt vớt chai, Ném một chai để nhận thêm 2 lượt, Hoặc nhận 10 lượt vớt chai cho ngày hôm sau.");
					send_message(tar, insufficientchance[utils::randomNum(0, insufficientchance.size() - 1)]);
				}
			}
		};

		static void remainingPickupcount(const Target tar) {
			Database db;
			db.Connect();
			int t = db.getUserBottleRemaining(tar.user_id);
			if (t > 0) {
				send_message(tar, u8"你当前还可以捡" + to_string(t) + u8"个瓶子！");
			}
			else {
				vector<string> insufficientchance;
				insufficientchance.push_back(u8"你当前没有捡瓶子的机会了，扔<一个>漂流瓶可以获得<两次>打捞的机会，或每日<首次>使用漂流瓶也可获得<10次>免费打捞的机会~");
				insufficientchance.push_back(u8"You don't have a chance to pick up any drift bottle right now! Throw a bottle to get 2 picks up chances, or you can also get 10 free times to pick up the bottle by using the drifting bottle for the first time every day!");
				insufficientchance.push_back(u8"あなたは今漂流瓶を拾う機会がありませんでした。また漂流瓶を得る方法はこちら：1つの漂流瓶を捨てると2回の拾う機会を得ることができます。あるいは毎日の初めて漂流瓶を拾うと10回無料で機会を得ることができます。");
				insufficientchance.push_back(u8"У тебя сейчас нет возможности подобрать дрифт бутылку! Брось бутылку, чтобы получить 2 возможности подобрать, или ты также можешь получить 10 бесплатных подборов бутылки используя дрифт бутылку раз в каждый день");
				insufficientchance.push_back(u8"Bạn hết lượt vớt chai, Ném một chai để nhận thêm 2 lượt, Hoặc nhận 10 lượt vớt chai cho ngày hôm sau.");
				send_message(tar, insufficientchance[utils::randomNum(0, insufficientchance.size() - 1)]);
			}
		}

		static void bottleStatistics(const Target tar) {
			Database db;
			db.Connect();
			send_message(tar, db.getBottleStatistics());
		}

		static void availableBottleCount(const Target tar) {
			Database db;
			db.Connect();
			int n = db.getNumberOfAvailableBottles();
			if (n == 0) { send_message(tar, u8"海上目前还没有漂流瓶呢..."); }
			else { send_message(tar, u8"当前海上有 " + to_string(n) + u8" 个漂流瓶正在远航..."); }
		}
	};

	class CmdParser {
	public:
		static void parser(const Target tar, const GroupSender sdr) {
			try {
				if (_stricmp(tar.message.substr(0, 18).c_str(), u8"剩余打捞次数") == 0) {
					Main::remainingPickupcount(tar); return;
				}
				if (_stricmp(tar.message.substr(0, 15).c_str(), u8"漂流瓶参数") == 0) {
					Main::bottleStatistics(tar); return;
				}
				if (_stricmp(tar.message.substr(0, 21).c_str(), u8"海上漂流瓶数量") == 0) {
					Main::availableBottleCount(tar); return;
				}
				if (_stricmp(tar.message.substr(0, 12).c_str(), u8"扔漂流瓶") == 0 || _stricmp(tar.message.substr(0, 12).c_str(), u8"丢漂流瓶") == 0) {
					Main::driftingBottleVoid(true, tar, sdr); return;
				}
				if (_stricmp(tar.message.substr(0, 12).c_str(), u8"捡漂流瓶") == 0 || _stricmp(tar.message.substr(0, 12).c_str(), u8"捞漂流瓶") == 0) {
					Main::driftingBottleVoid(false, tar, sdr); return;
				}
				/*if (_stricmp(tar.message.substr(0, 4).c_str(), "roll") == 0) {
					Main::roll(tar); Main::imagemonitor(sdr, tar); return;
				}
				if (_stricmp(tar.message.substr(0, 3).c_str(), "chp") == 0) {
					Main::chp(tar); return;
				}
				if (_stricmp(tar.message.substr(0, 5).c_str(), "sleep") == 0) {
					Main::sleep(tar); return;
				}
				if (_stricmp(tar.message.substr(0, 18).c_str(), u8"营销号生成器") == 0) {
					Main::marketingGenerator(tar); Main::imagemonitor(sdr, tar); return;
				}
				if (_stricmp(tar.message.substr(0, 7).c_str(), "nbnhhsh") == 0) {
					Main::nbnhhsh(tar); Main::imagemonitor(sdr, tar); return;
				}
				if (tar.message.find(u8"还是") != string::npos || tar.message.find(u8"不") != string::npos || tar.message.find(u8"没") != string::npos) {
					Main::randEvents(tar); Main::imagemonitor(sdr, tar); return;
				}
				if (_stricmp(tar.message.substr(0, 12).c_str(), u8"舔狗日记") == 0) {
					Main::tgrj(tar); return;
				}
				*/

				//猜单词游戏部分
				if (_stricmp(tar.message.substr(0, 15).c_str(), u8"猜单词帮助") == 0) {
					send_message(tar, u8"[CQ:image,file=osucat\\help\\hangmanhelp.png]");
					return;
				}
				if (_stricmp(tar.message.substr(0, 15).c_str(), u8"猜单词状态") == 0) {
					send_message(tar, HangmanGame::gameStatus());
					return;
				}
				if (_stricmp(tar.message.substr(0, 18).c_str(), u8"猜单词排行榜") == 0) {
					int page = 1;
					string page_str = tar.message.substr(18);
					if (utils::isNum(page_str)) page = stoi(page_str);
					send_message(tar, HangmanGame::hangmanRanking(page));
					return;
				}
				if (_stricmp(tar.message.substr(0, 10).c_str(), u8"猜单词+") == 0) {
					send_message(tar, HangmanGame::startHangman(tar, tar.message.substr(10)));
					return;
				}
				if (_stricmp(tar.message.substr(0, 9).c_str(), u8"猜单词") == 0) {
					send_message(tar, HangmanGame::startHangman(tar, ""));
					return;
				}
				if (_stricmp(tar.message.substr(0, 6).c_str(), u8"放弃") == 0) {
					send_message(tar, HangmanGame::giveupHangman(tar));
					return;
				}
				if (tar.message.length() == 1 && HangmanGame::findPlayer(tar.user_id) != -1) {
					send_message(tar, HangmanGame::inputHangman(tar, tar.message));
					return;
				}
				if (_stricmp(tar.message.substr(0, 15).c_str(), u8"我的猜单词") == 0) {
					send_message(tar, HangmanGame::showPlayerData(tar));
					return;
				}
				if (_stricmp(tar.message.substr(0, 35).c_str(), "warning:recalculateallhangmanscores") == 0) {
					for (int i = 0; i < adminlist.size(); i++) {
						if (tar.user_id == adminlist[i].user_id) {
							send_message(tar, HangmanGame::recalculateAllScores());
							return;
						}
					}
					send_message(tar, u8"权限不足");
					return;
				}
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
				send_message(tar, u8"addons模块出现了一个未知错误，请稍后重试...");
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
	};
}


#endif