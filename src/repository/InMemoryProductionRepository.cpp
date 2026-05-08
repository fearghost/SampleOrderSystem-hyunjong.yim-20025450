#include "InMemoryProductionRepository.h"
#include <algorithm>

void InMemoryProductionRepository::enqueue(const ProductionJob& job) {
    queue_.push_back(job);
}

std::optional<ProductionJob> InMemoryProductionRepository::front() {
    if (queue_.empty())
        return std::nullopt;
    return queue_.front();
}

void InMemoryProductionRepository::dequeue() {
    if (!queue_.empty())
        queue_.pop_front();
}

void InMemoryProductionRepository::remove(const std::string& orderId) {
    auto it = std::find_if(queue_.begin(), queue_.end(),
        [&](const ProductionJob& j) { return j.orderId == orderId; });
    if (it != queue_.end())
        queue_.erase(it);
}

std::vector<ProductionJob> InMemoryProductionRepository::waitingJobs() {
    if (queue_.size() <= 1)
        return {};
    return std::vector<ProductionJob>(queue_.begin() + 1, queue_.end());
}

bool InMemoryProductionRepository::isEmpty() {
    return queue_.empty();
}

int InMemoryProductionRepository::size() {
    return static_cast<int>(queue_.size());
}
