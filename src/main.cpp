#include <windows.h>
#include "repository/JsonSampleRepository.h"
#include "repository/JsonOrderRepository.h"
#include "repository/JsonProductionRepository.h"
#include "service/SampleService.h"
#include "service/OrderService.h"
#include "service/ProductionService.h"
#include "controller/SampleController.h"
#include "controller/OrderController.h"
#include "controller/MonitorController.h"
#include "controller/ProductionController.h"
#include "controller/ReleaseController.h"
#include "view/ConsoleMainView.h"
#include "view/ConsoleSampleView.h"
#include "view/ConsoleOrderView.h"
#include "view/ConsoleMonitorView.h"
#include "view/ConsoleProductionView.h"
#include "view/ConsoleReleaseView.h"

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // Repository 계층 (JSON 파일 기반)
    JsonSampleRepository     sampleRepo;
    JsonOrderRepository      orderRepo;
    JsonProductionRepository productionRepo;

    // Service 계층
    SampleService     sampleSvc(sampleRepo);
    OrderService      orderSvc(sampleRepo, orderRepo, productionRepo);
    ProductionService productionSvc(productionRepo, orderRepo, sampleRepo);

    // View 계층 (콘솔 구현체)
    ConsoleMainView       mainView;
    ConsoleSampleView     sampleView;
    ConsoleOrderView      orderView;
    ConsoleMonitorView    monitorView;
    ConsoleProductionView productionView;
    ConsoleReleaseView    releaseView;

    // Controller 계층
    SampleController     sampleCtrl(sampleSvc,    sampleView);
    OrderController      orderCtrl(orderSvc,       orderView);
    MonitorController    monitorCtrl(sampleSvc,    orderSvc,  monitorView);
    ProductionController productionCtrl(productionSvc, productionView);
    ReleaseController    releaseCtrl(orderSvc,     releaseView);

    while (true) {
        mainView.showMenu(sampleSvc.totalSampleCount(), orderSvc.totalOrderCount());
        switch (mainView.getMenuChoice()) {
            case 1: sampleCtrl.run();        break;
            case 2: orderCtrl.run();         break;
            case 3: orderCtrl.runApproval(); break;
            case 4: monitorCtrl.run();       break;
            case 5: productionCtrl.run();    break;
            case 6: releaseCtrl.run();       break;
            case 0: return 0;
            default: mainView.showError("잘못된 입력입니다.");
        }
    }
}
