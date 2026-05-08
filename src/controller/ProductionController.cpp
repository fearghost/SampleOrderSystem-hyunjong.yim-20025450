#include "ProductionController.h"
#include <stdexcept>

ProductionController::ProductionController(ProductionService& service, ProductionView& view)
    : service_(service), view_(view) {}

void ProductionController::run() {
    auto current = service_.getCurrentJob();
    auto waiting = service_.getWaitingJobs();
    view_.showProductionStatus(current, waiting);

    int action = view_.getMenuChoice();

    if (action == 1) {
        // [1] 생산 완료: 항상 [1] 큐 맨 앞 작업(FIFO) 처리
        try {
            service_.completeCurrentJob();
            view_.showSuccess("생산 완료 처리됐습니다.");
        } catch (const std::exception& e) {
            view_.showError(e.what());
        }
    } else if (action == 2) {
        // [2] 생산 취소: 번호로 작업 선택 후 취소 → 주문 REJECTED
        std::vector<ProductionJob> allJobs;
        if (current.has_value()) allJobs.push_back(*current);
        allJobs.insert(allJobs.end(), waiting.begin(), waiting.end());

        if (allJobs.empty()) {
            view_.showError("취소할 작업이 없습니다.");
            return;
        }

        view_.showCancelPrompt(static_cast<int>(allJobs.size()));
        int cancelIdx = view_.getMenuChoice();
        if (cancelIdx <= 0 || cancelIdx > static_cast<int>(allJobs.size())) return;

        try {
            service_.cancelJob(allJobs[cancelIdx - 1].orderId);
            view_.showSuccess("생산이 취소됐습니다. 주문이 거절 처리됩니다.");
        } catch (const std::exception& e) {
            view_.showError(e.what());
        }
    }
}
