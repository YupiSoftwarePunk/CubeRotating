#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/glu.h>
#include <iostream>
#include <cmath>
#include <algorithm>

struct Rotation {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

Rotation rotation;
bool isFilled = true;
float rotationSpeed = 2.0f;
bool autoRotate = false;

bool isUnfolded = false;
float currentUnfoldAngle = 0.0f;
float zoomFactor = 1.0f;


void drawFaceNumber(int face) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(3.0f);

    glBegin(GL_LINES);
    float offset = 1.01f;

    if (face == 0) { // 1 (Передняя, +Z)
        glVertex3f(0.0f, 0.5f, offset);  glVertex3f(0.0f, -0.5f, offset);
        glVertex3f(-0.2f, 0.3f, offset); glVertex3f(0.0f, 0.5f, offset);
    }
    else if (face == 1) { // 2 (Правая, +X)
        glVertex3f(offset, 0.5f, -0.2f); glVertex3f(offset, 0.5f, 0.2f);
        glVertex3f(offset, 0.5f, 0.2f);  glVertex3f(offset, 0.0f, 0.2f);
        glVertex3f(offset, 0.0f, 0.2f);  glVertex3f(offset, 0.0f, -0.2f);
        glVertex3f(offset, 0.0f, -0.2f); glVertex3f(offset, -0.5f, -0.2f);
        glVertex3f(offset, -0.5f, -0.2f); glVertex3f(offset, -0.5f, 0.2f);
    }
    else if (face == 2) { // 3 (Задняя, -Z)
        glVertex3f(0.2f, 0.5f, -offset);  glVertex3f(-0.2f, 0.5f, -offset);
        glVertex3f(-0.2f, 0.5f, -offset); glVertex3f(-0.2f, 0.0f, -offset);
        glVertex3f(0.2f, 0.0f, -offset);  glVertex3f(-0.2f, 0.0f, -offset);
        glVertex3f(-0.2f, 0.0f, -offset); glVertex3f(-0.2f, -0.5f, -offset);
        glVertex3f(0.2f, -0.5f, -offset); glVertex3f(-0.2f, -0.5f, -offset);
    }
    else if (face == 3) { // 4 (Левая, -X)
        glVertex3f(-offset, 0.5f, -0.2f); glVertex3f(-offset, 0.0f, -0.2f);
        glVertex3f(-offset, 0.0f, -0.2f); glVertex3f(-offset, 0.0f, 0.2f);
        glVertex3f(-offset, 0.5f, 0.2f);  glVertex3f(-offset, -0.5f, 0.2f);
    }
    else if (face == 4) { // 5 (Верхняя, +Y)
        glVertex3f(-0.2f, offset, -0.2f); glVertex3f(0.2f, offset, -0.2f);
        glVertex3f(-0.2f, offset, -0.2f); glVertex3f(-0.2f, offset, 0.0f);
        glVertex3f(-0.2f, offset, 0.0f);  glVertex3f(0.2f, offset, 0.0f);
        glVertex3f(0.2f, offset, 0.0f);   glVertex3f(0.2f, offset, 0.2f);
        glVertex3f(0.2f, offset, 0.2f);   glVertex3f(-0.2f, offset, 0.2f);
    }
    else if (face == 5) { // 6 (Нижняя, -Y)
        glVertex3f(0.2f, -offset, -0.2f); glVertex3f(-0.2f, -offset, -0.2f);
        glVertex3f(-0.2f, -offset, -0.2f); glVertex3f(-0.2f, -offset, 0.2f);
        glVertex3f(-0.2f, -offset, 0.2f);  glVertex3f(0.2f, -offset, 0.2f);
        glVertex3f(0.2f, -offset, 0.2f);   glVertex3f(0.2f, -offset, 0.0f);
        glVertex3f(0.2f, -offset, 0.0f);   glVertex3f(-0.2f, -offset, 0.0f);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}


void drawCube() {
    GLfloat vertices[8][3] = {
        {-1.0f, -1.0f,  1.0f}, // 0 - передняя нижняя левая
        { 1.0f, -1.0f,  1.0f}, // 1 - передняя нижняя правая
        { 1.0f,  1.0f,  1.0f}, // 2 - передняя верхняя правая
        {-1.0f,  1.0f,  1.0f}, // 3 - передняя верхняя левая
        {-1.0f, -1.0f, -1.0f}, // 4 - задняя нижняя левая
        { 1.0f, -1.0f, -1.0f}, // 5 - задняя нижняя правая
        { 1.0f,  1.0f, -1.0f}, // 6 - задняя верхняя правая
        {-1.0f,  1.0f, -1.0f}  // 7 - задняя верхняя левая
    };

    GLint faces[6][4] = {
        {0, 1, 2, 3},
        {1, 5, 6, 2},
        {5, 4, 7, 6},
        {4, 0, 3, 7},
        {3, 2, 6, 7},
        {4, 5, 1, 0}
    };

    GLfloat normals[6][3] = {
        {0, 0, 1},
        {1, 0, 0},
        {0, 0, -1},
        {-1, 0, 0},
        {0, 1, 0},
        {0, -1, 0}
    };

    if (isFilled) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (int face = 0; face < 6; face++) {
        glPushMatrix();


        if (currentUnfoldAngle > 0.0f) {
            if (face == 0) {
                glTranslatef(0.0f, -1.0f, 1.0f); glRotatef(currentUnfoldAngle, 1, 0, 0); glTranslatef(0.0f, 1.0f, -1.0f);
            }
            else if (face == 2) {
                glTranslatef(0.0f, -1.0f, -1.0f); glRotatef(-currentUnfoldAngle, 1, 0, 0); glTranslatef(0.0f, 1.0f, 1.0f);
            }
            else if (face == 1) {
                glTranslatef(1.0f, -1.0f, 0.0f); glRotatef(-currentUnfoldAngle, 0, 0, 1); glTranslatef(-1.0f, 1.0f, 0.0f);
            }
            else if (face == 3) {
                glTranslatef(-1.0f, -1.0f, 0.0f); glRotatef(currentUnfoldAngle, 0, 0, 1); glTranslatef(1.0f, 1.0f, 0.0f);
            }
            else if (face == 4) {
                glTranslatef(0.0f, -1.0f, 1.0f); glRotatef(currentUnfoldAngle, 1, 0, 0); glTranslatef(0.0f, 1.0f, -1.0f); // Движение вместе с передней
                glTranslatef(0.0f, 1.0f, 1.0f); glRotatef(currentUnfoldAngle, 1, 0, 0); glTranslatef(0.0f, -1.0f, -1.0f);  // Собственное откидывание
            }
        }

        glBegin(GL_QUADS);
        glNormal3fv(normals[face]);

        // Цвета граней
        float r = (sin(rotation.x * 0.05f) + 1) / 2;
        float g = (sin(rotation.y * 0.05f) + 1) / 2;
        float b = (sin(rotation.z * 0.05f) + 1) / 2;
        glColor3f(r, g, b);

        for (int vertex = 0; vertex < 4; vertex++) {
            glVertex3fv(vertices[faces[face][vertex]]);
        }
        glEnd();

        drawFaceNumber(face);

        glPopMatrix();
    }
}


void setupOpenGL(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat aspect = static_cast<GLfloat>(width) / height;
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightPos[] = { 3.0f, 5.0f, 4.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
}

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;

    sf::Window window(
        sf::VideoMode({ 1024, 768 }),
        "3D Cube Control",
        sf::Style::Default,
        sf::State::Windowed,
        settings
    );

    window.setVerticalSyncEnabled(true);
    setupOpenGL(1024, 768);

    bool running = true;
    sf::Clock clock;

    while (running) {
        float deltaTime = clock.restart().asSeconds();

        // Анимация развертки
        if (isUnfolded && currentUnfoldAngle < 90.0f) {
            currentUnfoldAngle += 100.0f * deltaTime;
            if (currentUnfoldAngle > 90.0f) currentUnfoldAngle = 90.0f;
        }
        else if (!isUnfolded && currentUnfoldAngle > 0.0f) {
            currentUnfoldAngle -= 100.0f * deltaTime;
            if (currentUnfoldAngle < 0.0f) currentUnfoldAngle = 0.0f;
        }

        float currentSpeed = rotationSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RShift)) {
            currentSpeed *= 3.0f; // Ускорение при Shift
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RControl)) {
            currentSpeed *= 0.3f; // Замедление при Ctrl
        }

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                running = false;
            }
            else if (event->is<sf::Event::Resized>()) {
                if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                    glViewport(0, 0, resized->size.x, resized->size.y);
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    GLfloat aspect = static_cast<GLfloat>(resized->size.x) / resized->size.y;
                    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
                    glMatrixMode(GL_MODELVIEW);
                }
            }
            else if (event->is<sf::Event::KeyPressed>()) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::C) {
                        isUnfolded = !isUnfolded; // С - Развертка куба
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                        zoomFactor = std::max(0.1f, zoomFactor - 0.1f); // R - Приблизить
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::L) {
                        zoomFactor += 0.1f; // L - Отдалить
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                        isFilled = !isFilled; // Пробел - прозрачность/заливка
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::T) {
                        autoRotate = !autoRotate; // T - Автоповорот 
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::Backspace) {
                        rotation.x = 0; rotation.y = 0; rotation.z = 0; // Backspace - сброс поворота
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::Equal) {
                        rotationSpeed += 1.0f; // Базовое ускорение на "="
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::Hyphen) {
                        rotationSpeed = std::max(1.0f, rotationSpeed - 1.0f); // Базовое замедление на "-"
                    }
                }
            }
        }

        float smoothSpeed = currentSpeed * deltaTime * 100.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) {
            rotation.y -= smoothSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) {
            rotation.y += smoothSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) {
            rotation.x -= smoothSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) {
            rotation.x += smoothSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Q)) {
            rotation.z -= smoothSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::E)) {
            rotation.z += smoothSpeed;
        }

        if (autoRotate) {
            rotation.y += currentSpeed * deltaTime * 50.0f;
            rotation.x += (currentSpeed * 0.7f) * deltaTime * 50.0f;
            rotation.z += (currentSpeed * 0.5f) * deltaTime * 50.0f;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        gluLookAt(4.0 * zoomFactor, 3.0 * zoomFactor, 5.0 * zoomFactor,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);

        glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
        glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
        glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);

        drawCube();

        window.display();
        sf::sleep(sf::milliseconds(10));
    }

    return 0;
}