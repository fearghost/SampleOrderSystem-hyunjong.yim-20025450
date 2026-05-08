#pragma once
#include "../service/SampleService.h"
#include "../view/SampleView.h"

class SampleController {
public:
    SampleController(SampleService& service, SampleView& view);
    void run();

private:
    SampleService& service_;
    SampleView&    view_;

    void handleRegister();
    void handleList();
    void handleSearch();
};
