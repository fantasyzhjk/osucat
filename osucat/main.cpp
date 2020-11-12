#include "main.h"


static unsigned old_code_page;
using namespace std;

int main()
{
	old_code_page = GetConsoleCP();
	SetConsoleCP(CP_UTF8); SetConsoleOutputCP(CP_UTF8);
	GetCurrentDirectory(512, OC_ROOT_PATH); SetCurrentDirectory(OC_ROOT_PATH);
	cout << "Current version: 1.0.0b" << endl; cout << "root path: " << OC_ROOT_PATH << endl; cout << "正在初始化..." << endl;
	Event::on_startup(); //初始化
	cout << u8"准备完成，正在尝试连接.." << endl;
	sprintf_s(wsfulladdress, "ws://%s:%d/", OC_SERVER_HOST, OC_SERVER_PORT);
A:
	try { cqhttp_api::wsclient(); }
	catch (std::exception& ex) { cout << ex.what() << endl; exit(-1); }
	goto A;
	return 0;
}
