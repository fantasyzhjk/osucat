#pragma once
#ifndef OC_MYSQL_HPP
#define OC_MYSQL_HPP

#define BOTLEEXPECTEDVALUE 100.00
#define BOTTLEMAXEXISTDAYS 7
#define MAXPOOLSIZE	200
#define MINPOOLSIZE 20
#define INITIALTPRATE 0.6

#include <mysql.h>

char SQL_USER[32], SQL_HOST[32], SQL_PWD[32], SQL_DATABASE[32];
int SQL_PORT;

namespace osucat::addons {
	enum driftingBottleDBEvent { WRITEIN = 0, ADDCOUNTER, CHANGESTATUS, DELETEBOTTLE };
}

namespace osucat::entertainment {
	// TODO
}

namespace osucat {

	struct AdminInfo {
		int64_t user_id;
		int role;  // 0=normal user 1=admin/owner 2=moderator
	};

	struct Badgeinfo {
		int id;
		std::string name;
		std::string name_chinese;
		std::string description;
	};

	static std::vector<AdminInfo> adminlist;

	class Database {
	public:
		Database() {
			mysql_init(&this->conn);
		}

		void Connect() {
			if (mysql_real_connect(&this->conn, SQL_HOST, SQL_USER, SQL_PWD, SQL_DATABASE, SQL_PORT, NULL, 0) == NULL) {
				throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
			}
			if (int t = mysql_set_character_set(&conn, "utf8mb4") != NULL) {
				mysql_close(&this->conn);
				throw osucat::database_exception(mysql_error(&this->conn), t);
			}
		}

		void Execute(const std::string& sql) {
			if (0 != mysql_query(&this->conn, sql.c_str())) {
				throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
			}
		}

		void Commit() {
			mysql_commit(&this->conn);
		}

		void Update(const std::string& updateSql) {
			std::string query = updateSql;
			query.erase(0, query.find_first_not_of(" "));
			query.erase(query.find_last_not_of(" ") + 1);
			std::string sub = query.substr(0, 6);
			std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
			if (sub.compare("update")) {
				throw osucat::database_exception("This Query is not an Update Query!", 1047);
			}
			if (0 != mysql_query(&this->conn, updateSql.c_str())) {
				throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
			}
			mysql_commit(&this->conn);
		}

		void Insert(const std::string& insertSql) {
			std::string query = insertSql;
			query.erase(0, query.find_first_not_of(" "));
			query.erase(query.find_last_not_of(" ") + 1);
			std::string sub = query.substr(0, 6);
			std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
			if (sub.compare("insert")) {
				throw osucat::database_exception("This Query is not an Insert Query!", 1047);
			}
			if (0 != mysql_query(&this->conn, insertSql.c_str())) {
				throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
			}
			mysql_commit(&this->conn);
		}

		void Delete(const std::string& deleteSql) {
			std::string query = deleteSql;
			query.erase(0, query.find_first_not_of(" "));
			query.erase(query.find_last_not_of(" ") + 1);
			std::string sub = query.substr(0, 6);
			std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
			if (sub.compare("delete")) {
				throw osucat::database_exception("This Query is not an Delete Query!", 1047);
			}
			if (0 != mysql_query(&this->conn, deleteSql.c_str())) {
				throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
			}
			mysql_commit(&this->conn);
		}

		json Select(const std::string& selectSql) {
			std::string query = selectSql;
			query.erase(0, query.find_first_not_of(" "));
			query.erase(query.find_last_not_of(" ") + 1);
			std::string sub = query.substr(0, 6);
			std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
			if (sub.compare("select")) {
				throw osucat::database_exception("This Query is not an Selete Query!", 1047);
			}

			MYSQL_RES* result;
			MYSQL_ROW row;
			json ret;
			if (mysql_query(&this->conn, selectSql.c_str()) == 0) {
				result = mysql_store_result(&this->conn);
				if (NULL != result) {
					ret = nlohmann::json::array();

					unsigned int cFields = mysql_num_fields(result);
					MYSQL_FIELD* field;
					std::vector<std::string> fields(cFields);
					for (unsigned int i = 0; i < cFields; ++i) {
						field = mysql_fetch_field(result);
						fields[i] = field->name;
					}

					while (row = mysql_fetch_row(result)) {
						int i = 0;
						json aResult;
						for (unsigned int i = 0; i < cFields; ++i) {
							std::string value = row[i];
							aResult[fields[i]] = value;
						}
						ret.push_back(aResult);
					}
				}
				else {
					throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
				}
				mysql_free_result(result);
				if (0 == ret.size()) {
					throw osucat::database_exception("Empty Set!", 1065);
				}
				return ret;
			}
			else {
				throw osucat::exception(mysql_error(&this->conn), mysql_errno(&this->conn));
			}
		}

		void addcallcount() {
			try { this->Update("update total_call_count set count=" + std::to_string(stoi(this->Select("select count from total_call_count")[0]["count"].get<std::string>()) + 1)); }
			catch (osucat::database_exception) {}
		}

		void UserSet(int64_t user_id, int64_t qq) {
			try {
				char query[1024];
				sprintf_s(query, 1024, "insert into info (uid, qq) values (%I64d, %I64d)", user_id, qq);
				this->Insert(query);
			}
			catch (osucat::database_exception) {
				//忽略
			}
		}

		void AddUserData(const osu_api::v1::user_info* info, const std::string& timeStr) {
			int64_t osu_uid = info->user_id;
			int64_t osu_count300 = info->n300;
			int64_t osu_count100 = info->n100;
			int64_t osu_count50 = info->n50;
			int64_t osu_playcount = info->playcount;
			int64_t osu_ranked_score = info->ranked_score;
			int64_t osu_total_score = info->total_score;
			int64_t osu_pp_rank = info->global_rank;
			double osu_level = info->level;
			double osu_pp_raw = info->pp;
			double osu_accuracy = info->accuracy;
			int64_t osu_count_rank_ss = info->count_ss;
			int64_t osu_count_rank_ssh = info->count_ssh;
			int64_t osu_count_rank_s = info->count_s;
			int64_t osu_count_rank_sh = info->count_sh;
			int64_t osu_count_rank_a = info->count_a;
			int64_t osu_total_seconds_played = info->playtime;
			int64_t osu_pp_country_rank = info->country_rank;
			char buffer[8192];
			sprintf_s(buffer,
				8192,
				"insert into osu_user_records values (%lld,%lld,%lld,%lld,%lld,"
				"%lld,%lld,%.2f,%.2f,%.2f,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%d,\"%s\")",
				osu_uid,
				osu_count300,
				osu_count100,
				osu_count50,
				osu_playcount,
				osu_ranked_score,
				osu_total_score,
				osu_level,
				osu_pp_raw,
				osu_accuracy,
				osu_count_rank_ss,
				osu_count_rank_ssh,
				osu_count_rank_s,
				osu_count_rank_sh,
				osu_count_rank_a,
				osu_total_seconds_played,
				osu_pp_country_rank,
				osu_pp_rank,
				info->mode,
				timeStr.c_str());
			this->Insert(buffer);
		}

		int64_t osu_getqq(int64_t uid) {
			try { return std::stoll(this->Select("select qq from osu_user_list where uid=" + std::to_string(uid))[0]["qq"].get<std::string>()); }
			catch (osucat::database_exception) { return 0; }
		}

		int64_t osu_getuserid(int64_t qq) {
			try { return std::stoll(this->Select("select uid from info where qq=" + std::to_string(qq))[0]["uid"].get<std::string>()); }
			catch (osucat::database_exception) {}
		}

		int osu_getunsetstatus(int64_t uid) {
			try { return std::stol(this->Select("select verifying_unset from info where uid=" + std::to_string(uid))[0]["verifying_unset"].get<std::string>()); }
			catch (osucat::database_exception) { return 0; }
		}

		void osu_changeunsetstatus(int64_t uid, bool status) {
			this->Update("update info set verifying_unset=" + std::to_string(status == true ? 1 : 0) + " where uid=" + std::to_string(uid));
		}

		void osu_deleteuser(int64_t uid) {
			this->Delete("DELETE from info_record where uid = " + std::to_string(uid));
			this->Delete("DELETE from info where uid = " + std::to_string(uid));
		}

		void osu_UpdatePPlus(int64_t uid, float pp, osu_api::v1::pplus_info pi) {
			if (this->osu_GetUserPreviousPP(uid) == 0.0) {
				this->Insert("insert into pplus (uid, pp, jump, flow, pre, acc, spd, sta) values (" + std::to_string(uid)
					+ ", " + std::to_string(pp) + ", " + std::to_string(pi.jump) + ", " + std::to_string(pi.flow) + ", "
					+ std::to_string(pi.pre) + ", " + std::to_string(pi.acc) + ", " + std::to_string(pi.spd) + ", "
					+ std::to_string(pi.sta) + ")");
			}
			else {
				this->Update("update pplus set pp=" + std::to_string(pp) + ",jump=" + std::to_string(pi.jump)
					+ ",flow=" + std::to_string(pi.flow) + ",pre=" + std::to_string(pi.pre) + ",acc=" + std::to_string(pi.acc)
					+ ",spd=" + std::to_string(pi.spd) + ",sta=" + std::to_string(pi.sta) + " where uid=" + std::to_string(uid));
			}
		}

		float osu_GetUserPreviousPP(int64_t uid) {
			try { return std::stof(this->Select("select pp from pplus where uid=" + std::to_string(uid))[0]["pp"].get<std::string>()); }
			catch (osucat::database_exception) { return 0.0; }
		}

		int osu_GetUserDefaultGameMode(int64_t uid) {
			try { return std::stoi(this->Select("select mainmode from info where uid=" + std::to_string(uid))[0]["mainmode"].get<std::string>()); }
			catch (osucat::database_exception& e) { return 0; }
		}

		void osu_GetUserPreviousPPlus(int64_t uid, osu_api::v1::pplus_info* pi) {
			try {
				json result = this->Select("select * from pplus where uid=" + std::to_string(uid));
				pi->acc = stoi(result[0]["acc"].get<std::string>());
				pi->flow = stoi(result[0]["flow"].get<std::string>());
				pi->spd = stoi(result[0]["spd"].get<std::string>());
				pi->sta = stoi(result[0]["sta"].get<std::string>());
				pi->pre = stoi(result[0]["pre"].get<std::string>());
				pi->jump = stoi(result[0]["jump"].get<std::string>());
			}
			catch (osucat::database_exception) {
				pi->acc = 0;
				pi->flow = 0;
				pi->spd = 0;
				pi->sta = 0;
				pi->pre = 0;
				pi->jump = 0;
			}
		}

		int osu_getshownbadges(int64_t uid) {
			try {
				int value = std::stol(this->Select("select shownbadgeID from info where uid=" + std::to_string(uid))[0]["shownbadgeID"].get<std::string>());
				if (value != 25565) { return value; }
				else { return EOF; }
			}
			catch (osucat::database_exception) { return EOF; }
		}

		// 返回值说明： 0 = 成功 ， -1 = 失败 ， 任意数字 = 未查询到所提供日期的结果 但是查询到了临近日期的数据
		int osu_GetUserData(int64_t user_id, int days, osu_api::v1::user_info* info) {
			char query[1024];
			time_t now = time(NULL);
			char timeStr[30] = { 0 };
			json result;
			tm* tm_time = localtime(&now);
			strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_time);
			if (days == 0) {
				try {
					sprintf_s(query,
						1024,
						"select * from osu_user_records where uid=%lld and gamemode=%d and lastupdate=\"%s\"",
						user_id,
						(int)info->mode,
						timeStr);
					result = this->Select(query);
				}
				catch (osucat::database_exception& e) {
					now = now - 86400;
					tm* tm_time = localtime(&now);
					strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_time);
					sprintf_s(query,
						1024,
						"select * from osu_user_records where uid=%lld and gamemode=%d and lastupdate=\"%s\"",
						user_id,
						(int)info->mode,
						timeStr);
					result = this->Select(query);
				}
			}
			else {
				now = now - days * 86400;
				tm* tm_time = localtime(&now);
				strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_time);
				sprintf_s(query,
					1024,
					"select * from osu_user_records where uid=%lld and gamemode=%d and lastupdate=\"%s\"",
					user_id,
					(int)info->mode,
					timeStr);
				try {
					result = this->Select(query);
				}
				catch (osucat::database_exception& e) {
					sprintf_s(query,
						1024,
						"select * from osu_user_records where uid=%lld and gamemode=%d and lastupdate<\"%s\"",
						user_id,
						(int)info->mode,
						timeStr);
					try {
						result = this->Select(query);
					}
					catch (osucat::database_exception& e) {
						sprintf_s(query,
							1024,
							"select * from osu_user_records where uid=%lld and gamemode=%d",
							user_id,
							(int)info->mode,
							timeStr);

						try {
							result = this->Select(query);
						}
						catch (osucat::database_exception& e) {
							return -1;
						}
					}
					tm time_tmp;
					int itmp = result.size() - 1;
					cqhttp_api::utils::fakestrptime(result[itmp]["lastupdate"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_tmp);
					info->user_id = std::stoll(result[itmp]["uid"].get<std::string>());
					info->n300 = std::stoll(result[itmp]["n300"].get<std::string>());
					info->n100 = std::stoll(result[itmp]["n100"].get<std::string>());
					info->n50 = std::stoll(result[itmp]["n50"].get<std::string>());
					info->playcount = std::stoll(result[itmp]["PC"].get<std::string>());
					info->total_score = std::stoll(result[itmp]["TS"].get<std::string>());
					info->ranked_score = std::stoll(result[itmp]["RS"].get<std::string>());
					info->country_rank = std::stoll(result[itmp]["CR"].get<std::string>());
					info->global_rank = std::stoll(result[itmp]["GR"].get<std::string>());
					info->playtime = std::stoll(result[itmp]["PT"].get<std::string>());
					info->pp = std::stof(result[itmp]["PP"].get<std::string>());
					info->level = std::stof(result[itmp]["LV"].get<std::string>());
					info->accuracy = std::stod(result[itmp]["ACC"].get<std::string>());
					info->count_ssh = std::stoi(result[itmp]["XH"].get<std::string>());
					info->count_ss = std::stoi(result[itmp]["X"].get<std::string>());
					info->count_sh = std::stoi(result[itmp]["S"].get<std::string>());
					info->count_s = std::stoi(result[itmp]["SH"].get<std::string>());
					info->count_a = std::stoi(result[itmp]["A"].get<std::string>());
					return ((time_t)time(NULL) - ((int64_t)mktime(&time_tmp) + 16 * 3600)) / 86400;
				}
			}
			info->user_id = std::stoll(result[0]["uid"].get<std::string>());
			info->n300 = std::stoll(result[0]["n300"].get<std::string>());
			info->n100 = std::stoll(result[0]["n100"].get<std::string>());
			info->n50 = std::stoll(result[0]["n50"].get<std::string>());
			info->playcount = std::stoll(result[0]["PC"].get<std::string>());
			info->total_score = std::stoll(result[0]["TS"].get<std::string>());
			info->ranked_score = std::stoll(result[0]["RS"].get<std::string>());
			info->country_rank = std::stoll(result[0]["CR"].get<std::string>());
			info->global_rank = std::stoll(result[0]["GR"].get<std::string>());
			info->playtime = std::stoll(result[0]["PT"].get<std::string>());
			info->pp = std::stof(result[0]["PP"].get<std::string>());
			info->level = std::stof(result[0]["LV"].get<std::string>());
			info->accuracy = std::stod(result[0]["ACC"].get<std::string>());
			info->count_ssh = std::stoi(result[0]["XH"].get<std::string>());
			info->count_ss = std::stoi(result[0]["X"].get<std::string>());
			info->count_sh = std::stoi(result[0]["S"].get<std::string>());
			info->count_s = std::stoi(result[0]["SH"].get<std::string>());
			info->count_a = std::stoi(result[0]["A"].get<std::string>());
			return 0;
		}

		void osu_UpdatePPRecord(int64_t qq, int64_t bid) {
			if (this->osu_GetPPRecord(qq) == EOF) { this->Insert("insert into pprecord (qq, bid) values (" + std::to_string(qq) + ", " + std::to_string(bid) + ")"); }
			else { this->Update("update pprecord set bid=" + std::to_string(bid) + " where qq=" + std::to_string(qq)); }
		}

		int64_t osu_GetPPRecord(int64_t qq) {
			try { return std::stoll(this->Select("select bid from pprecord where qq=" + std::to_string(qq))[0]["bid"].get<std::string>()); }
			catch (osucat::database_exception) { return EOF; }
		}

		int osu_SetUserMainMode(int64_t qq, int mode) {
			char buffer[1024];
			sprintf_s(buffer, 1024, "update info set mainmode=%d where qq=%lld", mode, qq);
			try {
				this->Update(buffer);
				return 1;
			}
			catch (osucat::database_exception) {
				return 0;
			}
		}

		///* 1 = bp /2 = rctpp /3 = recent /4 = entertainment /5 = repeater
		int isGroupEnable(int64_t group_id, int cmd) {
			try {
				std::string _cmd;
				switch (cmd) {
				case 1:
					_cmd = "bp";
					break;
				case 2:
					_cmd = "rctpp";
					break;
				case 3:
					_cmd = "recent";
					break;
				case 4:
					_cmd = "entertainment";
					break;
				case 5:
					_cmd = "repeater";
					break;
				}
				json result = this->Select("select " + _cmd + " from group_settings where group_id=" + std::to_string(group_id));
				switch (cmd) {
				case 1:
					return std::stoi(result[0]["bp"].get<std::string>());
				case 2:
					return std::stoi(result[0]["rctpp"].get<std::string>());
				case 3:
					return std::stoi(result[0]["recent"].get<std::string>());
				case 4:
					return std::stoi(result[0]["entertainment"].get<std::string>());
				case 5:
					return std::stoi(result[0]["repeater"].get<std::string>());
				}
			}
			catch (osucat::database_exception) {
				this->setGroupSettings(group_id);
				return 1;
			}
		}

		void changeGroupSettings(int64_t group_id, int cmd, bool on_off) {
			try {
				std::string _cmd;
				switch (cmd) {
				case 1:
					_cmd = "bp";
					break;
				case 2:
					_cmd = "rctpp";
					break;
				case 3:
					_cmd = "recent";
					break;
				case 4:
					_cmd = "entertainment";
					break;
				case 5:
					_cmd = "repeater";
					break;
				}
				if (on_off == true) {
					_cmd += "=1";
				}
				else {
					_cmd += "=0";
				}
				this->Update("update group_settings set " + _cmd + " where group_id=" + std::to_string(group_id));
			}
			catch (osucat::database_exception) {
				cqhttp_api::send_group_message(group_id, u8"修改群设置失败");
				//cout << "修改群组设定失败" << endl;
			}
		}

		void setGroupSettings(int64_t group_id) {
			try { this->Insert("insert into group_settings (group_id) values (" + std::to_string(group_id) + ")"); }
			catch (osucat::database_exception) { std::cout << "初始化群组设定失败" << std::endl; }
		}

		void osu_setshownBadges(int64_t uid, int badgeid) {
			try { this->Update("update info set shownbadgeID=" + std::to_string(badgeid) + " where uid=" + std::to_string(uid)); }
			catch (osucat::database_exception) {}
		}

		std::vector<int64_t> GetUserSet() {
			std::vector<int64_t> ret;
			json result = this->Select("SELECT uid FROM osu_user_list");
			for (int i = 0; i < result.size(); ++i) {
				ret.push_back(stoll(result[i]["uid"].get<std::string>()));
			}
			return ret;
		}

		std::string getBadgeStr(int id) {
			Badgeinfo bi = this->osu_getBadgeInfo(id);
			char tmp[2048];
			sprintf_s(tmp, "[ID:%d] %s (%s)", bi.id, bi.name_chinese.c_str(), bi.name.c_str());
			return tmp;
		}

		//当id=25565或name=404时 代表未找到此徽章
		Badgeinfo osu_getBadgeInfo(int ID) {
			Badgeinfo bi;
			try {
				json j = this->Select("SELECT * from badge_list where id=" + std::to_string(ID));
				bi.id = stoi(j[0]["id"].get<std::string>());
				bi.name = j[0]["name"].get<std::string>();
				bi.name_chinese = j[0]["name_chinese"].get<std::string>();
				bi.description = j[0]["description"].get<std::string>();
			}
			catch (osucat::database_exception) {
				bi.id = BADGENOTFOUND;
				bi.name = "404";
				bi.name_chinese = "未找到此勋章";
				bi.description = "no badge found.";
			}
			return bi;
		}

		std::vector<int> osu_GetBadgeList(int64_t uid) {
			char query[1024];
			std::vector<int> ret;
			sprintf_s(query, 1024, "select badge from badge where uid=%lld", uid);
			try {
				badgeSystem::main bgs;
				json result = this->Select(query);
				ret = bgs.badgeParser(result[0]["badge"].get<std::string>());
			}
			catch (osucat::database_exception) {
			}
			catch (json::exception& ex) {
			}
			return ret;
		}

		int Getcallcount() {
			try { return std::stoi(this->Select("select count from total_call_count")[0]["count"].get<std::string>()); }
			catch (osucat::database_exception) { return EOF; }
		}

		void osu_addbadge(int64_t uid, std::string str) {
			std::vector<int> tmp = this->osu_GetBadgeList(uid);
			if (tmp.size() == 0) { this->Insert("insert into badge (uid, badge) values (" + std::to_string(uid) + ", \"" + str + "\")"); }
			else { this->Update("update badge set badge=\"" + str + "\" where uid=" + std::to_string(uid)); }
		}

		bool add_blacklist(int64_t qq) {
			try {
				this->Insert("INSERT INTO blacklist (qq,is_blocked) values(" + std::to_string(qq) + ",1)");
				try {
					int64_t uid = this->osu_getuserid(qq);
					if (uid != 0) {
						this->Delete("DELETE from info where uid = " + std::to_string(uid));
						this->Delete("DELETE from info_record where uid = " + std::to_string(uid));
					}
				}
				catch (osucat::database_exception) {}
				try {
					this->Update(u8"UPDATE bottlemsgrecord SET available=0,sendtime=0,sender=-1,nickname=\"deleted\",message=\"此消息已被管理员删除\" where sender=" + std::to_string(qq));
				}
				catch (osucat::database_exception) {}
				return true;
			}
			catch (osucat::database_exception) {
				return false;
			}
		}

		int is_Blocked(int64_t qq) {
			try { return std::stoi(this->Select("select is_blocked from blacklist where qq=" + std::to_string(qq))[0]["is_blocked"].get<std::string>()); }
			catch (osucat::database_exception) { return 0; }
		}

		bool osu_setNewBadge(Badgeinfo bi, int* id) {
			try {
				this->Insert("INSERT INTO badge_list (name,name_chinese,description) VALUES (\"" + bi.name + "\",\"" + bi.name_chinese + "\",\"" + bi.description + "\")");
				json j = this->Select("Select id from badge_list");
				*id = j.size() - 1;
				return true;
			}
			catch (osucat::database_exception) { *id = -1; }
			return false;
		}

		bool osu_updatebadgeinfo(Badgeinfo bi, int f) {
			if (f > 3 || f < 1) {
				return false;
			}
			std::string query;
			if (f = 1) {
				query = u8"UPDATE badge_list set name=\"" + bi.name + "\" where id=" + std::to_string(bi.id);
			}
			else if (f = 2) {
				query = u8"UPDATE badge_list set name_chinese=\"" + bi.name_chinese + "\" where id=" + std::to_string(bi.id);
			}
			else if (f = 3) {
				query = u8"UPDATE badge_list set description=\"" + bi.description + "\" where id=" + std::to_string(bi.id);
			}
			try {
				this->Update(query);
				return true;
			}
			catch (osucat::database_exception) {
				return false;
			}
		}

		//1 = isdaily plus+20 /2 = plus /3 = minus
		void setBottleRemaining(int isdaily, int64_t qq, int dailyoldvalue = 0) {
			if (isdaily == 1) { this->Update("update bottlerecord set remaining=" + std::to_string(floor(dailyoldvalue * 0.5) + 5) + ",lastrewardtime=" + std::to_string(time(NULL)) + " where qq=" + std::to_string(qq)); return; }
			if (isdaily == 2) { this->Update("update bottlerecord set remaining=" + std::to_string(this->getUserBottleRemaining(qq) + 2) + " where qq=" + std::to_string(qq)); return; }
			if (isdaily == 3) { this->Update("update bottlerecord set remaining=" + std::to_string(this->getUserBottleRemaining(qq) - 1) + " where qq=" + std::to_string(qq)); return; }
		}

		int getUserBottleRemaining(int64_t qq) {
			try {
				std::string query = "SELECT * FROM bottlerecord WHERE qq=" + std::to_string(qq);
				json j = this->Select(query);
				int64_t a = stoll(j[0]["lastrewardtime"].get<std::string>());
				int b = stoi(j[0]["remaining"].get<std::string>());
				char rtn[128];
				time_t tick = a;
				struct tm tm = { 0 };
				tm = *localtime(&tick);
				strftime(rtn, 128, "%d", &tm);
				std::string tmp1 = rtn;
				tick = time(NULL);
				tm = *localtime(&tick);
				strftime(rtn, 128, "%d", &tm);
				std::string tmp2 = rtn;
				if (tmp1 != tmp2) {
					this->setBottleRemaining(1, qq, b);
					j = this->Select(query);
					return stoi(j[0]["remaining"].get<std::string>());
				}
				else { return stoi(j[0]["remaining"].get<std::string>()); }
			}
			catch (osucat::database_exception) { this->Insert("INSERT INTO bottlerecord (qq,remaining,lastrewardtime) VALUES (" + std::to_string(qq) + ",5," + std::to_string(time(NULL)) + ")"); return 5; }
		}

		void writeBottle(osucat::addons::driftingBottleDBEvent d, int id, int64_t sendtimetick, int64_t senderuid, const std::string nickname, const std::string message) {
			char tmp[10240];
			switch (d) {
			case osucat::addons::driftingBottleDBEvent::WRITEIN:
				sprintf_s(tmp,
					"INSERT INTO bottlemsgrecord (sendtime,sender,message,nickname) values (%lld,%lld,\"%s\",\"%s\")",
					sendtimetick,
					senderuid,
					message.c_str(),
					nickname.c_str());
				this->Insert(tmp);
				break;
			case osucat::addons::driftingBottleDBEvent::ADDCOUNTER: {
				json j = this->Select("SELECT pickcount FROM bottlemsgrecord where id=" + std::to_string(id));
				sprintf_s(tmp,
					"UPDATE bottlemsgrecord SET pickcount=%d where id=%d",
					stoi(j[0]["pickcount"].get<std::string>()) + 1,
					id);
				this->Update(tmp);
				break;
			}
			case osucat::addons::driftingBottleDBEvent::CHANGESTATUS:
				sprintf_s(tmp,
					"UPDATE bottlemsgrecord SET available=0 where id=%d",
					id);
				this->Update(tmp);
				break;
			case osucat::addons::driftingBottleDBEvent::DELETEBOTTLE:
				sprintf_s(tmp,
					u8"UPDATE bottlemsgrecord SET available=0,sendtime=0,sender=-1,nickname=\"deleted\",message=\"此消息已被管理员删除\" where id=%d",
					id);
				this->Update(tmp);
				break;
			default:
				break;
			}
		}

		std::string getBottleStatistics() {
			json j = this->Select("SELECT * FROM bottletprecord");
			int length = j.size();
			double total_pick = 0;
			double total_throw = 0;
			double poolSize = BOTLEEXPECTEDVALUE;
			double rate = INITIALTPRATE;
			std::string last3 = "";
			for (int k = 0; k < length; k++) {
				double kpick = stoi(j[k]["pick"].get<std::string>());
				double kthrow = stoi(j[k]["throw"].get<std::string>());
				total_pick += kpick;
				total_throw += kthrow;
				if (k < length - 1) {
					poolSize = poolSize * 0.8 + 0.2 * kpick * 0.4;
					if (kpick > 0.5) rate = rate * 0.8 + 0.2 * kthrow / kpick;
				}
				if (k >= length - 3) last3 = last3 + "\n" + std::to_string((int)kthrow) + "/" + std::to_string((int)kpick);
			}
			if (poolSize > MAXPOOLSIZE) poolSize = MAXPOOLSIZE;
			if (poolSize < MINPOOLSIZE) poolSize = MINPOOLSIZE;
			double removeProb = min(1, rate * pow((double)this->getNumberOfAvailableBottles() / poolSize, 0.7));
			return u8"当前剩余瓶数 = " + std::to_string(this->getNumberOfAvailableBottles()) + u8"\n总扔/捡数 = " + std::to_string((int)total_throw) + "/" + std::to_string((int)total_pick) + u8"\n扔捡率 = " + std::to_string(rate) + u8"\n期望池子大小 = " + std::to_string((int)poolSize) + u8"\n当前移除概率 = " + std::to_string(removeProb) + u8"\n近3天扔捡 = " + last3;
		}

		json getBottles() {
			try { return this->Select("SELECT * FROM bottlemsgrecord where available = 1"); }
			catch (osucat::database_exception) { json j; return  j; }
		}

		int getNumberOfAvailableBottles() {
			try { return this->Select("SELECT * FROM bottlemsgrecord where available = 1").size(); }
			catch (osucat::database_exception) { return -1; }
		}

		json getBottleByID(int id) {
			try { return this->Select("SELECT * FROM bottlemsgrecord where id = " + std::to_string(id)); }
			catch (osucat::database_exception) { json j; return j; }
		}

		bool RemoveBottle(int bottleExsitDays, int bottleid) {
			try {
				json j = this->Select("SELECT * FROM bottletprecord where date=\"" + cqhttp_api::utils::unixTime2DateStr(time(NULL) - 86400) + "\"");
				int length = j.size();
				double poolSize = BOTLEEXPECTEDVALUE;
				double rate = INITIALTPRATE;
				for (int k = 0; k < length; k++) {
					double kpick = stoi(j[k]["pick"].get<std::string>());
					double kthrow = stoi(j[k]["throw"].get<std::string>());
					poolSize = poolSize * 0.8 + 0.2 * kpick * 0.4;  // Adjust the size of pool to around 40% of #average daily pickups
					if (kpick > 0.5) rate = rate * 0.8 + 0.2 * kthrow / kpick;
				}
				if (poolSize > MAXPOOLSIZE) poolSize = MAXPOOLSIZE;
				if (poolSize < MINPOOLSIZE) poolSize = MINPOOLSIZE;
				double removeProb = pow(min(1, rate * pow((double)this->getNumberOfAvailableBottles() / poolSize, 0.7)), 1 / (max(1, bottleExsitDays - BOTTLEMAXEXISTDAYS)));
				if (cqhttp_api::utils::randomNum(1, 10000) / 10000.0 < removeProb) {
					this->writeBottle(osucat::addons::driftingBottleDBEvent::CHANGESTATUS, bottleid, 0, 0, "", "");
					return true;
				}
				else return false;
			}
			catch (osucat::database_exception) {
				try { this->Insert("INSERT INTO bottletprecord (date,throw,pick) values (\"" + cqhttp_api::utils::unixTime2DateStr(time(NULL) - 86400) + "\",100,200)"); }
				catch (osucat::database_exception) {}
				return false;
			}
		}

		//true = pick /false = throw
		void addPickThrowCount(bool pickthrow) {
			json j;
			try {
				j = this->Select("SELECT * FROM bottletprecord where date=\"" + cqhttp_api::utils::unixTime2DateStr(time(NULL)) + "\"");
			}
			catch (osucat::database_exception) {
				this->setNewPickThrowRecord();
				return;
			}
			if (pickthrow) {
				this->Update("UPDATE bottletprecord set pick=" + std::to_string(stoi(j[0]["pick"].get<std::string>()) + 1) + " where date=\"" + cqhttp_api::utils::unixTime2DateStr(time(NULL)) + "\"");
			}
			else {
				this->Update("UPDATE bottletprecord set throw=" + std::to_string(stoi(j[0]["throw"].get<std::string>()) + 1) + " where date=\"" + cqhttp_api::utils::unixTime2DateStr(time(NULL)) + "\"");
			}
		}

		void setNewPickThrowRecord() {
			this->Insert("INSERT INTO bottletprecord (date) values (\"" + cqhttp_api::utils::unixTime2DateStr(time(NULL)) + "\")");
		}

		int getBottleID(int64_t sender, std::string message, time_t time) {
			try { return stoi(this->Select("SELECT id FROM bottlemsgrecord where sendtime=" + std::to_string(time) + " AND sender=" + std::to_string(sender) + " AND message=\"" + message + "\"")[0]["id"].get<std::string>()); }
			catch (osucat::database_exception) { return 0; }
		}

		bool reloadAdmin() {
			try {
				json j = this->Select("SELECT * FROM info where role=1 or role=2");
				adminlist.clear();
				for (int i = 0; i < j.size(); ++i) {
					AdminInfo s;
					s.user_id = stoll(j[i]["qq"].get<std::string>());
					s.role = stoi(j[i]["role"].get<std::string>());
					adminlist.push_back(s);
				}
				return true;
			}
			catch (osucat::database_exception) { return false; }
		}

		/// /// /// /// /// outdated /// /// /// /// ///

		//vector<osucat::steamcheck::CSGOUserInfo> steam_get_csgo_listen_list() {
		//	vector <osucat::steamcheck::CSGOUserInfo> cui;
		//	string query = R"(SELECT * from `steam-ban-check` where IsBanned=0)";
		//	json j;
		//	try {
		//		j = this->Select(query);
		//		for (int i = 0; i < j.size(); ++i) {
		//			osucat::steamcheck::CSGOUserInfo t;
		//			t.SteamId = stoll(j[i]["SteamId"].get<string>());
		//			t.IsBanned = stoi(j[i]["IsBanned"].get<string>()) == 1 ? true : false;
		//			t.ReceiveUserId = j[i]["ReceiveUserId"].get<string>();
		//			t.ReceiveGroupId = j[i]["ReceiveGroupId"].get<string>();
		//			cui.push_back(t);
		//		}
		//	}
		//	catch (osucat::database_exception) {
		//		osucat::steamcheck::CSGOUserInfo t;
		//		t.SteamId = -1;
		//		t.IsBanned = false;
		//		t.ReceiveUserId = "null";
		//		t.ReceiveGroupId = "null";
		//		cui.push_back(t);
		//	}
		//	catch (std::exception& ex) {
		//		cout << ex.what() << endl;
		//	}
		//	return cui;
		//}

		//void steam_change_ban_stats(int64_t SteamId) {
		//	this->Update("UPDATE `steam-ban-check` SET IsBanned=1 WHERE SteamId=" + std::to_string(SteamId));
		//}

		//bool steam_add_ban_listening(int64_t SteamId, int64_t ReceiveUserId = EOF, int64_t ReceiveGroupId = EOF) {
		//	string query = "INSERT INTO `steam-ban-check` (SteamId,IsBanned,ReceiveUserId,ReceiveGroupId) VALUES (";
		//	query += to_string(SteamId) + ",0,";
		//	if (ReceiveUserId == EOF) {
		//		query += "\"null\",";
		//	}
		//	else {
		//		query += "\"" + std::to_string(ReceiveUserId) + "\",";
		//	}
		//	if (ReceiveGroupId == EOF) {
		//		query += "\"null\")";
		//	}
		//	else {
		//		query += "\"" + std::to_string(ReceiveGroupId) + "\")";
		//	}
		//	try {
		//		this->Insert(query);
		//	}
		//	catch (osucat::database_exception) {
		//		//用户已存在于监听列表中
		//		return false;
		//	}
		//	return true;
		//}

		//bool steam_change_ban_listening_receive_groupid(int64_t SteamId, int64_t ReceiveGroupId) {
		//	json j = this->Select("SELECT ReceiveGroupId FROM `steam-ban-check` WHERE SteamId=" + std::to_string(SteamId));
		//	string tmp = j[0]["ReceiveGroupId"].get<string>();
		//	if (tmp.find(';') == string::npos) {
		//		vector<string> t = cqhttp_api::utils::string_split(tmp, ';');
		//		for (int i = 0; i < t.size(); ++i) {
		//			if (t[i] == to_string(ReceiveGroupId)) {
		//				return false;
		//			}
		//		}
		//		this->Update("UPDATE `steam-ban-check` SET ReceiveGroupId=\"" + tmp + ";" + std::to_string(ReceiveGroupId) + "\"" + "WHERE SteamId=" + std::to_string(SteamId));
		//		return true;
		//	}
		//	else {
		//		if (tmp == to_string(ReceiveGroupId)) {
		//			return false;
		//		}
		//		if (tmp == "null") {
		//			this->Update("UPDATE `steam-ban-check` SET ReceiveGroupId=\"" + std::to_string(ReceiveGroupId) + "\"" + "WHERE SteamId=" + std::to_string(SteamId));
		//		}
		//		else {
		//			this->Update("UPDATE `steam-ban-check` SET ReceiveGroupId=\"" + tmp + ";" + std::to_string(ReceiveGroupId) + "\"" + "WHERE SteamId=" + std::to_string(SteamId));
		//		}
		//		return true;
		//	}
		//}

		//bool steam_change_ban_listening_receive_userid(int64_t SteamId, int64_t ReceiveUserId) {
		//	json j = this->Select("SELECT ReceiveUserId FROM `steam-ban-check` WHERE SteamId=" + std::to_string(SteamId));
		//	string tmp = j[0]["ReceiveUserId"].get<string>();
		//	if (tmp.find(';') == string::npos) {
		//		vector<string> t = cqhttp_api::utils::string_split(tmp, ';');
		//		for (int i = 0; i < t.size(); ++i) {
		//			if (t[i] == to_string(ReceiveUserId)) {
		//				return false;
		//			}
		//		}
		//		this->Update("UPDATE `steam-ban-check` SET ReceiveUserId=\"" + tmp + ";" + std::to_string(ReceiveUserId) + "\"" + "WHERE SteamId=" + std::to_string(SteamId));
		//		return true;
		//	}
		//	else {
		//		if (tmp == to_string(ReceiveUserId)) {
		//			return false;
		//		}
		//		if (tmp == "null") {
		//			this->Update("UPDATE `steam-ban-check` SET ReceiveUserId=\"" + std::to_string(ReceiveUserId) + "\"" + "WHERE SteamId=" + std::to_string(SteamId));
		//		}
		//		else {
		//			this->Update("UPDATE `steam-ban-check` SET ReceiveUserId=\"" + tmp + ";" + std::to_string(ReceiveUserId) + "\"" + "WHERE SteamId=" + std::to_string(SteamId));
		//		}
		//		return true;
		//	}
		//}

		void Close() {
			if (this->conn.net.vio != NULL) mysql_close(&this->conn);
		}
		~Database() {
			Close();
		}
	private:
		MYSQL conn;
	};
}
#endif