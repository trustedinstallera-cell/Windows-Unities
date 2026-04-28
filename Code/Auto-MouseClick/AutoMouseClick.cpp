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
    int logoffHour = 23;     // 签退时间（小时）
    int logoffMinute = 20;   // 签退时间（分钟）

    // 是否启用自定义时间（false则使用默认8:00-23:20）
    bool useCustomTime = false;
};

TimeConfig g_timeConfig;

// 记录上次签退日期
std::string lastLogoffDate = "";
// 记录上次签到日期
std::string lastLogonDate = "";

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

// 将时间转换为分钟数
int TimeToMinutes(int hour, int minute) {
    return hour * 60 + minute;
}

// 获取当前日期字符串 YYYY-MM-DD
std::string GetCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_t);
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d");
    return oss.str();
}

// 检查是否在签退时间点附近（随机偏差，模拟人工）
bool IsNearLogoffTime(const std::chrono::system_clock::time_point& now) {
    auto now_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_t);

    int currentMinutes = TimeToMinutes(now_tm.tm_hour, now_tm.tm_min);
    int logoffMinutes = TimeToMinutes(g_timeConfig.logoffHour, g_timeConfig.logoffMinute);

    // 在签退时间前后10分钟范围内，模拟人工操作的随机性
    int diff = abs(currentMinutes - logoffMinutes);
    return diff <= 10;
}

// 检查是否在活动时段内
bool IsActivePeriod(const std::chrono::system_clock::time_point& now) {
    auto now_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_t);

    int currentMinutes = TimeToMinutes(now_tm.tm_hour, now_tm.tm_min);
    int startMinutes = TimeToMinutes(g_timeConfig.startHour, g_timeConfig.startMinute);
    int endMinutes = TimeToMinutes(g_timeConfig.endHour, g_timeConfig.endMinute);

    if (startMinutes < endMinutes) {
        return currentMinutes >= startMinutes && currentMinutes < endMinutes;
    }
    else {
     // 跨天情况
        return currentMinutes >= startMinutes || currentMinutes < endMinutes;
    }
}

// 输入时间配置
void InputTimeConfig() {
    std::string input;
    std::cout << "是否使用默认时间配置（活动:8:00-23:20 签退:23:20）？(y/n): ";
    std::getline(std::cin, input);

    if (input == "n" || input == "N") {
        g_timeConfig.useCustomTime = true;

        std::cout << "请输入活动起始时间（格式：时 分，例如：8 0 或 9 30）: ";
        std::cin >> g_timeConfig.startHour >> g_timeConfig.startMinute;

        std::cout << "请输入活动结束时间（格式：时 分，例如：23 20 或 16 10）: ";
        std::cin >> g_timeConfig.endHour >> g_timeConfig.endMinute;

        std::cout << "请输入每日签退时间（格式：时 分，例如：23 20）: ";
        std::cin >> g_timeConfig.logoffHour >> g_timeConfig.logoffMinute;

        // 清空输入缓冲区
        std::cin.ignore();

        // 验证输入
        g_timeConfig.startHour = std::max(0, std::min(23, g_timeConfig.startHour));
        g_timeConfig.startMinute = std::max(0, std::min(59, g_timeConfig.startMinute));
        g_timeConfig.endHour = std::max(0, std::min(23, g_timeConfig.endHour));
        g_timeConfig.endMinute = std::max(0, std::min(59, g_timeConfig.endMinute));
        g_timeConfig.logoffHour = std::max(0, std::min(23, g_timeConfig.logoffHour));
        g_timeConfig.logoffMinute = std::max(0, std::min(59, g_timeConfig.logoffMinute));

        std::cout << "\n已设置自定义时间：" << std::endl;
        std::cout << "活动时段: " << std::setw(2) << std::setfill('0')
            << g_timeConfig.startHour << ":"
            << std::setw(2) << std::setfill('0') << g_timeConfig.startMinute
            << " - "
            << std::setw(2) << std::setfill('0') << g_timeConfig.endHour << ":"
            << std::setw(2) << std::setfill('0') << g_timeConfig.endMinute << std::endl;
        std::cout << "每日签退时间: " << std::setw(2) << std::setfill('0')
            << g_timeConfig.logoffHour << ":"
            << std::setw(2) << std::setfill('0') << g_timeConfig.logoffMinute << std::endl;
    }
    else {
        g_timeConfig.useCustomTime = false;
        std::cout << "使用默认配置：" << std::endl;
        std::cout << "活动时段: 8:00-23:20" << std::endl;
        std::cout << "每日签退时间: 23:20" << std::endl;
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
        std::cout << "每日签退时间: " << std::setw(2) << std::setfill('0')
            << g_timeConfig.logoffHour << ":"
            << std::setw(2) << std::setfill('0') << g_timeConfig.logoffMinute << std::endl;
    }
    else {
        std::cout << "时间模式: 默认" << std::endl;
        std::cout << "活动时段: 08:00 - 23:20" << std::endl;
        std::cout << "每日签退时间: 23:20" << std::endl;
    }
    std::cout << "活动时段操作: 签到" << std::endl;
    std::cout << "每日指定时间操作: 签退（前后10分钟随机检查）" << std::endl;
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

    while (true) {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);

        std::cout << "\n当前时间: " << std::put_time(std::localtime(&now_time_t), "%H:%M:%S") << std::endl;

        std::string currentDate = GetCurrentDate();
        bool isActive = IsActivePeriod(now);

        // 检查1：是否在签退时间点附近（优先执行签退）
        if (IsNearLogoffTime(now) && lastLogoffDate != currentDate) {
            std::cout << "接近每日签退时间点，执行签退操作" << std::endl;
            LogOff();
            lastLogoffDate = currentDate;
        }
        // 检查2：是否在活动时段内
        else if (isActive) {
            std::cout << "当前在活动时段" << std::endl;
            if (lastLogonDate != currentDate) {
                LogOn();
                lastLogonDate = currentDate;
            }
            else {
                std::cout << "已完成今日签到，无需重复操作" << std::endl;
            }
        }
        else {
            std::cout << "当前在非活动时段，等待签退时间点..." << std::endl;
        }

        // 随机等待时间范围：30-300秒，更接近人工操作间隔
        int sleepT = getRandomNumber(30, 300);
        std::cout << "暂停 " << sleepT << " 秒后继续检查..." << std::endl;
        Sleep(sleepT * 1000);
    }

    return 0;
}
