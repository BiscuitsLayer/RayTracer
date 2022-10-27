#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <cstring>
#include <regex>
#include "Loader/AssimpLoader.h"
#include "Camera/Camera.h"
#include "Engine/Engine.h"
#include "Dispatcher/Dispatcher.h"
#include "Vector/Vector2.h"

void displayToScreen(rt::Dispatcher &dispatcher, rt::Vector2<unsigned int> const& res) {
    sf::RenderWindow window(sf::VideoMode(res.X, res.Y), "RayTracer");
    sf::Uint8* frame = new sf::Uint8[window.getSize().x * window.getSize().y * 4];
    std::fill_n(frame, window.getSize().x * window.getSize().y * 4, 0xff); // Init with all component to 255
    sf::Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    sf::Sprite sprite(texture);

    bool shouldClear = false;
    while (window.isOpen()) {
        std::size_t i = 0;
        std::vector<rt::Color> pixels = dispatcher.Flush();

        for (auto& pixel : pixels) {
            if (shouldClear) {
                rt::Color_Component black{};
                pixel.SetColor(black);
            }
            rt::Color_Component const& components = pixel.GetColor();
            frame[i] = components.rgba.r;
            frame[i + 1] = components.rgba.g;
            frame[i + 2] = components.rgba.b;
            i = i + 4;
        }
        shouldClear = false;

        texture.update(frame);
        window.clear();
        window.draw(sprite);
        window.display();

        rt::Engine* engine = dispatcher.GetEngine();
        rt::Camera* camera = engine->GetCamera();

        sf::Event event;
        while (window.pollEvent(event)) {
            if ((event.type == sf::Event::Closed) || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
                dispatcher.Stop();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W) {
                std::cout << camera->GetPos() << std::endl;
                camera->MoveForward();
                shouldClear = true;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                std::cout << camera->GetPos() << std::endl;
                camera->MoveLeft();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                std::cout << camera->GetPos() << std::endl;
                camera->MoveBack();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                std::cout << camera->GetPos() << std::endl;
                camera->MoveRight();
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

    rt::Vector2<unsigned int> res = engine.GetRes();
    rt::Dispatcher dispatcher(engine, res);
    dispatcher.Start();

    displayToScreen(dispatcher, res);
    return 0;
}
