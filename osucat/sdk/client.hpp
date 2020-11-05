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
		case Type::REQUEST:
			Event::on_request(req);
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
		gwsp1->from_url(wsfulladdress);
		std::unique_ptr<easywsclient::WebSocket> ws(easywsclient::WebSocket::from_url(wsfulladdress));
		gwsp = &*ws;
		std::cout << u8"已成功建立连接。" << std::endl;
		while (ws->getReadyState() != easywsclient::WebSocket::CLOSED) {
			easywsclient::WebSocket::pointer wspw = &*ws;
			ws->poll();
			ws->dispatch([wspw](const std::string& message) {
				//if (DEBUGMODE) cout << message << "\n" << endl;
				cout << message << endl;
				thread MultiProcessThread(bind(&Processer, message));
				MultiProcessThread.detach();
				});
		}
	}
}
#endif