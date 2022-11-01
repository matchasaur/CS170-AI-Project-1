#include <iostream>
#include <cstdlib>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <algorithm>

using namespace std;

chrono::milliseconds timespan(50);

// vector<vector<int>> start_state = { {1, 2, 3},         //Sample start state for testing purposes, should return a depth of 2
//                                          {4, 5, 6},
//                                          {0, 7, 8} };

struct State {
    vector<vector<int> > state;         //Vector/Array representation of our board
    pair<int, int> blankTile;               //Position of blank tile
    int cost;
    State(){};

    State(vector<vector<int> > newState)
    {
        state = newState;
        findBlank();
    }
    void findBlank()                        //Finds blank tile in state
    {
        for(unsigned int i = 0; i < state.size(); i++)
        {
            for(unsigned int j = 0; j < state.size(); j++)
            {
                if (state[i][j] == 0)
                {
                    blankTile.first = i;
                    blankTile.second = j;
                    return;
                }
            }
        }
    }

    void Expand(priority_queue<State*>& nodes, vector<vector<vector<int> > >& repeatedStates, int& maxqueuesize)
    {
        State* temp;
	vector<vector<int> > tempState;
	std::vector<vector<vector<int> > >::iterator it;
        int row = this->blankTile.first;
        int col = this->blankTile.second;

        if(row-1 > 0)       //Validate moving up
        {
	    cout << "wassuppp\n";
	    tempState = this->state;
	    moveUp(tempState);
	    it = find(repeatedStates.begin(), repeatedStates.end(), tempState);
	    if (it == repeatedStates.end())
	    {
		cout << "Wassssssup!!! 2.0!!!!\n";
		temp = new State(tempState);
		temp->cost = this->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }            
	}
        if(row+1 < 3)       //Validate moving down
        {
            tempState = this->state;
	    moveDown(tempState);
	    it = find(repeatedStates.begin(), repeatedStates.end(), tempState);
	    if (it == repeatedStates.end())
	    {
		temp = new State(tempState);
	    	temp->cost = this->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }
        }
        if(col-1 > 0)       //Validate moving left
        {
            tempState = this->state;
	    moveLeft(tempState);
	    it = find(repeatedStates.begin(), repeatedStates.end(), tempState);
	    if (it == repeatedStates.end())
	    {
		temp = new State(tempState);
	    	temp->cost = this->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }
        }
        if(col+1 < 3)       //Validate moving right
        {
            tempState = this->state;
	    moveRight(tempState);
	    it = find(repeatedStates.begin(), repeatedStates.end(), tempState);
	    if (it == repeatedStates.end())
	    {
		temp = new State(tempState);
	    	temp->cost = this->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }
        }
    }

    void moveUp(vector<vector<int> >& state)                                                             //Moves blank tile up
    {
        swap(state[blankTile.first][blankTile.second], state[blankTile.first-1][blankTile.second]);         
    }

    void moveDown(vector<vector<int> >& state)                                                             //Moves blank tile down
    {
        swap(state[blankTile.first][blankTile.second], state[blankTile.first+1][blankTile.second]);         
    }

    void moveLeft(vector<vector<int> >& state)                                                             //Moves blank tile left
    {
        swap(state[blankTile.first][blankTile.second], state[blankTile.first][blankTile.second-1]);         
    }

    void moveRight(vector<vector<int> >& state)                                                             //Moves blank tile right
    {
        swap(state[blankTile.first][blankTile.second], state[blankTile.first][blankTile.second+1]);         
    }
};

class Puzzle {
    public:
    State* start;
    State* goal;


    Puzzle()                                //Initializes our puzzle with the start and goal states
    {
    	start = new State;
	goal = new State;

	start->state = {{1,2,3}, {4,5,6}, {0,7,8}};
	start->cost = 0;
	goal->state = {{1,2,3}, {4,5,6}, {7,8,0}};
 
        start->findBlank();

        goal->blankTile.first = 2;
        goal->blankTile.second = 2;
    }

};

void printPuzzle(State*);
vector<vector<int> > uniformCostSearch(Puzzle);


int main() 
{
    Puzzle puzzle;

    uniformCostSearch(puzzle);




    return 0;
}


vector<vector<int> > uniformCostSearch(Puzzle puzzle)
{
    priority_queue<State*> nodes;                 //initialize queue and push starting state to top
    nodes.push(puzzle.start);
    int maxqueuesize = 0;
    
    State* currNode;
    vector<vector<vector<int> > > repeatedStates;
    repeatedStates.push_back(puzzle.start->state);


    cout << "hello!\n";

    while(1)
    {
        this_thread::sleep_for(timespan);
        if (nodes.empty())                                          // If the queue is empty, than a solution was not found, we return the starting state
        {
            cout << "A solution was not found. :(\n";
            return puzzle.start->state;
        }
        currNode = nodes.top();
        nodes.pop();
	repeatedStates.push_back(currNode->state);	

	cout << "Currently looking at: \n";
	printPuzzle(currNode);
        if (currNode->state == puzzle.goal->state)                          //We have found a solution. Output the matrix, depth found at, and nodes expanded
        {
            cout << "We have found a solution: " << endl;
            printPuzzle(currNode);
            return currNode->state;
            //TODO: Compute depth and expanded nodes
        }
        currNode->Expand(nodes, repeatedStates, maxqueuesize);                                            //Expand all children, push to the queue
    }

    return puzzle.start->state;
}









//Misellaneous Functions

void printPuzzle(State* board)
{
    for(unsigned int i = 0; i < 3; i++)
    {
        cout << "[ ";
        for(unsigned int j = 0; j < 3; j++)
        {
            cout << board->state[i][j] << " ";
        }
        cout << "]" << endl;
    }
}
