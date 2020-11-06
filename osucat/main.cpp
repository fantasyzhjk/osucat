#include "main.h"


static unsigned old_code_page;
using namespace std;

int main()
{
	sprintf_s(wshost, "%s", "127.0.0.1");
	sprintf_s(output_prefix, "%s", "[osucat]");
	wsport = 6700;
	cout << "Hello CMake." << endl;
	old_code_page = GetConsoleCP(); //保存当前控制台代码页
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	// version infomation
	cout << "Current version: 1.0b" << endl;
	// initalizing
	cout << "Initializing..." << endl;
	GetCurrentDirectory(512, OC_ROOT_PATH);
	SetCurrentDirectory(OC_ROOT_PATH);
	Magick::InitializeMagick(OC_ROOT_PATH);
	cout << "root path: " << OC_ROOT_PATH << endl;
	if (OC_IS_ACTIVE) {
		cout << u8"准备完成，正在尝试连接.." << endl;
	A:
		sprintf_s(wsfulladdress, "ws://%s:%d/", wshost, wsport);
		cqhttp_api::wsclient();
		goto A;
	}
	else {
		cout << "not active." << endl;
		system("pause");
		return -1;
	}
	return 0;
}
