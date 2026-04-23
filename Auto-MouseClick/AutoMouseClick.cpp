#include <windows.h>
#include <iostream>

int main() {
    std::cout << "输入持续时间/分钟:";
    double tm;
    std::cin >> tm;

    int totalSeconds = tm * 60;
    std::cout << "将在 " << tm << " 分钟后执行鼠标点击..." << std::endl;
    std::cout << "请将鼠标移动到目标位置" << std::endl;

    // 每秒显示一次倒计时（可选）
    for (int i = totalSeconds; i > 0; i--) {
        //if (i % 10 == 0 || i <= 5) {  // 每10秒和最后5秒显示
            std::cout << "剩余 " << i << " 秒..." << std::endl;
       // }
        Sleep(1000);
        system("cls");
    }

    std::cout << "执行点击！" << std::endl;

    INPUT input = { 0 };
    input.type = INPUT_MOUSE;

    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));

    Sleep(50);

    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));

    std::cout << "Done." << std::endl;
    system("pause");
    return 0;
}
