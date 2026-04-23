#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <chrono>

//Compile Command: g++ -std=c++17 -Wall main.cpp -o main.exe   -lsfml-graphics -lsfml-window -lsfml-system

int main(){
    sf::RenderWindow window(sf::VideoMode({2560, 1440}), "SFML Window");
    sf::View view(sf::FloatRect({0.f, 0.f}, {2560, 1440}));
    window.setView(view);

    sf::Texture backGroundTexture;
    bool loaded = backGroundTexture.loadFromFile("route.jpg");
    if(!loaded){
        throw "route.jpg failed to load";
    }
    sf::Sprite backGroundSprite(backGroundTexture);
    backGroundSprite.setScale({0.58, 0.58});
    float halfWidth = backGroundSprite.getLocalBounds().size.x / 2;
    backGroundSprite.setPosition({1280 - halfWidth * 0.58, 0});

    std::vector<std::vector<sf::Vector2f>> stopPaths;

    std::vector<sf::Vector2f> stopOneToTwo;
    stopOneToTwo.push_back({1120, 830});
    stopOneToTwo.push_back({1040, 706});
    stopOneToTwo.push_back({1150, 694});
    stopOneToTwo.push_back({1175, 598});

    std::vector<sf::Vector2f> stopTwoToThree;
    stopTwoToThree.push_back({1268, 537});
    stopTwoToThree.push_back({1158, 384});
    stopTwoToThree.push_back({1166, 334});
    stopTwoToThree.push_back({973, 317});
    stopTwoToThree.push_back({995, 106});
    stopTwoToThree.push_back({1043, 103});
    
    std::vector<sf::Vector2f> stopThreeToFour;
    stopThreeToFour.push_back({1213, 101});
    stopThreeToFour.push_back({1225, 122});
    stopThreeToFour.push_back({1307, 160});
    stopThreeToFour.push_back({1307, 179});
    stopThreeToFour.push_back({1534, 174});
    stopThreeToFour.push_back({1531, 363});
    stopThreeToFour.push_back({1420, 541});

    std::vector<sf::Vector2f> stopFourToFive;
    stopFourToFive.push_back({1352, 658});
    stopFourToFive.push_back({1339, 754});
    stopFourToFive.push_back({1375, 882});

    std::vector<sf::Vector2f> stopFiveToOne;
    stopFiveToOne.push_back({1486, 1213});
    stopFiveToOne.push_back({1406, 1323});
    stopFiveToOne.push_back({1118, 1360});
    stopFiveToOne.push_back({1088, 1136});

    stopPaths.push_back(stopOneToTwo);
    stopPaths.push_back(stopTwoToThree);
    stopPaths.push_back(stopThreeToFour);
    stopPaths.push_back(stopFourToFive);
    stopPaths.push_back(stopFiveToOne);

    int currentPathIndex = 0;

    std::queue<sf::Vector2f> pathQueue;
    for(int i = 0; i < stopPaths[currentPathIndex].size(); i++){
        pathQueue.push(stopPaths[currentPathIndex][i]);
    }

    //1088, 1136 is the initial position at bus stop 1
    sf::Vector2f currentPosition = {1088, 1136};
    sf::Vector2f nextPosition = pathQueue.front();
    pathQueue.pop();
    sf::Vector2f velocity = (nextPosition - currentPosition).normalized();
    float speed = 200;

    //Test before sprite added
    sf::RectangleShape rectangle(sf::Vector2f(50.0f, 50.0f));
    rectangle.setFillColor(sf::Color::Green);
    rectangle.setPosition(currentPosition);
    rectangle.setOrigin({25.f, 25.f}); 

    bool shouldStop[5] = {};

    bool stopping = false;

    sf::Clock deltaClock;
    auto initialTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();

    while(window.isOpen()){
        float deltaTime = deltaClock.restart().asSeconds();

        while(auto event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){
                window.close();
            }
            if (auto* resized = event->getIf<sf::Event::Resized>()) {
                // Compute the aspect ratio
                float windowRatio = float(resized->size.x) / resized->size.y;
                float viewRatio   = 2560 / 1440;
                float sizeX = 2560;
                float sizeY = 1440;
                float offsetX = 0.f;
                float offsetY = 0.f;

                if (windowRatio > viewRatio) {
                    // Window is wider than 16:9
                    sizeX = 1440 * windowRatio;
                    offsetX = (sizeX - 2560) / 2.f;
                } else if (windowRatio < viewRatio) {
                    // Window is taller than 16:9
                    sizeY = 2560 / windowRatio;
                    offsetY = (sizeY - 1440) / 2.f;
                }

                view.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f})); // use full window
                view.setSize({sizeX, sizeY});
                view.setCenter({2560 / 2.f + offsetX, 1440 / 2.f + offsetY});
                window.setView(view);
            }
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Num1) {
                    shouldStop[0] = true;
                } else if (keyEvent->code == sf::Keyboard::Key::Num2) {
                    shouldStop[1] = true;
                } else if (keyEvent->code == sf::Keyboard::Key::Num3) {
                    shouldStop[2] = true;
                } else if (keyEvent->code == sf::Keyboard::Key::Num4) {
                    shouldStop[3] = true;
                } else if (keyEvent->code == sf::Keyboard::Key::Num5) {
                    shouldStop[4] = true;
                }
            }
        }

        if(!stopping){
            currentPosition += velocity * (speed * deltaTime);
            rectangle.setPosition(currentPosition);

            if((currentPosition - nextPosition).length() < 1){
                currentPosition = nextPosition;
                if(pathQueue.empty()){
                    int stoppingIndex = (currentPathIndex + 1) % 5;

                    if(shouldStop[stoppingIndex]){
                        stopping = true;
                        shouldStop[stoppingIndex] = false;
                        initialTime = std::chrono::steady_clock::now();
                    }
                    currentPathIndex = (currentPathIndex + 1) % 5;
                    for(int i = 0; i < stopPaths[currentPathIndex].size(); i++){
                        pathQueue.push(stopPaths[currentPathIndex][i]);
                    }
                }
                nextPosition = pathQueue.front();
                pathQueue.pop();
                velocity = (nextPosition - currentPosition).normalized();
            }
        } else {
            currentTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - initialTime);
            if(duration.count() >= 2.5){
                stopping = false;
            }
        }

        window.clear();

        window.draw(backGroundSprite);
        window.draw(rectangle);

        window.display();
    }
    return 0;
}