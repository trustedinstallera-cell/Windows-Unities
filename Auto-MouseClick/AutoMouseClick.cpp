#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <random>


// 模拟按键
void PressKey(WORD keyCode) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = keyCode;

    // 按下
    SendInput(1, &input, sizeof(INPUT));

    // 短暂延迟（模拟真实按键）
    Sleep(50);

    // 释放
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));

    Sleep(50);
}

void PressTab() {
    std::clog << "TAB\n";
    PressKey(VK_TAB);
}

void PressSpace() {
    std::clog << "SPACE\n";
    PressKey(VK_SPACE);
}

void PressEsc() {
    std::clog << "ESC\n";
    PressKey(VK_ESCAPE);
}

void PressAltF4() {
    INPUT inputs[4] = {};  // 需要4个输入事件：按下Alt、按下F4、释放F4、释放Alt

    // 1. 按下 Alt
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_MENU;  // VK_MENU 是 Alt 键

    // 2. 按下 F4
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_F4;

    // 3. 释放 F4
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = VK_F4;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    // 4. 释放 Alt
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_MENU;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    // 发送所有输入事件
    SendInput(4, inputs, sizeof(INPUT));
}

void OpenWebsite(const std::string& url) {
    // ShellExecute 会自动打开默认浏览器
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void LogOn() {
    std::cout << "执行签到操作..." << std::endl;
    OpenWebsite("http://210.43.65.206:8080/fzf/#/SignInCenter");
    Sleep(7500);
    for (int i = 0; i < 10; i++) {
        PressEsc();
    }
    PressTab();
    PressTab();
    PressTab();
    PressTab();
    PressSpace();
    std::cout << "签到完成" << std::endl;
    // system("pause");  // 注释掉，避免阻塞自动化流程
}

void LogOff() {
    std::cout << "执行签退操作..." << std::endl;
    OpenWebsite("http://210.43.65.206:8080/fzf/#/SignInCenter");
    Sleep(7500);
    for (int i = 0; i < 10; i++) {
        PressEsc();
    }
    PressTab();
    PressTab();
    PressTab();
    PressTab();
    PressTab();
    PressSpace();
    std::cout << "签退完成" << std::endl;
}

enum class TimePeriod {
    UNDEFINED,
    NIGHT,   // 23:20 到 8:00
    DAY      // 8:00 到 23:20
};

TimePeriod GetTimePeriod(const std::chrono::system_clock::time_point& now) {
    // 转换为 time_t 以便提取时分
    auto now_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_t);

    int hour = now_tm.tm_hour;
    int minute = now_tm.tm_min;
    int currentMinutes = hour * 60 + minute;

    int nightStart = 23 * 60 + 20;  // 23:20 = 1400 分钟
    int dayStart = 8 * 60;          // 8:00 = 480 分钟

    if (currentMinutes >= nightStart || currentMinutes < dayStart) {
        return TimePeriod::NIGHT;   // 23:20 之后 或 8:00 之前
    }
    else {
        return TimePeriod::DAY;     // 8:00 之后 23:20 之前
    }
}

int getRandomNumber(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

int main() {
    TimePeriod status = TimePeriod::UNDEFINED;

    std::cout << "自动化签到签退程序已启动" << std::endl;
    std::cout << "白天时段（8:00-23:20）：执行签到" << std::endl;
    std::cout << "夜晚时段（23:20-8:00）：执行签退" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    while (true) {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);

        std::cout << "当前时间: " << std::put_time(std::localtime(&now_time_t), "%H:%M:%S") << std::endl;

        TimePeriod period = GetTimePeriod(now);

        // 修正：根据实际时间段判断
        if (period == TimePeriod::DAY) {
            std::cout << "当前在白天时段（8:00-23:20）" << std::endl;
            if (status != TimePeriod::DAY) {
                status = TimePeriod::DAY;
                LogOn();  // 白天执行签到
            }
            else {
                std::cout << "已完成签到，无需重复操作" << std::endl;
            }
        }
        else if (period == TimePeriod::NIGHT) {
            std::cout << "当前在夜晚时段（23:20-8:00）" << std::endl;
            if (status != TimePeriod::NIGHT) {
                status = TimePeriod::NIGHT;
                LogOff();  // 夜晚执行签退
            }
            else {
                std::cout << "已完成签退，无需重复操作" << std::endl;
            }
        }

        int sleepT = getRandomNumber(20, 300);
        std::cout << "暂停 " << sleepT << " 秒后继续检查..." << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        Sleep(sleepT * 1000);
    }

    return 0;
}
