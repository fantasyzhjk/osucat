#pragma once
#ifndef CQHTTP_API_MAIN
#define CQHTTP_API_MAIN

using namespace osucat;

namespace cqhttp_api {
	class Event {
	public:
		static void on_message(const Target target, const GroupSender sender) {
			//gwsp->close();
			//echo(target, sender);
		}

		static void on_request(const Request request) {
			switch (request.type) {
			case Request::Type::FRIEND:
				friend_request(request, Request::Event::ACCEPT);
			case Request::Type::GROUP:
				if (request.subtype == Request::SubType::INVITE)
					friend_request(request, Request::Event::ACCEPT);
			default:
				break;
			}
		}
	};
}
#endif

