#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <thread>
#include <cmath>
#include <iostream>
#include <cstdint>

struct Vec3 {
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 1) : x(x), y(y), z(z) {}  // W modelu Poincarégo z > 0

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }

    Vec3 normalize() const { 
        float len = sqrt(x*x + y*y + z*z); 
        return (*this) * (1.0f / len); 
    }
};
inline double dot(const Vec3& a, const Vec3& b)
{
    return (a.x*b.x + a.y*b.y + a.z*b.z);
}

// Ray w przestrzeni Poincarégo
struct Ray {
    Vec3 origin, direction;
    
    Ray(Vec3 o, Vec3 d) : origin(o), direction(d.normalize()) {}

    // Przekształcenie promienia do modelu Poincarégo
    Vec3 at(float t) const {
        return Vec3(origin.x + direction.x * t, 
                    origin.y + direction.y * t, 
                    origin.z * exp(t));  // Geodezyjne w modelu Poincarégo
    }
};

Vec3 light(5,5,-5);

//podstawowy materiał
struct PerfectDiffuse
{
    sf::Color materialColor = sf::Color::Black;
    PerfectDiffuse(sf::Color materialColor)
    {
        this->materialColor = materialColor;
    }

    sf::Color shade(Vec3 hit, Vec3 normal)
    {
        sf::Color totalColor = sf::Color::Black;
        
        Vec3 inDirection =  (light - hit).normalize();
        double diffuseFactor = dot(inDirection, normal);
        if (diffuseFactor < 0) { return totalColor; }

        totalColor = sf::Color(materialColor.r * diffuseFactor, 
                               materialColor.g * diffuseFactor,
                               materialColor.b * diffuseFactor);
        
        return totalColor;
    }

};

// Kula w przestrzeni hiperbolicznej
struct HyperbolicSphere {
    Vec3 center;
    float radius;
    sf::Color color;

    HyperbolicSphere(Vec3 c, float r, sf::Color col) : center(c), radius(r), color(col) {}

    bool intersect(const Ray& ray, float& t) const {
        // Przekształcenie do przestrzeni hiperbolicznej
        Vec3 oc = ray.origin - center;
        float b = 2.0f * (oc.x * ray.direction.x + oc.y * ray.direction.y + oc.z * ray.direction.z);
        float c = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - radius * radius;
        
        float disc = b * b - 4 * c;
        if (disc < 0) return false;

        t = (-b - sqrt(disc)) / 2;
        return t > 0;
    }
};

const unsigned int WIDTH = 800, HEIGHT = 600;
std::vector<std::uint8_t> pixels(WIDTH * HEIGHT * 4);
HyperbolicSphere hyperbolicSphere(Vec3(0, 0, -3), 0.5f, sf::Color::Green);
Vec3 camera(0, 0, 0);

void render(int startY, int endY) {
    for (int y = startY; y < endY; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Vec3 camera(0, 0.5f, 4.0f);  // Pozycja oka (kamery) nad płaszczyzną

            float u = (x - WIDTH / 2.0f) / WIDTH;
            float v = (y - HEIGHT / 2.0f) / HEIGHT;
            
            Vec3 target(u, v, -1.0f);  // Punkt na ekranie przed kamerą (np. w z = 1)
            Vec3 direction = (target - camera).normalize();  // Nowy kierunek promienia

            Ray ray(camera, direction);  // Teraz promienie wychodzą z jednego punktu
                
            float t, min = INT16_MAX;
            sf::Color color = sf::Color::Black;

            if (hyperbolicSphere.intersect(ray, t)) {
                color = hyperbolicSphere.color;
                Vec3 hitPoint = ray.at(t);
                Vec3 normal = (hitPoint - hyperbolicSphere.center).normalize();

                // Tworzymy obiekt materiału i wywołujemy shade()
                PerfectDiffuse material(hyperbolicSphere.color);
                std::cout << "(" << x << "," << y << ")" << " " << color.r << " " << color.g << " " << color.b << "\n";
                color = material.shade(hitPoint, normal);
                std::cout << "(" << x << "," << y << ")" << " " << color.r << " " << color.g << " " << color.b << "\n\n\n";
            }

            int index = (y * WIDTH + x) * 4;
            pixels[index] = color.r;
            pixels[index + 1] = color.g;
            pixels[index + 2] = color.b;
            pixels[index + 3] = 255;
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "HSRT");
    sf::Texture texture(sf::Vector2u{WIDTH, HEIGHT});
    sf::Sprite sprite(texture);
    
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        
        std::vector<std::thread> threads;
        int numThreads = 4;
        for (int i = 0; i < numThreads; i++) {
            threads.emplace_back(render, i * (HEIGHT / numThreads), (i + 1) * (HEIGHT / numThreads));
        }
        for (auto &t : threads) t.join();
        
        texture.update(pixels.data());
        window.clear();
        window.draw(sprite);
        window.display();
    }
    return 0;
}
