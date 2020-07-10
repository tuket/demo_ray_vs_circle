#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>

using glm::vec2;

const int START_WINDOW_WIDTH = 800;
const int START_WINDOW_HEIGHT = 600;
const vec2 circlePos = {START_WINDOW_WIDTH/2, START_WINDOW_HEIGHT/2};
const float circleRad = 100;

vec2 rayStart = {0, 0};
vec2 rayEnd = {0, 0};

sf::Font font;
sf::Text text;
char buffer[4*1024];

bool rayVsCircle(float& depth,
    glm::vec2 rayOrig, glm::vec2 rayDir,
    glm::vec2 circlePos, float circleRad)
{
    const float R = circleRad;
    const glm::vec2 op = circlePos - rayOrig;
    const float D = dot(rayDir, op);
    const float H2 = dot(op, op) - D*D;
    const float K2 = R*R - H2;

    char* buf = buffer + sprintf(buffer,
        "rayOrig: {%f, %f}\n"
        "rayDir: {%f, %f}\n"
        "circlePos: {%f, %f}\n"
        "R: %f\n"
        "op: {%f, %f}\n"
        "D: %f\nH2: %f\nK2: %f\n",
        rayOrig.x, rayOrig.y,
        rayDir.x, rayDir.y,
        circlePos.x, circlePos.y,
        R,
        op.x, op.y,
        D, H2, K2);

    if(dot(op, op) < R*R) {
        sprintf(buf, "rayOrigin is inside the circle");
        depth = 0;
        return true;
    }

    if(K2 >= 0) {
        depth = D - sqrt(K2);
        sprintf(buf, "depth: %f", depth);
        if(depth > 0)
            return true;
    }
    return false;
}

int main()
{
    if (!font.loadFromFile("SourceCodePro-Regular.ttf"))
    {
        printf("Error loading font\n");
        return 1;
    }
    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor({255, 255, 255});

    sf::RenderWindow window(sf::VideoMode(START_WINDOW_WIDTH, START_WINDOW_HEIGHT),
        "ray vs circle", sf::Style::Titlebar | sf::Style::Close);
    
    bool prevMousePressed = false;

    sf::CircleShape shape(circleRad);
    shape.setOrigin(circleRad, circleRad);
    shape.setPosition(circlePos.x, circlePos.y);
    shape.setFillColor(sf::Color(0, 255, 0));

    sf::CircleShape dot1(3);
    dot1.setOrigin(3, 3);
    dot1.setFillColor({0, 255, 255});

    sf::CircleShape dot2(3);
    dot2.setOrigin(3, 3);
    dot2.setFillColor({255, 255, 0});

    while(window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        const bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        const auto windowMousePos = sf::Mouse::getPosition(window);
        const vec2 mousePos = {windowMousePos.x, windowMousePos.y};
        if(rayStart != rayEnd) {
            float depth;
            bool intersects = rayVsCircle(depth, rayStart, normalize(rayEnd - rayStart), circlePos, circleRad);
            text.setString(buffer);
            if(intersects)
                shape.setFillColor(sf::Color(0, 255, 0));
            else
                shape.setFillColor(sf::Color(255, 0, 0));
        }
        else
            shape.setFillColor(sf::Color(255, 0, 0));

        if(mousePressed) {
            if(!prevMousePressed) {
                rayStart = mousePos;
            }
            rayEnd = mousePos;
        }
        prevMousePressed = mousePressed;

        dot1.setPosition(rayStart.x, rayStart.y);
        dot2.setPosition(rayEnd.x, rayEnd.y);

        window.clear();

        window.draw(shape);
        window.draw(dot1);
        window.draw(dot2);
        if(rayStart != rayEnd) {
            vec2 rayDir = glm::normalize(rayEnd - rayStart);
            vec2 rayLongEnd = rayStart + 1000.f * rayDir;
            sf::Vertex line[] = {
                sf::Vertex({rayStart.x, rayStart.y}),
                sf::Vertex({rayLongEnd.x, rayLongEnd.y})
            };
            window.draw(line, 2, sf::Lines);
        }
        window.draw(text);
        window.display();
        sf::sleep(sf::seconds(0.01));
    }
}
