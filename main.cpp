#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <chrono>

//Compile Command: g++ -std=c++17 -Wall main.cpp -o main.exe -lsfml-graphics -lsfml-window -lsfml-system

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

    sf::Texture busTexture;
    loaded = busTexture.loadFromFile("marker.png");
    if(!loaded){
        throw "marker.png failed to load";
    }
    sf::Sprite busSprite(busTexture);
    busSprite.setOrigin({50.f, 50.f});
    busSprite.setScale({0.4, 0.4});

    sf::Texture personSignTexture;
    loaded = personSignTexture.loadFromFile("personWaiting.png");
    if(!loaded){
        throw "personWaiting.png failed to load";
    }
    sf::Sprite personSignSprite(personSignTexture);
    personSignSprite.setOrigin({100.f, 100.f});
    personSignSprite.setScale({0.15, 0.15});

    std::vector<sf::Vector2f> personSignPositions;
    personSignPositions.push_back({1088, 1086});
    personSignPositions.push_back({1175, 548});
    personSignPositions.push_back({1043, 53});
    personSignPositions.push_back({1420, 491});
    personSignPositions.push_back({1375, 832});

    sf::Font font;
    loaded = font.openFromFile("times.ttf");
    if(!loaded){
        throw "times.ttf failed to load";
    }
    sf::Text controlText(font);
    controlText.setString("Controls:\n\nUse keys 1-5 to signal that a pedestrian is\nwaiting at a bus stop");
    controlText.setCharacterSize(36);
    controlText.setFillColor(sf::Color::Black);
    controlText.setPosition({1800.f, 400.f});

    sf::Text stopText(font);
    stopText.setString("Stop ");
    stopText.setCharacterSize(36);
    stopText.setFillColor(sf::Color::Blue);

    std::vector<sf::Vector2f> textPositions;
    textPositions.push_back({988, 1136});
    textPositions.push_back({1060, 598});
    textPositions.push_back({1003, 123});
    textPositions.push_back({1430, 541});
    textPositions.push_back({1405, 862});

    std::vector<sf::CircleShape> stopRings;

    sf::CircleShape ring(10.f);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(8.f);
    ring.setOutlineColor(sf::Color::Transparent);
    ring.setOrigin({10.f, 10.f});

    ring.setPosition({1088.f, 1136.f});
    stopRings.push_back(ring);
    ring.setPosition({1175.f, 598.f});
    stopRings.push_back(ring);
    ring.setPosition({1043.f, 103.f});
    stopRings.push_back(ring);
    ring.setPosition({1420.f, 541.f});
    stopRings.push_back(ring);
    ring.setPosition({1375.f, 882.f});
    stopRings.push_back(ring);

    std::vector<float> ringAlphas = {0, 0, 0, 0, 0};

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
    for(size_t i = 0; i < stopPaths[currentPathIndex].size(); i++){
        pathQueue.push(stopPaths[currentPathIndex][i]);
    }

    //1088, 1136 is the initial position at bus stop 1
    sf::Vector2f currentPosition = {1088, 1136};
    sf::Vector2f nextPosition = pathQueue.front();
    pathQueue.pop();
    sf::Vector2f velocity = (nextPosition - currentPosition).normalized();
    float speed = 60;
    float blipSpeed = 25;

    //Test before sprite added
    //sf::busSpriteShape busSprite(sf::Vector2f(50.0f, 50.0f));
    //busSprite.setFillColor(sf::Color::Red);
    busSprite.setPosition(currentPosition); 

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
                if (keyEvent->code == sf::Keyboard::Key::Num1 && !shouldStop[0]) {
                    shouldStop[0] = true;
                    stopRings[0].setRadius(7.f);
                    stopRings[0].setOutlineColor(sf::Color::Red);
                    ringAlphas[0] = 255;
                } else if (keyEvent->code == sf::Keyboard::Key::Num2 && !shouldStop[1]) {
                    shouldStop[1] = true;
                    stopRings[1].setRadius(7.f);
                    stopRings[1].setOutlineColor(sf::Color::Red);
                    ringAlphas[1] = 255;
                } else if (keyEvent->code == sf::Keyboard::Key::Num3 && !shouldStop[2]) {
                    shouldStop[2] = true;
                    stopRings[2].setRadius(7.f);
                    stopRings[2].setOutlineColor(sf::Color::Red);
                    ringAlphas[2] = 255;
                } else if (keyEvent->code == sf::Keyboard::Key::Num4 && !shouldStop[3]) {
                    shouldStop[3] = true;
                    stopRings[3].setRadius(7.f);
                    stopRings[3].setOutlineColor(sf::Color::Red);
                    ringAlphas[3] = 255;
                } else if (keyEvent->code == sf::Keyboard::Key::Num5 && !shouldStop[4]) {
                    shouldStop[4] = true;
                    stopRings[4].setRadius(7.f);
                    stopRings[4].setOutlineColor(sf::Color::Red);
                    ringAlphas[4] = 255;
                }
            }
        }

        if(!stopping){
            currentPosition += velocity * (speed * deltaTime);
            busSprite.setPosition(currentPosition);

            if((currentPosition - nextPosition).length() < 1){
                currentPosition = nextPosition;
                if(pathQueue.empty()){
                    int stoppingIndex = (currentPathIndex + 1) % 5;

                    if(shouldStop[stoppingIndex]){
                        stopping = true;
                        shouldStop[stoppingIndex] = false;
                        initialTime = std::chrono::steady_clock::now();

                        stopRings[stoppingIndex].setRadius(7.f);
                        stopRings[stoppingIndex].setOutlineColor(sf::Color::Green);
                        ringAlphas[stoppingIndex] = 255;
                    }
                    currentPathIndex = (currentPathIndex + 1) % 5;
                    for(size_t i = 0; i < stopPaths[currentPathIndex].size(); i++){
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

        window.clear(sf::Color::White);

        window.draw(backGroundSprite);
        for(size_t i = 0; i < stopRings.size(); i++){
            float radius = stopRings[i].getRadius();
            if(radius < 20){
                stopRings[i].setRadius(radius + blipSpeed * deltaTime);
                float newRadius = stopRings[i].getRadius();
                stopRings[i].setOrigin({newRadius, newRadius});
            }
            if(ringAlphas[i] > 0){
                ringAlphas[i] -= blipSpeed * 10 * deltaTime;
                sf::Color currentColor = stopRings[i].getOutlineColor();
                currentColor.a = ringAlphas[i];
                stopRings[i].setOutlineColor(currentColor);
            }
            window.draw(stopRings[i]);
        }

        for(size_t i = 0; i < personSignPositions.size(); i++){
            if(shouldStop[i]){
                personSignSprite.setPosition(personSignPositions[i]);
                window.draw(personSignSprite);
            }
        }
        for(size_t i = 0; i < textPositions.size(); i++){
            stopText.setString("Stop " + std::to_string(i + 1));
            stopText.setPosition(textPositions[i]);
            window.draw(stopText);
        }
        window.draw(busSprite);
        window.draw(controlText);

        window.display();
    }
    return 0;
}