#pragma once

#include <atomic>
#include <mutex>
#include <future>
#include "../Engine/Engine.h"
#include "../Vector/Vector2.h"

namespace rt {
    class Dispatcher {
    public:
        explicit                        Dispatcher(Engine const& engine, Vector2<unsigned int> const& res);

        void                            Start(void);
        void                            Stop(void);
        std::vector<Color>              Flush(void);
        std::size_t                     GetNumberOfProcessed(void) const;
        Engine*                         GetEngine() { return &_engine; }

    private:
        void                            execute(void);

        bool                            _running;
        Engine                          _engine;
        Vector2<unsigned int>           _res;
        std::vector<std::thread>        _threads;
        std::mutex                      _image_mutex;
        std::vector<Color>              _image;
        std::atomic<std::size_t>        _sample;

        std::mt19937                           _gen;
        std::normal_distribution<double>       _disX;
        std::normal_distribution<double>       _disY;
    };
}  // namespace rt
