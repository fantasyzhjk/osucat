#pragma once
#ifndef CQHTTP_API
#define CQHTTP_API

char output_prefix[512];
char wsfulladdress[1024];
char wshost[64];
int wsport;
static easywsclient::WebSocket::pointer gwsp = NULL;

namespace cqhttp_api {

	enum class Type {
		INVALID,
		MESSAGE,
		REQUEST,
		EVENT,
	};

	struct Target {
		enum class Type {
			PRIVATE,
			GROUP,
		};
		Type type;
		int64_t time;
		int64_t group_id;
		int64_t user_id;
		int64_t message_id;
		std::string nickname;
		std::string message;
	};

	struct Request {
		enum class Event {
			ACCEPT,
			REJECT,
		};
		enum class Type {
			FRIEND,
			GROUP,
		};
		enum class SubType {
			ADD,
			INVITE,
		};
		SubType subtype;
		Type type;
		int64_t time;
		int64_t group_id;
		int64_t user_id;
		std::string flag;
		std::string message;//附加信息
	};

	struct GroupSender {
		enum class Role {
			OWNER,
			ADMIN,
			MEMBER,
		};
		int age;
		Role role;
		std::optional<std::string> card; //群昵称
		std::string level;
		std::string nickname; //原有用户名
		std::optional<std::string> title; //头衔
		std::string sex;
	};

	struct PictureInfo {
		int32_t size; //图片大小
		std::string filename; //原始文件名
		std::string url; //图片下载地址
		std::string format; //图片格式 .jpg .png .bmp .gif
	};



	void send(json j) {
		gwsp->send(j.dump());
	}

	void send_private_message(Target tar, const std::string msg) {
		tar.message = msg;
		json j;
		std::string tmp;
		j["action"] = "send_private_msg";
		j["params"]["user_id"] = tar.user_id;
		tar.message.empty() ? j["params"]["message"] = "null" : j["params"]["message"] = tar.message;
		tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 发送至好友 " + to_string(tar.user_id) + u8"的消息：";
		tmp += tar.message;
		std::cout << tmp << std::endl;
		send(j);
	}

	void send_group_message(Target tar, const std::string msg) {
		tar.message = msg;
		json j;
		std::string tmp;
		j["action"] = "send_group_msg";
		j["params"]["group_id"] = tar.group_id;
		tar.message.empty() ? j["params"]["message"] = "null" : j["params"]["message"] = tar.message;
		tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 发送至群 " + to_string(tar.group_id) + u8"的消息：";
		tmp += tar.message;
		std::cout << tmp << std::endl;
		send(j);
	}

	void send_private_message(int64_t user_id, const std::string msg) {
		json j;
		std::string tmp;
		j["action"] = "send_private_msg";
		j["params"]["user_id"] = user_id;
		msg.empty() ? j["params"]["message"] = "null" : j["params"]["message"] = msg;
		tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 发送至好友 " + to_string(user_id) + u8"的消息：";
		tmp += msg;
		std::cout << tmp << std::endl;
		send(j);
	}

	void send_group_message(int64_t group_id, const std::string msg) {
		json j;
		std::string tmp;
		j["action"] = "send_group_msg";
		j["params"]["group_id"] = group_id;
		msg.empty() ? j["params"]["message"] = "null" : j["params"]["message"] = msg;
		tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 发送至群 " + to_string(group_id) + u8"的消息：";
		tmp += msg;
		std::cout << tmp << std::endl;
		send(j);
	}

	void send_message(Target tar, const std::string msg) {
		if (tar.type == Target::Type::GROUP) {
			send_group_message(tar, msg);
		}
		else if (tar.type == Target::Type::PRIVATE) {
			send_private_message(tar, msg);
		}
	}

	void friend_request(const Request req, const Request::Event event) {
		if (event == Request::Event::ACCEPT) {
			json j;
			std::string tmp;
			j["action"] = "set_friend_add_request";
			j["params"]["flag"] = req.flag;
			j["params"]["approve"] = true;
			tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 已通过 " + to_string(req.user_id) + u8"的好友请求。";
			std::cout << tmp << std::endl;
			send(j);
		}
		else if (event == Request::Event::REJECT) {
			json j;
			std::string tmp;
			j["action"] = "set_friend_add_request";
			j["params"]["flag"] = req.flag;
			j["params"]["approve"] = false;
			tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 已拒绝 " + to_string(req.user_id) + u8"的好友请求。";
			std::cout << tmp << std::endl;
			send(j);
		}
	}

	void group_request(const Request req, const Request::Event event) {
		if (event == Request::Event::ACCEPT) {
			json j;
			std::string tmp;
			j["action"] = "set_group_add_request";
			j["params"]["flag"] = req.flag;
			req.subtype == Request::SubType::INVITE ? j["params"]["sub_type"] = "invite" : j["params"]["sub_type"] = "add";
			j["params"]["approve"] = true;
			req.subtype == Request::SubType::INVITE
				? tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 已通过 " + to_string(req.user_id) + u8"的群 (" + to_string(req.group_id) + u8") 邀请请求。"
				: tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 已通过 " + to_string(req.user_id) + u8"的群 (" + to_string(req.group_id) + u8") 添加请求。";
			std::cout << tmp << std::endl;
			send(j);
		}
		else if (event == Request::Event::REJECT) {
			json j;
			std::string tmp;
			j["action"] = "set_group_add_request";
			j["params"]["flag"] = req.flag;
			req.subtype == Request::SubType::INVITE ? j["params"]["sub_type"] = "invite" : j["params"]["sub_type"] = "add";
			j["params"]["approve"] = true;
			req.subtype == Request::SubType::INVITE
				? tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 已拒绝 " + to_string(req.user_id) + u8"的群 (" + to_string(req.group_id) + u8") 邀请请求。"
				: tmp += u8"[" + utils::unixTime2Str(time(NULL)) + u8"] " + output_prefix + u8"[↑]: 已拒绝 " + to_string(req.user_id) + u8"的群 (" + to_string(req.group_id) + u8") 添加请求。";
			std::cout << tmp << std::endl;
			send(j);
		}
	}

	PictureInfo getImage(std::string file_name) {
		json jp;
		jp["file"] = file_name;
		PictureInfo rtn;
		rtn.size = 0;
		rtn.filename = "";
		rtn.url = "";
		rtn.format = "";
		try {
			std::string data = osucat::NetConnection::HttpPost("http://127.0.0.1:5700/get_image", jp);
			json j = json::parse(data)["data"];
			rtn.size = j["size"].get<int32_t>();
			rtn.filename = j["filename"].get<std::string>();
			rtn.url = j["url"].get<std::string>();
			rtn.format = rtn.filename.substr(rtn.filename.length() - 4);
		}
		catch (osucat::NetWork_Exception& ex) {
			std::cout << ex.Show() << std::endl;
		}
		char msg[4096];
		sprintf_s(msg,
			u8"[%s] %s[↓]: 接收图片 %s | 图片格式：%s | 大小: %ld | URL: %s",
			utils::unixTime2Str(time(NULL)).c_str(), output_prefix,
			rtn.filename.c_str(),
			rtn.format,
			rtn.size,
			rtn.url.c_str());
		std::cout << msg << std::endl;
		return rtn;
	}

	Type Parser(std::string rawstr, Target* target, GroupSender* sender, Request* request) {
		if (rawstr.find("user_id") != std::string::npos) {
			cqhttp_api::Target t;
			json j = json::parse(rawstr);
			cqhttp_api::Type type;
			if (j["post_type"].get<std::string>() == "message") {
				type = cqhttp_api::Type::MESSAGE;
			}
			else if (j["post_type"].get<std::string>() == "request") {
				type = cqhttp_api::Type::REQUEST;
			}
			else return cqhttp_api::Type::INVALID;
			if (type == cqhttp_api::Type::MESSAGE) {
				cqhttp_api::Target tar;
				cqhttp_api::GroupSender sdr;
				json jm = json::parse(rawstr)["sender"];
				if (j["message_type"].get<std::string>() == "private") {
					tar.type = cqhttp_api::Target::Type::PRIVATE;
				}
				else if (j["message_type"].get<std::string>() == "group") {
					tar.type = cqhttp_api::Target::Type::GROUP;
					tar.group_id = j["group_id"].get<int64_t>();
					sdr.card = jm["card"].get<std::string>();
					sdr.title = jm["title"].get<std::string>();
				}
				sdr.age = jm["age"].get<int>();
				sdr.nickname = jm["nickname"].get<std::string>();
				tar.nickname = sdr.nickname;
				tar.user_id = j["user_id"].get<int64_t>();
				tar.time = j["time"].get<int64_t>();
				tar.message = j["message"].get<std::string>();
				std::string msg;
				if (tar.type == cqhttp_api::Target::Type::PRIVATE) {
					msg += u8"[" + utils::unixTime2Str(tar.time) + u8"] " + output_prefix + u8"[↓]: 好友 " + to_string(tar.user_id) + u8" 的消息：";
					msg += tar.message;
				}
				else if (tar.type == cqhttp_api::Target::Type::GROUP) {
					msg += u8"[" + utils::unixTime2Str(tar.time) + u8"] " + output_prefix + u8"[↓]: 群 " + to_string(tar.group_id) + u8" 的 " + to_string(tar.user_id) + u8" 的消息：";
					msg += tar.message;
				}
				std::cout << msg << std::endl;
				*target = tar;
				*sender = sdr;
				return type;
			}
			else if (type == cqhttp_api::Type::REQUEST) {
				cqhttp_api::Request r;
				cqhttp_api::Request::Type rt;
				std::string msg;
				r.message = j["comment"].get<std::string>();
				r.user_id = j["user_id"].get<int64_t>();
				r.flag = j["flag"].get<std::string>();
				r.time = j["time"].get<int64_t>();
				if (j["request_type"].get<std::string>() == "friend") {
					rt = cqhttp_api::Request::Type::FRIEND;
					r.type = cqhttp_api::Request::Type::FRIEND;
					msg += u8"[" + utils::unixTime2Str(r.time) + u8"] " + output_prefix + u8"[↓]: 收到来自用户 " + to_string(r.user_id) + u8" 的好友请求";
					if (!r.message.empty())msg += u8"附加消息：" + r.message;
				}
				else if (j["request_type"].get<std::string>() == "group") {
					rt = cqhttp_api::Request::Type::GROUP;
					r.type = cqhttp_api::Request::Type::GROUP;
					r.subtype = j["request_type"].get<std::string>() == "invite" ? cqhttp_api::Request::SubType::INVITE : cqhttp_api::Request::SubType::ADD;
					msg += u8"[" + utils::unixTime2Str(r.time) + u8"] " + output_prefix + u8"[↓]: 收到来自用户 " + to_string(r.user_id) + u8" 的群邀请";
					if (!r.message.empty())msg += u8"附加消息：" + r.message;
				}
				std::cout << msg << std::endl;
				*request = r;
				return type;
			}
		}
		return cqhttp_api::Type::INVALID;
	}
}
#endif