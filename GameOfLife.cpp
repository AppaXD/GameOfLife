#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>

double cd = 0.03;

int square_size = 4;
double thickness = 0;
int cell_size = square_size - thickness;
int x_c = 0, y_c = 0;

sf::Color alive(sf::Color::White);
sf::Color dead(sf::Color::Black);

class Cell
{
public:
    bool alive;
    sf::Vector2f position; // position is x and y value on grid, not actual pos
    Cell(bool, sf::Vector2f);
};

Cell::Cell(bool m_alive, sf::Vector2f m_position)
{
    alive = m_alive;
    position = m_position;
}

std::vector<sf::RectangleShape> grid;
std::vector<Cell> cells;

Cell cellAt(sf::Vector2f pos)
{
    return cells[(pos.y*x_c)+pos.x];
}

int roundUp(int num, int factor)
{
    return num+factor-1-(num-1)%factor;
}

std::vector<std::string> split(const std::string &text, const char* sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        if (end != start) {
          tokens.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    if (end != start) {
       tokens.push_back(text.substr(start));
    }
    return tokens;
}

int countNeighbours(Cell cell)
{
    sf::Vector2f pos = cell.position;

    int count = 0;

    if(pos.x != 0) count += (cellAt(sf::Vector2f(pos.x-1, pos.y)).alive ? 1 : 0);
    if(pos.y != 0) count += (cellAt(sf::Vector2f(pos.x, pos.y-1)).alive ? 1 : 0);
    if(pos.x != x_c) count += (cellAt(sf::Vector2f(pos.x+1, pos.y)).alive ? 1 : 0);
    if(pos.y != y_c) count += (cellAt(sf::Vector2f(pos.x, pos.y+1)).alive ? 1 : 0);

    if(pos.y != 0 && pos.x != 0) count += (cellAt(sf::Vector2f(pos.x-1, pos.y-1)).alive ? 1 : 0);
    if(pos.y != 0 && pos.x != x_c) count += (cellAt(sf::Vector2f(pos.x+1, pos.y-1)).alive ? 1 : 0);
    if(pos.y != x_c && pos.x != 0) count += (cellAt(sf::Vector2f(pos.x-1, pos.y+1)).alive ? 1 : 0);
    if(pos.y != x_c && pos.x != x_c) count += (cellAt(sf::Vector2f(pos.x+1, pos.y+1)).alive ? 1 : 0);

    return count;
}

int rand(int min, int max)
{
    return rand() % max + min;
}

int main()
{
    int width = 1100;
    int height = 700;

    width = ((width + square_size/2) / square_size) * square_size;
    height = ((height + square_size/2) / square_size) * square_size;

    bool hasStarted = true;

    x_c = width/square_size; y_c = height/square_size;

    for(int i = 0; i < height/square_size; ++i)
    {
        for(int j = 0; j < width/square_size; ++j)
        {
            bool ialive = rand(1, 10) < 3;
            sf::RectangleShape square;
            square.setSize(sf::Vector2f(square_size, cell_size));
            square.setPosition(j*square_size, i*square_size);
            square.setFillColor(ialive ? alive : dead);
            square.setOutlineColor(sf::Color(128, 128, 128));
            square.setOutlineThickness(thickness);
            grid.push_back(square);

            Cell cell(ialive, sf::Vector2f(j, i));
            cells.push_back(cell);
        }
    }

    std::vector<int> deaths;
    std::vector<int> alives;

    bool down = false; bool down2 = false;

    sf::RenderWindow window(sf::VideoMode(width, height), "Conway's Game of Life");
    sf::Clock clock;

    float elapsed = 0.0;

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();

            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left || (event.type == sf::Event::MouseMoved && down))
            {
                down = true;
                sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                int x = (roundUp(mouse.x, square_size)-square_size)/square_size;
                int y = (roundUp(mouse.y, square_size)-square_size)/square_size;

                int i = (y*x_c)+x;
                grid[i].setFillColor(alive);
                cells[i].alive = true;
            }

            if((event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) || (event.type == sf::Event::MouseMoved && down2))
            {
                down2 = true;
                sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                int x = (roundUp(mouse.x, square_size)-square_size)/square_size;
                int y = (roundUp(mouse.y, square_size)-square_size)/square_size;

                int i = (y*x_c)+x;
                grid[i].setFillColor(dead);
                cells[i].alive = false;
            }

            if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
                down = false;
            if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
                down2 = false;

            if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::C)
                {
                    for(int i = 0; i < grid.size(); ++i)
                    {
                        grid[i].setFillColor(dead);
                        cells[i].alive = false;
                    }
                }
                else if(event.key.code == sf::Keyboard::Space)
                {
                    if(hasStarted) hasStarted = false;
                    else hasStarted = true;
                }
                else if(event.key.code == sf::Keyboard::E)
                {
                    std::ofstream file;
                    file.open("file.txt");
                    file << "|";
                    for(int i = 0; i < grid.size(); ++i)
                    {
                        if(cells[i].alive)
                        {
                            file << cells[i].position.x;
                            file << ",";
                            file << cells[i].position.y;
                            file << "|";
                        }
                    }
                    file.close();
                    std::cout << "Exported to file.\n";
                }
                else if(event.key.code == sf::Keyboard::I)
                {
                    std::ifstream file("file.txt");
                    std::string str;
                    std::string content;

                    while(std::getline(file, str))
                    {
                        content += str;
                    }

                    for(int i = 0; i < grid.size(); ++i) { grid[i].setFillColor(dead); cells[i].alive = false; }

                    std::vector<std::string> positions = split(content, "|");
                    for(int i = 0; i < positions.size(); ++i)
                    {
                        std::vector<std::string> pos = split(positions[i], ",");
                        int x = atoi(pos[0].c_str());
                        int y = atoi(pos[1].c_str());
                        int index = (y*x_c)+x;
                        cells[index].alive = true;
                        grid[index].setFillColor(alive);
                    }
                }
            }
        }

        for(int i = 0; i < alives.size(); ++i)
        {
            int ind = alives[i];
            cells[ind].alive = true;
            grid[ind].setFillColor(alive);
        }

        for(int i = 0; i < deaths.size(); ++i)
        {
            int ind = deaths[i];
            cells[ind].alive = false;
            grid[ind].setFillColor(dead);
        }

        alives.clear(); deaths.clear();

        float dt = clock.restart().asSeconds();
        if(hasStarted) elapsed += dt;

        window.clear();

        for(int i = 0; i < grid.size(); ++i)
        {
            if(hasStarted && elapsed > cd)
            {
                int neighbours = countNeighbours(cells[i]);
                if(cells[i].alive)
                {
                    if(neighbours < 2 || neighbours > 3)
                    {
                        deaths.push_back(i);
                    }
                }
                else
                {
                    if(neighbours == 3)
                    {
                        alives.push_back(i);
                    }
                }
            }
            window.draw(grid[i]);
        }

        if(hasStarted && elapsed > cd)
            elapsed = 0.0;

        window.display();
    }

    return 0;
}

