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
					u8"�û� %s(%lld) �ϴ���ͼƬ,��Ϣ�������£�\n%s",
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
			if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"��������..."); return; }
			if (cmd.length() > 199) { send_message(tar, u8"�������..."); return; }
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
				send_message(tar, u8"��Ȼ��" + temp[utils::randomNum(1, (int)temp.size()) - 1] + u8"��");
			}
			else
				send_message(tar, u8"��Ҫ���������ϵĲ�����..����Ĳ�����...");
		}

		static void chp(const Target tar) {
			send_message(tar, NetConnection::HttpsGet("https://chp.shadiao.app/api.php"));
		}

		static void sleep(const Target tar) {
			switch (utils::randomNum(1, 4)) {
			case 1:
				send_message(tar, u8"[CQ:record,file=osucat\\����ô˯���ŵ�.mp3]");
				break;
			case 2:
				send_message(tar, u8"[CQ:record,file=osucat\\˯���Ű� Ӳ���.mp3]");
				break;
			case 3:
				send_message(tar, u8"[CQ:record,file=osucat\\����-˯���Ű�.mp3]");
				break;
			case 4:
				send_message(tar, u8"[CQ:record,file=osucat\\С����-˯������Ҫ������.mp3]");
				break;
			default:
				break;
			}
		}

		static void marketingGenerator(const Target tar) {
			string cmd = tar.message.substr(18);
			cmd = utils::cqunescape(cmd);
			if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"��������..."); return; }
			if (cmd.length() > 400) { send_message(tar, u8"̫���ˣ�"); return; }
			utils::string_trim(cmd);
			std::vector<std::string> temp = utils::string_split(cmd, '#');
			if (temp.size() != 3) { send_message(tar, u8"������������ʧ�ܡ�\n������ʽΪ ����#�¼�#�¼�����һ��˵��"); return; }
			std::string ���� = temp[0], �¼� = temp[1], �¼�����һ��˵�� = temp[2];
			char message[3072];
			sprintf_s(message,
				3072,
				u8"%s%s����ô�����أ�%s���Ŵ�Ҷ�����Ϥ������%s%s"
				u8"����ô�����أ��������С������һ���˽��°ɡ�"
				u8"%s%s����ʵ����%s����ҿ��ܻ�ܾ���%s��ô"
				u8"%s�أ�����ʵ����������С��Ҳ�е��ǳ����ȡ�"
				u8"����ǹ���%s%s�������ˣ������ʲô�뷨�أ���ӭ����������С��һ������Ŷ~",
				����.c_str(),
				�¼�.c_str(),
				����.c_str(),
				����.c_str(),
				�¼�.c_str(),
				����.c_str(),
				�¼�.c_str(),
				�¼�����һ��˵��.c_str(),
				����.c_str(),
				�¼�.c_str(),
				����.c_str(),
				�¼�.c_str());
			send_message(tar, message);
		}

		static void randEvents(const Target tar) {
			string cmd = tar.message;
			try {
				cmd = utils::cqunescape(cmd);
				if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"��������..."); return; }
				if (cmd.length() > 1024) { send_message(tar, u8"�������..."); return; }
				utils::string_trim(cmd);
				utils::string_replace(cmd, u8"��", u8"{@@}");
				utils::string_replace(cmd, u8"��", u8"��");
				utils::string_replace(cmd, u8"{@@}", u8"��");
				if (cmd.substr(cmd.length() - 3, 3) == u8"��" || cmd.substr(cmd.length() - 1, 1) == "?")
					cmd = cmd.substr(cmd.length() - 3, 3) == u8"��" ? cmd.substr(0, cmd.length() - 3)
					: cmd.substr(0, cmd.length() - 1);
				if (cmd.substr(cmd.length() - 3, 3) == "��" || cmd.substr(cmd.length() - 3, 3) == "��")
					cmd = cmd.substr(0, cmd.length() - 3);
				if (cmd.find(u8"����") != string::npos) {
					string str1, str2;
					str1 = cmd.substr(0, cmd.find(u8"����"));
					str2 = cmd.substr(cmd.find(u8"����") + 6);
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
				if (cmd.find(u8"��") != string::npos) {
					if ((cmd[cmd.find(u8"��") - 1] & 0x80) != 0) {
						//�Ǻ���
						if (cmd.substr(cmd.find(u8"��") - 3, 3) == cmd.substr(cmd.find(u8"��") + 3, 3)) {
							//do something here
							string str1 = cmd.substr(0, cmd.find(u8"��") - 3), str2 = cmd.substr(cmd.find(u8"��") + 3);
							switch (utils::randomNum(1, 2)) {
							case 1:
								send_message(tar, str1 + str2);
								break;
							case 2:
								send_message(tar, str1 + u8"��" + str2);
								break;
							}
						}
					}
					else {
						//���Ǻ���
						if (cmd.substr(0, cmd.find(u8"��")) == cmd.substr(cmd.find(u8"��") + 3)) {
							//do something here
							string str1 = cmd.substr(0, cmd.find(u8"��")), str2 = cmd.substr(cmd.find(u8"��") + 3);
							switch (utils::randomNum(1, 2)) {
							case 1:
								send_message(tar, str2);
								break;
							case 2:
								send_message(tar, u8"��" + str2);
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
			if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"��������..."); return; }
			if (cmd.length() > 199) { send_message(tar, u8"̫���ˣ�"); return; }
			if (cmd == "") { send_message(tar, u8"�㶼����������Ҫ����ɶ���գ�"); return; }
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
							u8"������\"%s\"���˻���%s",
							cmd.c_str(),
							tmp1.c_str());
						send_message(tar, trntmp);
					}
					else { send_message(tar, u8"û���ҵ������д�ķ���;w;"); }
				}
				catch (json::exception) { send_message(tar, u8"û���ҵ������д�ķ���;w;"); }
			}
			catch (osucat::NetWork_Exception) { send_message(tar, u8"��ȡ����ʱ��ʱ...���Ժ�����..."); }
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
				if (throwcount > 20) { send_message(tar, u8"���Ѿ�����20��ƿ�ӳ�ȥ��...��Ϣһ�����Ӱ�...");				return; }
				cmd = utils::cqunescape(cmd);
				utils::string_trim(cmd);
				if (utils::forbiddenWordsLibrary(cmd) == true) { send_message(tar, u8"��������...");			return; }
				if (cmd.length() == 0) { send_message(tar, u8"����д��ʲô����...?");					return; }
				if (cmd.length() > 5000) { send_message(tar, u8"̫���ˣ�"); return; }
				time_t timetmp = time(NULL);
				db.setBottleRemaining(2, tar.user_id);
				db.writeBottle(driftingBottleDBEvent::WRITEIN, 0, timetmp, tar.user_id, senderinfo.nickname, utils::ocescape(cmd));
				db.addPickThrowCount(false);
				char reportMsg[6000];
				string s_bottle = utils::randomNum(1, 100) > 50 ? +u8"Ư��ƿ" : +u8"ƿ��";
				Sleep(100);
				string s_yours = utils::randomNum(1, 100) > 50 ? +u8"���" : +u8"����";
				Sleep(100);
				string s_sendout = utils::randomNum(1, 100) > 50 ? +u8"Ư��Զ��" : +u8"����Զ��";
				if (cmd.find("[CQ:image") != string::npos) {
					send_message(tar, s_yours + s_bottle + u8"�Ѿ�" + s_sendout + utils::random_numofchars(u8".", 3, 8));
					sprintf_s(reportMsg,
						"[%s]\n"
						u8"�û� %s(%lld) ��Ư��ƿ���ϴ���ͼƬ\nƯ��ƿID: %d\n��Ϣ�������£�\n%s",
						utils::unixTime2Str(time(NULL)).c_str(),
						senderinfo.nickname.c_str(),
						tar.user_id,
						db.getBottleID(tar.user_id, utils::ocescape(cmd), timetmp),
						tar.message.c_str()
					);
					send_group_message(management_groupid, reportMsg);
				}
				else {
					send_message(tar, s_yours + s_bottle + u8"�Ѿ�" + s_sendout + utils::random_numofchars(u8".", 3, 8));
					sprintf_s(reportMsg,
						"[%s]\n"
						u8"�û� %s(%lld) �ϴ���Ư��ƿ\nƯ��ƿID: %d\n��Ϣ�������£�\n%s",
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
						string s_contentis = utils::randomNum(1, 100) > 50 ? +u8"ƿ�����������" : +u8"ƿ�������";
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
							u8"�������� %s(%lld) ��Ư��ƿ%s\n"
							u8"ID:%d ���� %s\n"
							u8"%s%s\n%s",
							dfb.nickname.c_str(),
							dfb.sender, utils::random_numofchars(u8".", 3, 8).c_str(), dfb.id,
							utils::unixTime2StrChinese(dfb.sendTime).c_str(), s_contentis.c_str(), utils::random_numofchars(u8".", 3, 8).c_str(),
							dfb.msg.c_str());
						send_message(tar, tempm);
						if (db.RemoveBottle(floor((time(NULL) - stoll(j[tempi]["sendtime"].get<std::string>())) / 86400), dfb.id)) {
							if (dfb.pickcount == 0) {
								sprintf_s(tempm,
									u8"�㷢�� %s\n"
									u8"������Ϊ%s%s����Ϣ�Ѿ��� %s(%lld) ��������%s\n",
									utils::unixTime2StrChinese(dfb.sendTime).c_str(),
									utils::random_numofchars(u8".", 3, 8).c_str(), dfb.msg.c_str(),
									senderinfo.nickname.c_str(), tar.user_id), utils::random_numofchars(u8".", 3, 8).c_str();
							}
							else {
								sprintf_s(tempm,
									u8"�㷢�� %s\n"
									u8"������Ϊ%s%s����Ϣ�Ѿ��� %s(%lld) ��������....\n�ڴ�֮ǰ���ƿ�ӻ����Ķ��� %d ��%s",
									utils::unixTime2StrChinese(dfb.sendTime).c_str(),
									utils::random_numofchars(u8".", 3, 8).c_str(), dfb.msg.c_str(),
									senderinfo.nickname.c_str(), tar.user_id, dfb.pickcount),
									utils::random_numofchars(u8".", 3, 8).c_str();
							}
							Sleep(3000);
							send_private_message(dfb.sender, tempm);
						}
					}
					else { 
						send_message(tar, u8"��û���˶���Ư��ƿ��" + utils::random_numofchars(u8".", 3, 8)); 
					}
				}
				else {
					vector<string> insufficientchance;
					insufficientchance.push_back(u8"�㵱ǰû�м�ƿ�ӵĻ����ˣ���<һ��>Ư��ƿ���Ի��<����>���̵Ļ��ᣬ��ÿ��<�״�>ʹ��Ư��ƿҲ�ɻ��<10��>��Ѵ��̵Ļ���~");
					insufficientchance.push_back(u8"You don't have a chance to pick up any drift bottle right now! Throw a bottle to get 2 picks up chances, or you can also get 10 free times to pick up the bottle by using the drifting bottle for the first time every day!");
					insufficientchance.push_back(u8"���ʤ��Ͻ�Ư��ƿ��ʰ���C�᤬����ޤ���Ǥ������ޤ�Ư��ƿ��ä뷽���Ϥ����飺1�Ĥ�Ư��ƿ��ΤƤ��2�ؤ�ʰ���C���ä뤳�Ȥ��Ǥ��ޤ������뤤�Ϛ��դγ����Ư��ƿ��ʰ����10�؟o�ϤǙC���ä뤳�Ȥ��Ǥ��ޤ���");
					insufficientchance.push_back(u8"�� ��֧ҧ� ��֧ۧ�ѧ� �ߧ֧� �ӧ�٧ާ�اߧ���� ���է�ҧ�ѧ�� �է�ڧ�� �ҧ���ݧܧ�! ������� �ҧ���ݧܧ�, ����ҧ� ���ݧ��ڧ�� 2 �ӧ�٧ާ�اߧ���� ���է�ҧ�ѧ��, �ڧݧ� ��� ��ѧܧا� �ާ�ا֧�� ���ݧ��ڧ�� 10 �ҧ֧��ݧѧ�ߧ�� ���էҧ���� �ҧ���ݧܧ� �ڧ���ݧ�٧�� �է�ڧ�� �ҧ���ݧܧ� ��ѧ� �� �ܧѧاէ�� �է֧ߧ�");
					insufficientchance.push_back(u8"�㵱ǰû�м�ƿ�ӵĻ�����");
					send_message(tar, insufficientchance[utils::randomNum(0, insufficientchance.size() - 1)]);
				}
			}
		};

		static void remainingPickupcount(const Target tar) {
			Database db;
			db.Connect();
			int t = db.getUserBottleRemaining(tar.user_id);
			if (t > 0) {
				send_message(tar, u8"�㵱ǰ�����Լ�" + to_string(t) + u8"��ƿ�ӣ�");
			}
			else {
				send_message(tar, u8"�㵱ǰû�м�ƿ�ӵĻ����ˣ���Ư��ƿ���Ի��һ�δ��̵Ļ��ᣬ��ÿ���״�ʹ��Ư��ƿҲ�ɻ��20����Ѵ��̵Ļ���~");
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
			if (n == 0) { send_message(tar, u8"����Ŀǰ��û��Ư��ƿ��..."); }
			else { send_message(tar, u8"��ǰ������ " + to_string(n) + u8" ��Ư��ƿ����Զ��..."); }
		}
	};

	class CmdParser {
	public:
		static void parser(const Target tar, const GroupSender sdr) {
			try {
				if (_stricmp(tar.message.substr(0, 18).c_str(), u8"ʣ����̴���") == 0) {
					Main::remainingPickupcount(tar); return;
				}
				if (_stricmp(tar.message.substr(0, 15).c_str(), u8"Ư��ƿ����") == 0) {
					Main::bottleStatistics(tar); return;
				}
				if (_stricmp(tar.message.substr(0, 21).c_str(), u8"����Ư��ƿ����") == 0) {
					Main::availableBottleCount(tar); return;
				}
				if (_stricmp(tar.message.substr(0, 12).c_str(), u8"��Ư��ƿ") == 0 || _stricmp(tar.message.substr(0, 12).c_str(), u8"��Ư��ƿ") == 0) {
					Main::driftingBottleVoid(true, tar, sdr); return;
				}
				if (_stricmp(tar.message.substr(0, 12).c_str(), u8"��Ư��ƿ") == 0 || _stricmp(tar.message.substr(0, 12).c_str(), u8"��Ư��ƿ") == 0) {
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
				if (_stricmp(tar.message.substr(0, 18).c_str(), u8"Ӫ����������") == 0) {
					Main::marketingGenerator(tar); Main::imagemonitor(sdr, tar); return;
				}
				if (_stricmp(tar.message.substr(0, 7).c_str(), "nbnhhsh") == 0) {
					Main::nbnhhsh(tar); Main::imagemonitor(sdr, tar); return;
				}
				if (tar.message.find(u8"����") != string::npos || tar.message.find(u8"��") != string::npos || tar.message.find(u8"û") != string::npos) {
					Main::randEvents(tar); Main::imagemonitor(sdr, tar); return;
				}
				if (_stricmp(tar.message.substr(0, 12).c_str(), u8"���ռ�") == 0) {
					Main::tgrj(tar); return;
				}
				*/

				//�µ�����Ϸ����
				if (_stricmp(tar.message.substr(0, 15).c_str(), u8"�µ��ʰ���") == 0) {
					send_message(tar, u8"[CQ:image,file=osucat\\help\\hangmanhelp.png]");
					return;
				}
				if (_stricmp(tar.message.substr(0, 15).c_str(), u8"�µ���״̬") == 0) {
					send_message(tar, HangmanGame::gameStatus());
					return;
				}
				if (_stricmp(tar.message.substr(0, 18).c_str(), u8"�µ������а�") == 0) {
					int page = 1;
					string page_str = tar.message.substr(18);
					if (utils::isNum(page_str)) page = stoi(page_str);
					send_message(tar, HangmanGame::hangmanRanking(page));
					return;
				}
				if (_stricmp(tar.message.substr(0, 10).c_str(), u8"�µ���+") == 0) {
					send_message(tar, HangmanGame::startHangman(tar, tar.message.substr(10)));
					return;
				}
				if (_stricmp(tar.message.substr(0, 9).c_str(), u8"�µ���") == 0) {
					send_message(tar, HangmanGame::startHangman(tar, ""));
					return;
				}
				if (_stricmp(tar.message.substr(0, 6).c_str(), u8"����") == 0) {
					send_message(tar, HangmanGame::giveupHangman(tar));
					return;
				}
				if (tar.message.length() == 1 && HangmanGame::findPlayer(tar.user_id) != -1) {
					send_message(tar, HangmanGame::inputHangman(tar, tar.message));
					return;
				}
				if (_stricmp(tar.message.substr(0, 15).c_str(), u8"�ҵĲµ���") == 0) {
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
					send_message(tar, u8"Ȩ�޲���");
					return;
				}
			}
			catch (osucat::database_exception& ex) {
				send_message(tar, u8"�������ݿ�ʱ������һ���������Ժ�����...");
				char reportMsg[1024];
				sprintf_s(reportMsg,
					"[%s]\n"
					u8"Mysql���ִ���\n"
					u8"������룺%d\n"
					u8"��ϸ��Ϣ��%s\n",
					utils::unixTime2Str(time(NULL)).c_str(),
					ex.Code(),
					ex.Info().c_str()
				);
				send_private_message(owner_userid, reportMsg);
			}
			catch (osucat::NetWork_Exception& ex) {
				send_message(tar, u8"����apiʱ��ʱ...���Ժ�����...");
			}
			catch (std::exception& ex) {
				send_message(tar, u8"addonsģ�������һ��δ֪�������Ժ�����...");
				char reportMsg[1024];
				sprintf_s(reportMsg,
					"[%s]\n"
					u8"�Ѳ���std::exception\n"
					u8"�����ߣ�%lld\n"
					u8"����ָ�%s\n"
					u8"��ϸ��Ϣ��%s\n",
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