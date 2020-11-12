#pragma once
#ifndef CQHTTP_API_MAIN
#define CQHTTP_API_MAIN

using namespace osucat;

namespace cqhttp_api {
	class Event {

	public:
		static void on_startup() {
			Magick::InitializeMagick(OC_ROOT_PATH);
			osucat::System::_FolderCheck();
			osucat::System::_ConfigCheck();
			osucat::System::_AdminCheck();
			osucat::System::_CreateDUThread();
			steamcat::csgocheck::_AutoCheck();
		}

		static void on_message(const Target target, const GroupSender sender) {
			osucat::CmdParser::parser(target, sender);
		}

		static void on_request(const Request request) {
			switch (request.type) {
			case Request::Type::FRIEND:
				friend_request(request, Request::Event::ACCEPT); //自动同意好友请求
			case Request::Type::GROUP:
				if (request.subtype == Request::SubType::INVITE) //如果群请求事件是被邀请
					friend_request(request, Request::Event::ACCEPT); //自动同意
			default:
				break;
			}
		}
	};
}
#endif

