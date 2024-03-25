#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

// Forward declaration of classes
class GameObject;
class Fruit;
class Obstacle;
class Player;

// Class representing any game object
class GameObject {
protected:
    int x, y; // Position of the object
public:
    GameObject(int maxX, int maxY) {
        x = rand() % maxX;
        y = rand() % maxY;
    }
    
    // Virtual destructor
    virtual ~GameObject() {}
    
    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }
};

// Class representing a fruit
class Fruit : public GameObject {
public:
    Fruit(int maxX, int maxY) : GameObject(maxX, maxY) {}
};

// Class representing an obstacle
class Obstacle : public GameObject {
public:
    Obstacle(int maxX, int maxY) : GameObject(maxX, maxY) {}
};

// Class representing the player
class Player {
private:
    int x, y; // Position of the player
    int score;
public:
    Player(int maxX, int maxY) : score(0) {
        x = maxX / 2;
        y = maxY / 2;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    int getScore() const {
        return score;
    }

    void move(char direction) {
        switch(direction) {
            case 'w':
                y--;
                break;
            case 's':
                y++;
                break;
            case 'a':
                x--;
                break;
            case 'd':
                x++;
                break;
            default:
                break;
        }
    }

    bool checkCollision(const GameObject& object) const {
        return (x == object.getX() && y == object.getY());
    }

    void increaseScore() {
        score++;
    }
};

// Function to check if the given position is valid (not occupied by any object)
bool isValidPosition(const std::vector<GameObject*>& objects, int x, int y) {
    for (const GameObject* obj : objects) {
        if (obj->getX() == x && obj->getY() == y) {
            return false;
        }
    }
    return true;
}

int main() {
    const int maxX = 10; // Width of the game area
    const int maxY = 10; // Height of the game area
    const int maxFruits = 5; // Maximum number of fruits in the game
    const int maxObstacles = 10; // Maximum number of obstacles in the game
    const int gameDuration = 60; // Game duration in seconds

    std::vector<GameObject*> objects;
    srand(time(nullptr)); // Seed the random number generator

    // Initialize fruits
    for (int i = 0; i < maxFruits; ++i) {
        objects.push_back(new Fruit(maxX, maxY));
    }

    // Initialize obstacles
    for (int i = 0; i < maxObstacles; ++i) {
        int x, y;
        do {
            x = rand() % maxX;
            y = rand() % maxY;
        } while (!isValidPosition(objects, x, y));
        objects.push_back(new Obstacle(maxX, maxY));
    }

    Player player(maxX, maxY);

    char moveDirection;
    auto startTime = std::chrono::steady_clock::now();
    int elapsedTime = 0;

    // Game loop
    while (elapsedTime < gameDuration) {
        // Display the game area
        for (int i = 0; i < maxY; ++i) {
            for (int j = 0; j < maxX; ++j) {
                bool objectFound = false;
                for (const GameObject* obj : objects) {
                    if (obj->getX() == j && obj->getY() == i) {
                        if (dynamic_cast<const Fruit*>(obj) != nullptr) {
                            std::cout << "F ";
                        } else if (dynamic_cast<const Obstacle*>(obj) != nullptr) {
                            std::cout << "O ";
                        }
                        objectFound = true;
                        break;
                    }
                }
                if (!objectFound) {
                    if (j == player.getX() && i == player.getY()) {
                        std::cout << "P ";
                    } else {
                        std::cout << ". ";
                    }
                }
            }
            std::cout << std::endl;
        }

        // Check for collision with fruits or obstacles
        for (auto it = objects.begin(); it != objects.end();) {
            if (player.checkCollision(**it)) {
                if (dynamic_cast<const Fruit*>(*it) != nullptr) {
                    player.increaseScore();
                    it = objects.erase(it);
                } else if (dynamic_cast<const Obstacle*>(*it) != nullptr) {
                    std::cout << "Game Over! You collided with an obstacle." << std::endl;
                    return 0;
                }
            } else {
                ++it;
            }
        }

        std::cout << "Score: " << player.getScore() << std::endl;

        std::cout << "Enter move (w/a/s/d): ";
        std::cin >> moveDirection;

        player.move(moveDirection);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto currentTime = std::chrono::steady_clock::now();
        elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
    }

    std::cout << "Time's up! Your final score: " << player.getScore() << std::endl;

    // Clean up allocated memory
    for (GameObject* obj : objects) {
        delete obj;
    }

    return 0;
}
