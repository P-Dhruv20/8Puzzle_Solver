#include <iostream>
#include <queue>
#include <utility>
#include <vector>
#include <cmath>
#include <unordered_map>

using namespace std;

// tracking number of nodes expanded and max frontier size for each search
int expansion, maxfrontier = 0;

// Node class to represent each state of the puzzle
class Node
{
    // private variables of the node class
private:
    int cost;
    int h_n;
    int g_n;
    string move;
    vector<vector<int>> state;
    vector<vector<int>> goalState = {{1, 2, 3},
                                     {4, 5, 6},
                                     {7, 8, 0}};
    Node *parent;

    // public variables of the node class
public:
    void setH(int h) { this->h_n = h; } // set h(n) value of node object (heuristic) for A* search

    int getH() { return this->h_n; } // get h(n) value of node object (heuristic) for A* search

    int getG() { return this->g_n; } // get g(n) value of node object (cost)

    int getCost() { return this->cost; } // get f(n) value of node object (cost + heuristic)

    Node *getParent() { return this->parent; } // get parent node of current node

    string getMove() { return this->move; } // get move that was made to get to current node

    vector<vector<int>> getGoalState() { return this->goalState; } // get goal state of puzzle

    vector<vector<int>> getState() { return this->state; } // get current state of puzzle

    Node();

    // constructor for initial node object
    Node(int g, int h, int cost, string move, vector<vector<int>> state, Node *parent)
    {
        this->g_n = g;
        this->h_n = h;
        this->cost = cost;
        this->move = move;
        this->state = state;
        this->parent = parent;
    }
};

// compareCost struct to compare cost of two nodes for priority queue
struct compareCost
{
    bool operator()(Node *a, Node *b)
    {
        return a->getCost() > b->getCost();
    }
};

// returns coordinates of a value in a node's state representation as 2D vector and return the x,y coordinates as a pair
pair<int, int> find_coordinates(Node *node, int val)
{
    pair<int, int> coordinate;
    for (int i = 0; i < node->getState().size(); i++)
    {
        for (int j = 0; j < node->getState()[1].size(); j++)
        {
            if (val == node->getGoalState()[i][j])
            {
                coordinate.first = i;
                coordinate.second = j;
                return coordinate;
            }
        }
    }
    // if value is not found, return -1,-1
    return make_pair(-1, -1);
}

// returns the manhattan distance of a node object based on its state representation and the goal state
int calculate_ManhattanDistance(Node *node)
{
    int distance = 0;
    // loop through the state representation of the node object (2D) and calculate the manhattan distance of each value
    for (int i = 0; i < node->getState().size(); i++)
    {
        for (int j = 0; j < node->getState()[1].size(); j++)
        {
            pair<int, int> curr(i, j);
            //
            if (find_coordinates(node, node->getState()[i][j]) == curr)
                distance += 0;
            else
            {
                // add the manhattan distance of the current value to the total distance of the node
                distance += abs(find_coordinates(node, node->getState()[i][j]).first - i) +
                            abs(find_coordinates(node, node->getState()[i][j]).second - j);
            }
        }
    }
    return distance;
}

// returns the number of misplaced tiles of a node object based on its state representation and the goal state
int calculateMisplacedTile(Node *node)
{
    int num = 0;
    for (int i = 0; i < node->getState().size(); i++)
    {
        for (int j = 0; j < node->getState()[i].size(); j++)
        {
            // if the value is not 0 and is not in the correct position, increment the number of misplaced tiles
            // (0 is not counted as a misplaced tile)
            if (node->getState()[i][j] != 0 && node->getState()[i][j] != node->getGoalState()[i][j])
                num++;
        }
    }
    return num;
}

// returns a vector of all possible moves that can be made from a node object (up, down, left, right) based on the current state of the puzzle
// this also creates a new node object for each move and adds it to the vector of possible moves and returns the vector
// this vector is used to add the possible moves to the priority queue for future expansion
vector<Node *> operators(Node *node, int method)
{
    int x = -1;
    int y = -1;
    vector<Node *> vals;
    for (int i = 0; i < node->getState().size(); i++)
    {
        for (int j = 0; j < node->getState()[i].size(); j++)
        {
            if (node->getState()[i][j] == 0)
            {
                x = i;
                y = j;
                break;
            }
        }
    }
    // these are the possible moves to be done on the blank space (0) in the puzzle
    // if the blank space is in the middle or bottom row, add the possible moves to the vector of possible moves
    if (x == 1 || x == 2)
    {
        int h = 0;
        // create a copy of the current state of the node object and swap the blank space with the value above it
        vector<vector<int>> copy = node->getState();
        copy[x][y] = copy[x - 1][y];
        copy[x - 1][y] = 0;
        string move = "up";
        // calculate the heuristic value of the node based on the method chosen by the user
        if (method == 0)
            h = 0;
        else if (method == 1)
            h = calculateMisplacedTile(node);
        else
            h = calculate_ManhattanDistance(node);
        // create a new node object with the new state representation and add it to the vector of possible moves to be returned
        Node *n = new Node(node->getG() + 1, h, node->getG() + 1 + h, move, copy, node);
        vals.push_back(n);
    }
    // if the blank space is in the middle or top row, add the possible moves to the vector of possible moves
    if (x == 1 || x == 0)
    {
        int h = 0;
        // create a copy of the current state of the node object and swap the blank space with the value below it
        vector<vector<int>> copy = node->getState();
        copy[x][y] = copy[x + 1][y];
        copy[x + 1][y] = 0;
        string move = "down";
        // calculate the heuristic value of the node based on the method chosen by the user
        if (method == 0)
            h = 0;
        else if (method == 1)
            h = calculateMisplacedTile(node);
        else
            h = calculate_ManhattanDistance(node);
        // create a new node object for the move and add it to the vector of possible moves to be returned
        Node *n = new Node(node->getG() + 1, h, node->getG() + 1 + h, move, copy, node);
        vals.push_back(n);
    }
    // if the blank space is in the middle or left column, add the possible moves to the vector of possible moves
    if (y == 1 || y == 2)
    {
        int h = 0;
        // create a copy of the current state of the node object and swap the blank space with the value to the left of it
        vector<vector<int>> copy = node->getState();
        copy[x][y] = copy[x][y - 1];
        copy[x][y - 1] = 0;
        string move = "left";
        // calculate the heuristic value of the node based on the method chosen by the user
        if (method == 0)
            h = 0;
        else if (method == 1)
            h = calculateMisplacedTile(node);
        else
            h = calculate_ManhattanDistance(node);
        // create a new node object with the new state representation and add it to the vector of possible moves to be returned
        Node *n = new Node(node->getG() + 1, h, node->getG() + 1 + h, move, copy, node);
        vals.push_back(n);
    }
    // if the blank space is in the middle or right column, add the possible moves to the vector of possible moves
    if (y == 0 || y == 1)
    {
        int h = 0;
        // create a copy of the current state of the node and swap the blank space with the value to the right of it
        vector<vector<int>> copy = node->getState();
        copy[x][y] = copy[x][y + 1];
        copy[x][y + 1] = 0;
        string move = "right";
        // calculate the heuristic value of the node based on the method chosen by the user
        if (method == 0)
            h = 0;
        else if (method == 1)
            h = calculateMisplacedTile(node);
        else
            h = calculate_ManhattanDistance(node);
        // create a new node object for each possible move and add it to the vector of possible moves to be returned
        Node *n = new Node(node->getG() + 1, h, node->getG() + 1 + h, move, copy, node);
        vals.push_back(n);
    }
    if (vals.size() > 0)
        expansion++;
    return vals;
}

// check if the goal state has been reached by comparing the current state to the goal state
bool isGoalReached(vector<vector<int>> init)
{
    Node *node = new Node(0, 0, 0, "", init, nullptr);
    for (int i = 0; i < node->getState().size(); i++)
    {
        for (int j = 0; j < node->getState()[i].size(); j++)
        {
            if (init[i][j] != node->getGoalState()[i][j])
                return false;
        }
    }
    return true;
}

// return the key for the map based on the current state of the node
string getKey(vector<vector<int>> state)
{
    string key = "";
    for (auto row : state)
    {
        for (int i = 0; i < row.size(); i++)
        {
            key += to_string(row.at(i));
        }
    }
    return key;
}

// function to print the path from the initial state to the goal state and results of the search
void displayState(Node *node)
{
    // if the node is the initial state, print the initial state
    if (node->getParent() == nullptr)
    {
        cout << "Expanding state" << endl;
        for (auto i : node->getState())
        {
            for (int j = 0; j < i.size(); ++j)
            {
                cout << i[j] << " ";
            }
            cout << endl;
        }
        cout << endl;
        return;
    }
        // if the node is not the initial state, recursively call the function to print the path from the initial state to the goal state
    else
    {
        displayState(node->getParent());
        cout << "The best state to expand with g(n) = " << node->getG() << " and h(n) = " << node->getH()
             << " and move the zero : " << node->getMove() << endl;
        for (auto i : node->getState())
        {
            for (int j = 0; j < i.size(); ++j)
            {
                cout << i[j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

// function to perform the general search algorithm which calls the appropriate search algorithm based on the method chosen by the user
void generalSearch(vector<vector<int>> init, int method)
{
    // define the priority queue to store the nodes and the maps to store the seen nodes and the nodes in the queue
    priority_queue<Node *, vector<Node *>, compareCost> Nodes;
    Node *start = new Node(0, 0, 0, "", init, nullptr);
    unordered_map<string, int> seenMap, Nodesmap;
    Nodes.push(start);

    // loop until the queue is empty or the goal state is reached
    while (true)
    {
        // check if the queue is empty, if so, print the failure message and return
        if (Nodes.empty())
        {
            cout << "NO NODES TO TRAVERSE, FAILED!!!" << endl;
            cout << "Before failure," << endl;
            cout << "Number of nodes expanded: " << expansion << endl;
            cout << "Max queue size: " << maxfrontier << endl;
            return;
        }
        // pop the top node from the queue and check if it is the goal state, if so, print the path from the initial state to the goal state and the results of the search
        Node *node = Nodes.top();
        Nodes.pop();
        string key = getKey(node->getState());
        seenMap[key] = 1;
        if (isGoalReached(node->getState()))
        {
            node->setH(0);
            displayState(node);
            cout << "Goal state!!!" << endl
                 << endl;
            cout << "Solution depth was " << node->getG() << endl;
            cout << "Number of nodes expanded: " << expansion << endl;
            cout << "Max queue size: " << maxfrontier << endl;

            return;
        }
        // if the node is not the goal state, expand the node and add the expanded nodes to the queue if they have not been seen before
        vector<Node *> expandingNodes = operators(node, method);
        for (auto i : expandingNodes)
        {
            // check if the node has been seen before, if so, do not add it to the queue
            if (seenMap[getKey(i->getState())] == 1 && Nodesmap[getKey(i->getState())] == 1)
                continue;
            Nodes.push(i);
            Nodesmap[getKey(i->getState())] = 1;
        }
        // update the max frontier size if the current size of the queue is greater than the current max frontier size
        maxfrontier = max(maxfrontier, static_cast<int>(Nodes.size()));
    }
}

// driver function to run the program
int main()
{
    // define the variables to store the initial state, the choice of the user, and the method of search chosen by the user
    vector<vector<int>> init;
    int choice, method;
    vector<int> row1, row2, row3;
    // prompt the user to choose the puzzle to solve
    cout << "Welcome to my 8 puzzle solver." << endl;
    cout << "Type \"1\" to use a default puzzle, Or \"2\" to enter your own puzzle." << endl;
    cin >> choice;
    // if the user chooses to enter their own puzzle, prompt the user to enter the puzzle and store the puzzle in the vector
    if (choice == 2)
    {
        int val1, val2, val3;
        cout << "Enter your puzzle, using a zero to represent the blank. Please only enter valid 8-puzzles. Enter the puzzle demilimiting the numbers with a space. Type RETURN only when finished." << endl;
        cout << "Enter the first row: ";
        cin >> val1 >> val2 >> val3;
        row1.push_back(val1);
        row1.push_back(val2);
        row1.push_back(val3);
        cout << "Enter the second row: ";
        cin >> val1 >> val2 >> val3;
        row2.push_back(val1);
        row2.push_back(val2);
        row2.push_back(val3);
        cout << "Enter the third row: ";
        cin >> val1 >> val2 >> val3;
        row3.push_back(val1);
        row3.push_back(val2);
        row3.push_back(val3);
    }
    else
    {
        // Default puzzle
        row1 = {7, 1, 2};
        row2 = {4, 8, 5};
        row3 = {6, 3, 0};
    }
    init.push_back(row1);
    init.push_back(row2);
    init.push_back(row3);
    // prompt the user to choose the method of search
    cout << "Enter your choice of algorithm:" << endl;
    cout << "1. Uniform Cost Search" << endl;
    cout << "2. A* with Misplaced Tile heuristic" << endl;
    cout << "3. A* with Manhattan distance Heuristic" << endl;
    cin >> method;
    cout << endl;

    // call the general search algorithm to solve the puzzle
    generalSearch(init, method);

    return 0;
}
