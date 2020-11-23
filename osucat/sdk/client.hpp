#pragma once
#ifndef CQHTTP_API_DATA
#define CQHTTP_API_DATA
namespace cqhttp_api {
	void Processer(std::string rawstr) {
		Target tar;
		GroupSender sdr;
		Request req;
		Type type = Parser(rawstr, &tar, &sdr, &req);
		switch (type) {
		case Type::MESSAGE:
			Event::on_message(tar, sdr);
			break;
		case Type::REQUEST:
			Event::on_request(req);
			break;
		default:
			break;
		}
	}
	void wsclient() {
		INT rc;
		WSADATA wsaData;
		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			printf("WSAStartup Failed.\n");
			system("pause");
			return;
		}
		std::unique_ptr<easywsclient::WebSocket> ws(easywsclient::WebSocket::from_url(wsfulladdress));
		gwsp = &*ws;
		if (NULL == ws) {
			return;
		}
		else {
			cout << u8"已成功建立连接。" << endl;
		}
		while (ws->getReadyState() != easywsclient::WebSocket::CLOSED) {
			easywsclient::WebSocket::pointer wspw = &*ws;
			ws->poll();
			ws->dispatch([wspw](const std::string& message) {
				if (OC_DEBUG_MODE) cout << message << "\n" << endl;
				thread MultiProcessThread(bind(&Processer, message));
				MultiProcessThread.detach();
				});
		}
		cout << u8"已失去连接，正在重试..." << endl;
	}
}
#endif