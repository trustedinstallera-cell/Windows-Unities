#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <windows.h>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <random>
#include <sstream>

// 全局时间配置
struct TimeConfig {
    int startHour = 8;      // 起始时间（小时）
    int startMinute = 0;     // 起始时间（分钟）
    int endHour = 23;        // 结束时间（小时）
    int endMinute = 20;      // 结束时间（分钟）

    // 是否启用自定义时间（false则使用默认8:00-23:20）
    bool useCustomTime = false;
};

TimeConfig g_timeConfig;

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
    std::clog << "已经按下：TAB\n";
    PressKey(VK_TAB);
}

void PressSpace() {
    std::clog << "已经按下：SPACE\n";
    PressKey(VK_SPACE);
}

void PressEsc() {
    std::clog << "已经按下：ESC\n";
    PressKey(VK_ESCAPE);
}

void PressAltF4() {
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_MENU;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_F4;
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = VK_F4;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_MENU;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));
}

void OpenWebsite(const std::string& url) {
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void LogOn() {
    std::cout << "执行签到操作..." << std::endl;
    Sleep(5000);
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
    OFF_PERIOD,   // 休息时段（结束时间到起始时间之间，跨夜）
    ACTIVE_PERIOD // 活动时段（起始时间到结束时间之间）
};

// 将时间转换为分钟数
int TimeToMinutes(int hour, int minute) {
    return hour * 60 + minute;
}

// 输入时间配置
void InputTimeConfig() {
    std::string input;
    std::cout << "是否使用默认时间配置（8:00-23:20）？(y/n): ";
    std::getline(std::cin, input);

    if (input == "n" || input == "N") {
        g_timeConfig.useCustomTime = true;

        std::cout << "请输入起始时间（格式：时 分，例如：8 0 或 9 30）: ";
        std::cin >> g_timeConfig.startHour >> g_timeConfig.startMinute;

        std::cout << "请输入结束时间（格式：时 分，例如：23 20 或 22 0）: ";
        std::cin >> g_timeConfig.endHour >> g_timeConfig.endMinute;

        // 清空输入缓冲区
        std::cin.ignore();

        // 验证输入
        g_timeConfig.startHour = std::max(0, std::min(23, g_timeConfig.startHour));
        g_timeConfig.startMinute = std::max(0, std::min(59, g_timeConfig.startMinute));
        g_timeConfig.endHour = std::max(0, std::min(23, g_timeConfig.endHour));
        g_timeConfig.endMinute = std::max(0, std::min(59, g_timeConfig.endMinute));

        std::cout << "\n已设置自定义时间：" << std::endl;
        std::cout << "起始时间: " << std::setw(2) << std::setfill('0')
            << g_timeConfig.startHour << ":"
            << std::setw(2) << std::setfill('0') << g_timeConfig.startMinute << std::endl;
        std::cout << "结束时间: " << std::setw(2) << std::setfill('0')
            << g_timeConfig.endHour << ":"
            << std::setw(2) << std::setfill('0') << g_timeConfig.endMinute << std::endl;
    }
    else {
        g_timeConfig.useCustomTime = false;
        std::cout << "使用默认时间：8:00-23:20" << std::endl;
    }
}

// 获取当前时间段（根据配置的时间）
TimePeriod GetTimePeriod(const std::chrono::system_clock::time_point& now) {
    auto now_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_t);

    int currentMinutes = TimeToMinutes(now_tm.tm_hour, now_tm.tm_min);

    int startMinutes = TimeToMinutes(g_timeConfig.startHour, g_timeConfig.startMinute);
    int endMinutes = TimeToMinutes(g_timeConfig.endHour, g_timeConfig.endMinute);

    // 判断当前时间是否在活动时段
    if (startMinutes < endMinutes) {
        // 正常情况：起始时间 < 结束时间（例如 8:00 到 23:20）
        if (currentMinutes >= startMinutes && currentMinutes < endMinutes) {
            return TimePeriod::ACTIVE_PERIOD;
        }
        else {
            return TimePeriod::OFF_PERIOD;
        }
    }
    else {
     // 跨天情况：起始时间 > 结束时间（例如 22:00 到 6:00）
        if (currentMinutes >= startMinutes || currentMinutes < endMinutes) {
            return TimePeriod::ACTIVE_PERIOD;
        }
        else {
            return TimePeriod::OFF_PERIOD;
        }
    }
}

int getRandomNumber(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

// 显示当前配置信息
void ShowConfig() {
    std::cout << "\n========== 当前配置 ==========" << std::endl;
    if (g_timeConfig.useCustomTime) {
        std::cout << "时间模式: 自定义" << std::endl;
        std::cout << "活动时段: " << std::setw(2) << std::setfill('0')
            << g_timeConfig.startHour << ":"
            << std::setw(2) << std::setfill('0') << g_timeConfig.startMinute
            << " - "
            << std::setw(2) << std::setfill('0') << g_timeConfig.endHour << ":"
            << std::setw(2) << std::setfill('0') << g_timeConfig.endMinute << std::endl;
    }
    else {
        std::cout << "时间模式: 默认（8:00-23:20）" << std::endl;
    }
    std::cout << "活动时段操作: 签到" << std::endl;
    std::cout << "休息时段操作: 签退" << std::endl;
    std::cout << "==============================\n" << std::endl;
}

int main() {
    std::cout << "自动化签到签退程序" << std::endl;
    std::cout << "==================" << std::endl;

    // 输入时间配置
    InputTimeConfig();

    // 显示配置
    ShowConfig();

    std::cout << "程序启动，按 Ctrl+C 退出..." << std::endl;
    std::cout << "========================================" << std::endl;

    TimePeriod status = TimePeriod::UNDEFINED;

    while (true) {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);

        std::cout << "\n当前时间: " << std::put_time(std::localtime(&now_time_t), "%H:%M:%S") << std::endl;

        TimePeriod period = GetTimePeriod(now);

        if (period == TimePeriod::ACTIVE_PERIOD) {
            std::cout << "当前在活动时段" << std::endl;
            if (status != TimePeriod::ACTIVE_PERIOD) {
                status = TimePeriod::ACTIVE_PERIOD;
                LogOn();  // 活动时段执行签到
            }
            else {
                std::cout << "已完成签到，无需重复操作" << std::endl;
            }
        }
        else if (period == TimePeriod::OFF_PERIOD) {
            std::cout << "当前在休息时段" << std::endl;
            if (status != TimePeriod::OFF_PERIOD) {
                status = TimePeriod::OFF_PERIOD;
                LogOff();  // 休息时段执行签退
            }
            else {
                std::cout << "已完成签退，无需重复操作" << std::endl;
            }
        }

        int sleepT = getRandomNumber(20, 300);
        std::cout << "暂停 " << sleepT << " 秒后继续检查..." << std::endl;
        Sleep(sleepT * 1000);
    }

    return 0;
}
