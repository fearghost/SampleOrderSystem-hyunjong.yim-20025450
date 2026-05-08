#include "OrderService.h"
#include <stdexcept>
#include <chrono>
#include <sstream>
#include <iomanip>

OrderService::OrderService(ISampleRepository&     samples,
                           IOrderRepository&      orders,
                           IProductionRepository& production)
    : samples_(samples), orders_(orders), production_(production) {}

void OrderService::placeOrder(const std::string& sampleId,
                              const std::string& customerName,
                              int                quantity,
                              const std::string& orderId,
                              const std::string& createdAt) {
    if (!samples_.existsById(sampleId))
        throw std::invalid_argument("존재하지 않는 시료 ID: " + sampleId);

    Order o;
    o.orderId       = orderId;
    o.sampleId      = sampleId;
    o.customerName  = customerName;
    o.quantity      = quantity;
    o.status        = OrderStatus::RESERVED;
    o.createdAt     = createdAt;
    orders_.save(o);
}

void OrderService::approveOrder(const std::string& orderId) {
    auto orderOpt = orders_.findById(orderId);
    if (!orderOpt.has_value())
        throw std::invalid_argument("존재하지 않는 주문: " + orderId);

    const Order order = *orderOpt;
    if (order.status != OrderStatus::RESERVED)
        throw std::logic_error("RESERVED 상태 주문만 승인할 수 있습니다.");

    auto sampleOpt = samples_.findById(order.sampleId);
    if (!sampleOpt.has_value())
        throw std::invalid_argument("존재하지 않는 시료: " + order.sampleId);

    const Sample sample = *sampleOpt;

    if (sample.stock >= order.quantity) {
        orders_.updateStatus(orderId, OrderStatus::CONFIRMED);
        samples_.updateStock(order.sampleId, -order.quantity);
    } else {
        int    shortage   = order.quantity - sample.stock;
        int    actualQty  = calcActualQty(shortage, sample.yieldRate);
        double totalTime  = sample.avgProductionTime * actualQty;

        ProductionJob job;
        job.orderId   = orderId;
        job.sampleId  = order.sampleId;
        job.shortage  = shortage;
        job.actualQty = actualQty;
        job.totalTime = totalTime;

        production_.enqueue(job);
        orders_.updateStatus(orderId, OrderStatus::PRODUCING);
        if (sample.stock > 0)
            samples_.updateStock(order.sampleId, -sample.stock);
    }
}

void OrderService::rejectOrder(const std::string& orderId) {
    auto orderOpt = orders_.findById(orderId);
    if (!orderOpt.has_value())
        throw std::invalid_argument("존재하지 않는 주문: " + orderId);

    orders_.updateStatus(orderId, OrderStatus::REJECTED);
}

void OrderService::processShipment(const std::string& orderId) {
    auto orderOpt = orders_.findById(orderId);
    if (!orderOpt.has_value())
        throw std::invalid_argument("존재하지 않는 주문: " + orderId);

    if (orderOpt->status != OrderStatus::CONFIRMED)
        throw std::logic_error("CONFIRMED 상태 주문만 출고할 수 있습니다.");

    orders_.updateStatus(orderId, OrderStatus::RELEASE);
}

void OrderService::completeProduction(const std::string& orderId) {
    auto orderOpt = orders_.findById(orderId);
    if (!orderOpt.has_value())
        throw std::invalid_argument("존재하지 않는 주문: " + orderId);

    orders_.updateStatus(orderId, OrderStatus::CONFIRMED);
}

std::vector<Order> OrderService::listByStatus(OrderStatus status) const {
    return orders_.findByStatus(status);
}

std::vector<Order> OrderService::listAll() const {
    return orders_.findAll();
}

int OrderService::totalOrderCount() const {
    return orders_.count();
}

int OrderService::calcActualQty(int shortage, double yieldRate) {
    return static_cast<int>(std::ceil(static_cast<double>(shortage) / (yieldRate * 0.9)));
}

std::string OrderService::generateTimestamp() {
    auto now  = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}
