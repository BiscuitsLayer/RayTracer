#include <thread>
#include <set>
#include "Dispatcher.h"
#include "../Engine/Constant.h"

namespace rt {
    Dispatcher::Dispatcher(const Engine &engine, Vector2<unsigned int> const& res) : _running(false), _engine(engine), _res(res), _sample(0) {
        std::size_t size = _res.Y * _res.X;
        _image.reserve(size);
        _image.resize(size, Color(0x000000ff));
        _gen = std::mt19937{std::random_device()()};
        _disY = std::normal_distribution<double>(_res.Y / 2, _res.Y / 2);
        _disX = std::normal_distribution<double>(_res.X / 2, _res.X / 2);
    }

    void Dispatcher::Start() {
        _running = true;
        std::size_t cores = std::thread::hardware_concurrency();
        while (cores--) {
            _threads.emplace_back(std::thread(&Dispatcher::execute, this));
        }
        std::cout << "Dispatching over " << _threads.size() << " threads..." << std::endl;
    }

    void Dispatcher::Stop() {
        std::cout << "Shutting down threads..." << std::endl;
        _running = false;
        for (auto & thread : _threads) {
            thread.join();
        }
        _threads.clear();
    }

    std::vector<Color> Dispatcher::Flush() {
        return _image;
    }

    std::size_t Dispatcher::GetNumberOfProcessed() const {
        return _sample;
    }

    void Dispatcher::execute() {
        std::size_t size = _res.Y * _res.X;

        while (_running) {
            std::size_t current = (int)_disY(_gen) * Constant::DefaultScreenWidth + (int)_disX(_gen);
            current = current % size;

            Color color = _engine.Raytrace(Vector2<unsigned int>(current % _res.X, current / _res.X));
            _image_mutex.lock();
            _image[current] = color;
            _image_mutex.unlock();
        }
    }
}  // namespace rt
