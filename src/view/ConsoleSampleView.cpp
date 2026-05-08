#include "ConsoleSampleView.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace {
    constexpr int kColId             = 8;
    constexpr int kColName           = 26;  // 한글 시료명 최대 시각 폭 여유
    constexpr int kColProductionTime = 12;
    constexpr int kColYieldRate      = 8;
    constexpr int kSeparatorWidth    = kColId + kColName + kColProductionTime + kColYieldRate + 8;

    // setw는 바이트 기준이므로 한글(3바이트, 시각 2칸)이 섞이면 정렬이 틀어진다.
    // 3바이트 UTF-8 문자(한글) 수 K만큼 setw를 보정한다: setw(K + visualWidth)
    int ksetw(const std::string& text, int visualWidth) {
        int k = 0;
        for (size_t i = 0; i < text.size(); ) {
            unsigned char c = static_cast<unsigned char>(text[i]);
            if      ((c & 0xF0) == 0xE0) { k++; i += 3; }  // 3바이트 UTF-8 (한글)
            else if ((c & 0x80) == 0x00) { i += 1; }         // ASCII
            else if ((c & 0xE0) == 0xC0) { i += 2; }         // 2바이트 UTF-8
            else                          { i += 4; }          // 4바이트 UTF-8
        }
        return k + visualWidth;
    }
}

void ConsoleSampleView::showSubMenu() {
    std::cout << "\n[1] 시료 관리\n";
    std::cout << " [1] 시료 등록   [2] 시료 목록   [3] 시료 검색   [0] 위로\n";
}

int ConsoleSampleView::getMenuChoice() {
    std::cout << "선택 > ";
    int choice = 0;
    std::cin >> choice;
    std::cin.ignore();
    return choice;
}

SampleInput ConsoleSampleView::getRegisterInput() {
    SampleInput input;
    std::cout << "시료 ID > "; std::getline(std::cin, input.id);
    std::cout << "시료명  > "; std::getline(std::cin, input.name);
    std::cout << "평균 생산시간 (min/ea) > "; std::cin >> input.avgProductionTime;
    std::cout << "수율 (0~1.0) > ";           std::cin >> input.yieldRate;
    std::cout << "초기 재고    > ";           std::cin >> input.initialStock;
    std::cin.ignore();
    return input;
}

std::string ConsoleSampleView::getSearchKeyword() {
    std::cout << "검색어 > ";
    std::string kw; std::getline(std::cin, kw);
    return kw;
}

void ConsoleSampleView::showSampleList(const std::vector<Sample>& list, const std::string& header) {
    std::cout << "\n" << header << " (총 " << list.size() << "종)\n";
    // 헤더: 한글 글자 수만큼 setw 보정 → 데이터 행과 시각적 정렬 일치
    std::cout << std::left
              << std::setw(kColId)                          << "ID"
              << std::setw(ksetw("시료명",   kColName))     << "시료명"
              << std::setw(ksetw("생산시간", kColProductionTime)) << "생산시간"
              << std::setw(ksetw("수율",     kColYieldRate)) << "수율"
              << "재고\n";
    std::cout << std::string(kSeparatorWidth, '-') << "\n";
    for (const auto& s : list) {
        std::ostringstream timeStr;
        timeStr << std::fixed << std::setprecision(1) << s.avgProductionTime << " min";
        std::ostringstream yieldStr;
        yieldStr << std::fixed << std::setprecision(2) << s.yieldRate;
        // 데이터 행: s.name도 한글 포함 → ksetw로 보정해야 setw가 초과되지 않음
        std::cout << std::left
                  << std::setw(kColId)                     << s.id
                  << std::setw(ksetw(s.name, kColName))    << s.name
                  << std::setw(kColProductionTime)         << timeStr.str()
                  << std::setw(kColYieldRate)              << yieldStr.str()
                  << s.stock << " ea\n";
    }
}

void ConsoleSampleView::showSuccess(const std::string& msg) { std::cout << "[완료] " << msg << "\n"; }
void ConsoleSampleView::showError(const std::string& msg)   { std::cout << "[오류] " << msg << "\n"; }
