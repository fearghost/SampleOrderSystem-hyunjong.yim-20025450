#include "ProductionController.h"
#include <stdexcept>

ProductionController::ProductionController(ProductionService& service, ProductionView& view)
    : service_(service), view_(view) {}

void ProductionController::run() {
    auto current = service_.getCurrentJob();
    auto waiting = service_.getWaitingJobs();
    view_.showProductionStatus(current, waiting);
    int choice = view_.getMenuChoice();
    if (choice == 1) {
        try {
            service_.completeCurrentJob();
            view_.showSuccess("생산 완료 처리됐습니다.");
        } catch (const std::exception& e) {
            view_.showError(e.what());
        }
    }
}
