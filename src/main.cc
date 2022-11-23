#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <cstring>
#include <regex>
#include "Engine/Constant.h"
#include "Loader/AssimpLoader.h"
#include "Camera/Camera.h"
#include "Engine/Engine.h"
#include "Vector/Vector2.h"

rt::Vector2<unsigned int> res{};
std::size_t size{};
std::vector<rt::Color> pixels{};

std::mt19937 gen;
std::normal_distribution<double> disY;
std::normal_distribution<double> disX;

void Init() {
    size = res.Y * res.X;
    pixels = std::vector<rt::Color>(size, rt::Color(0x000000ff));
    disY = std::normal_distribution<double>(res.Y / 2, res.Y / 2);
    disX = std::normal_distribution<double>(res.X / 2, res.X / 2);
}

void Flush() {
    pixels.clear();
    pixels.reserve(size);
    pixels.resize(size, rt::Color(0x000000ff));
}

void Execute(rt::Engine& engine) {
    size_t current = (int)disY(gen) * rt::Constant::DefaultScreenWidth + (int)disX(gen);
    current = current % size;

    rt::Color color = engine.Raytrace(rt::Vector2<unsigned int>(current % res.X, current / res.X));
    pixels[current] = color;
}

class Demo {
public:
    Demo(rt::Camera* camera) : camera_(camera), launched_(false) {
    }

    bool isOn() const {
        return launched_;
    }
    void TurnOn() {
        launched_ = true;
    }
    void TurnOff() {
        launched_ = false;
    }
    void Run() {
        if (launched_) {
            if (counter_++ % 5 == 0) {
                if ((counter_ < 100) || (counter_ > 300)) {
                    Flush();
                    camera_->TurnLeft();
                    camera_->MoveRight();
                    camera_->MoveRight();
                    camera_->MoveRight();
                } else {
                    Flush();
                    camera_->TurnRight();
                    camera_->MoveLeft();
                    camera_->MoveLeft();
                    camera_->MoveLeft();
                }
                if (counter_ >= 400) {
                    counter_ = 0;
                }
            }
        }
    }

private:
    int counter_ = 0;
    rt::Camera* camera_ = nullptr;
    bool launched_ = false;
};

void displayToScreen(rt::Engine &engine, rt::Vector2<unsigned int> const& res) {
    sf::VideoMode video_mode{sf::Vector2u{res.X, res.Y}};
    sf::RenderWindow window{video_mode, "RayTracer"};
    uint8_t* frame = new uint8_t[window.getSize().x * window.getSize().y * 4];
    std::fill_n(frame, window.getSize().x * window.getSize().y * 4, 255);
    sf::Texture texture;
    texture.create(window.getSize());
    sf::Sprite sprite(texture);

    rt::Camera* camera = engine.GetCamera();
    Demo demo{camera};

    while (window.isOpen()) {
        std::size_t i = 0;

        demo.Run();

        for (int cnt = 0; cnt < 10'000; ++cnt) {
            Execute(engine);
        }

        for (auto& pixel : pixels) {
            rt::Color_Component const& components = pixel.GetColor();
            frame[i] = components.rgba.r;
            frame[i + 1] = components.rgba.g;
            frame[i + 2] = components.rgba.b;
            i = i + 4;
        }

        texture.update(frame);
        window.clear();
        window.draw(sprite);
        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            if ((event.type == sf::Event::Closed) || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W) {
                std::cout << camera->GetPos() << std::endl;
                Flush();
                camera->MoveForward();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                std::cout << camera->GetPos() << std::endl;
                Flush();
                camera->MoveLeft();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                std::cout << camera->GetPos() << std::endl;
                Flush();
                camera->MoveBack();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                std::cout << camera->GetPos() << std::endl;
                Flush();
                camera->MoveRight();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left) {
                std::cout << camera->GetPos() << std::endl;
                Flush();
                camera->TurnLeft();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right) {
                std::cout << camera->GetPos() << std::endl;
                Flush();
                camera->TurnRight();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                if (demo.isOn()) {
                    demo.TurnOff();
                } else {
                    demo.TurnOn();
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " scene.dae" << std::endl;
        return 1;
    }

    rt::AssimpLoader loader;

    std::cout << "Loading scene " << argv[1] << "..." << std::endl;
    if (!loader.LoadFile(argv[1])) {
        return 1;
    }

    rt::Engine engine{loader};

    //rt::Vector2<unsigned int> res = engine.GetRes();
    res = engine.GetRes();
    Init();

    displayToScreen(engine, res);
    return 0;
}
