#pragma once
#include <bits/stdc++.h>
#include <boost/lockfree/spsc_queue.hpp>

template<typename WorkerT, typename MsgT>
class WorkerPool {
    static constexpr size_t kQueueCapacity = 1 << 16;

public:
    explicit WorkerPool(int n_workers = std::max(1u, std::thread::hardware_concurrency())) {
        for (int i = 0; i < n_workers; ++i)
            shards_.push_back(std::make_unique<Shard>(i));
        for (auto& s : shards_)
            s->thread = std::thread([p = s.get()] {
                MsgT msg;
                while (!p->done || !p->queue.empty()) {
                    if (p->queue.pop(msg))
                        p->worker.Process(msg);
                    else
                        std::this_thread::yield();
                }
                p->worker.Close();
            });
    }

    ~WorkerPool() { Close(); }

    void Dispatch(int64_t symbol_key, const MsgT& msg) {
        size_t idx = std::hash<int64_t>{}(symbol_key) % shards_.size();
        while (!shards_[idx]->queue.push(msg))
            std::this_thread::yield();
    }

    void Close() {
        if (closed_) return;
        closed_ = true;
        for (auto& s : shards_) s->done = true;
        for (auto& s : shards_) if (s->thread.joinable()) s->thread.join();
    }

private:
    struct Shard {
        WorkerT worker;
        boost::lockfree::spsc_queue<MsgT, boost::lockfree::capacity<kQueueCapacity>> queue;
        std::thread thread;
        std::atomic<bool> done{false};
        explicit Shard(int idx) : worker(idx) {}
    };

    std::vector<std::unique_ptr<Shard>> shards_;
    bool closed_ = false;
};
