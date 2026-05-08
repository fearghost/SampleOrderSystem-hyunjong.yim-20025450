#pragma once
#include "../service/ProductionService.h"
#include "../view/ProductionView.h"

class ProductionController {
public:
    ProductionController(ProductionService& service, ProductionView& view);
    void run();

private:
    ProductionService& service_;
    ProductionView&    view_;
};
