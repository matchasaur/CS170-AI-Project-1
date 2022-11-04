#include <iostream>
#include <cstdlib>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <list>


using namespace std;

chrono::milliseconds timespan(50);

struct State {
    vector<vector<int> > state;         //Vector/Array representation of our board
    pair<int, int> blankTile;               //Position of blank tile
    int cost;
    int heuristic;
    State()
    {
	cost = 0;
	heuristic = 0;
    };

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

};

class Puzzle {
    public:
    State* start;
    State* goal;


    Puzzle()                                //Initializes our puzzle with the start and goal states
    {
    	start = new State;
	goal = new State;

	start->state = {{0,7,2}, {4,6,1}, {3,5,8}};
	start->cost = 0;
	goal->state = {{1,2,3}, {4,5,6}, {7,8,0}};
 
        start->findBlank();

        goal->blankTile.first = 2;
        goal->blankTile.second = 2;
    }

};

class Hash
{
    public:
	int numBuckets;
	
	list<vector<vector<int> > > *table;

	Hash()
	{
	    numBuckets = 1000000;
	    table = new list<vector<vector<int> > >[numBuckets];
	}

	int hashFunction(vector<vector<int> > node)
	{
	    
	    int row1 = node[0][0]*node[0][1]*node[0][2];
	    int row2 = node[1][0]*node[1][1]*node[1][2];
	    int col1 = node[0][0]*node[1][0]*node[2][0];
	    int col2 = node[0][1]*node[1][1]*node[2][1];
	    

	    return row1+row2+col1+col2;
	    
	}

	void insertItem(vector<vector<int> > node)
	{
	    int index = hashFunction(node);
	    cout << "inserting at: " << index << endl;
	    table[index].push_back(node);
	}

	bool isRepeated(vector<vector<int> > node)
	{
	    int index = hashFunction(node);
 	    cout << "Searching at: " << index << endl;
	    list<vector<vector<int> > >::iterator i;
	    for(i = table[index].begin(); i!= table[index].end(); i++)
	    {
	        if(*i == node)
		{
		    break;
		}
	    }
	    if (i != table[index].end())
	    {
		return true;
	    }
	    else
	    {
		return false;
	    }
	}
};

class UniformCompare
{
    public:
	bool operator() (State* node1, State* node2)
	{
	    return node1->cost > node2->cost;
	}
};

void printPuzzle(State*);
vector<vector<int> > uniformCostSearch(Puzzle);
vector<vector<int> > MisplacedTileSearch(Puzzle);
void UniformExpand(State* &currNode, priority_queue<State*, vector<State*>, UniformCompare>& nodes, Hash& h, int& maxqueuesize);
void MisplacedExpand(State* &currNode, priority_queue<State*, vector<State*>, UniformCompare>& nodes, vector<vector<vector<int> > >& repeatedStates, int& maxqueuesize); 
void moveUp(vector<vector<int> >& state, pair<int, int> blankTile); 
void moveDown(vector<vector<int> >& state, pair<int, int> blankTile); 
void moveLeft(vector<vector<int> >& state, pair<int, int> blankTile); 
void moveRight(vector<vector<int> >& state, pair<int, int> blankTile);
int MisplacedEval(State*); 

int main() 
{
    Puzzle puzzle;

    uniformCostSearch(puzzle);

    return 0;
}


vector<vector<int> > uniformCostSearch(Puzzle puzzle)
{
    priority_queue<State*, vector<State*>, UniformCompare> nodes;                 //initialize queue and push starting state to top
    nodes.push(puzzle.start);
    int maxqueuesize = 0;
    
    State* currNode;
    
    Hash h;

    //vector<vector<vector<int> > > repeatedStates;
    //repeatedStates.push_back(puzzle.start->state);


    cout << "hello!\n";

    while(1)
    {
        //this_thread::sleep_for(timespan);
        if (nodes.empty())                                          // If the queue is empty, than a solution was not found, we return the starting state
        {
            cout << "A solution was not found. :(\n" << "at depth: " << currNode->cost << endl;
            return puzzle.start->state;
        }
        currNode = nodes.top();
        nodes.pop();
	h.insertItem(currNode->state);	

	cout << "Currently looking at: \n";
	printPuzzle(currNode);
        if (currNode->state == puzzle.goal->state)                          //We have found a solution. Output the matrix, depth found at, and nodes expanded
        {
            cout << "We have found a solution: " << endl;
            printPuzzle(currNode);
	    cout << "Solution Depth: " << currNode->cost << endl;
	    cout << "Max queue size: " << maxqueuesize << endl;
            return currNode->state;
            //TODO: Compute depth and expanded nodes
        }
        UniformExpand(currNode, nodes, h, maxqueuesize);                                            //Expand all children, push to the queue
    }

    return puzzle.start->state;
}

void UniformExpand(State* &currNode, priority_queue<State*, vector<State*>, UniformCompare>& nodes, Hash& h, int& maxqueuesize)
    {
	bool repeat;
	State* temp;
	vector<vector<int> > tempState;
	//std::vector<vector<vector<int> > >::iterator it;
        int row = currNode->blankTile.first;
        int col = currNode->blankTile.second;

        if(row-1 > -1)       //Validate moving up
        {
	    tempState = currNode->state;
	    moveUp(tempState,currNode->blankTile);
	    repeat = h.isRepeated(tempState);
	    if (!repeat)
	    {
		temp = new State(tempState);
		temp->cost = currNode->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }            
	}
        if(row+1 < 3)       //Validate moving down
        {
            tempState = currNode->state;
	    moveDown(tempState,currNode->blankTile);
	    repeat = h.isRepeated(tempState);
	    if (!repeat)
	    {
		temp = new State(tempState);
	    	temp->cost = currNode->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }
        }
        if(col-1 > -1)       //Validate moving left
        {
            
	    tempState = currNode->state;
	    moveLeft(tempState, currNode->blankTile);
	    repeat = h.isRepeated(tempState);
	    if (!repeat)
	    {
		temp = new State(tempState);
	    	temp->cost = currNode->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }
        }
        if(col+1 < 3)       //Validate moving right
        {
       	    tempState = currNode->state;
	    moveRight(tempState,currNode->blankTile);
	    repeat = h.isRepeated(tempState);
	    if (!repeat)
	    {
		temp = new State(tempState);
	    	temp->cost = currNode->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }
        }
    }

/*

vector<vector<int> > MisplacedTile(Puzzle puzzle)
{
    priority_queue<State*, vector<State*>, UniformCompare> nodes;                 //initialize queue and push starting state to top
    nodes.push(puzzle.start);
    int maxqueuesize = 0;
    
    State* currNode;
    Hash h;
    h.insertItem(puzzle.start->state);
    //vector<vector<vector<int> > > repeatedStates;
    //repeatedStates.push_back(puzzle.start->state);


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
	    cout << "Solution Depth: " << currNode->cost << endl;
	    cout << "Max queue size: " << maxqueuesize << endl;
            return currNode->state;
        }
        MisplacedExpand(currNode, nodes, repeatedStates, maxqueuesize);                                            //Expand all children, push to the queue
    }

    return puzzle.start->state;
}

void MisplacedExpand(State* &currNode, priority_queue<State*, vector<State*>, UniformCompare>& nodes, vector<vector<vector<int> > >& repeatedStates, int& maxqueuesize)
    {
	State* temp;
	vector<vector<int> > tempState;
	std::vector<vector<vector<int> > >::iterator it;
        int row = currNode->blankTile.first;
        int col = currNode->blankTile.second;

        if(row-1 > -1)       //Validate moving up
        {
	    tempState = currNode->state;
	    moveUp(tempState,currNode->blankTile);
	    it = find(repeatedStates.begin(), repeatedStates.end(), tempState);
	    if (it == repeatedStates.end())
	    {
		temp = new State(tempState);
		temp->cost = currNode->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }            
	}
        if(row+1 < 3)       //Validate moving down
        {
            tempState = currNode->state;
	    moveDown(tempState,currNode->blankTile);
	    it = find(repeatedStates.begin(), repeatedStates.end(), tempState);
	    if (it == repeatedStates.end())
	    {
		temp = new State(tempState);
	    	temp->cost = currNode->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }
        }
        if(col-1 > -1)       //Validate moving left
        {
            tempState = currNode->state;
	    moveLeft(tempState, currNode->blankTile);
	    it = find(repeatedStates.begin(), repeatedStates.end(), tempState);
	    if (it == repeatedStates.end())
	    {
		temp = new State(tempState);
	    	temp->cost = currNode->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }
        }
        if(col+1 < 3)       //Validate moving right
        {
            tempState = currNode->state;
	    moveRight(tempState,currNode->blankTile);
	    it = find(repeatedStates.begin(), repeatedStates.end(), tempState);
	    if (it == repeatedStates.end())
	    {
		temp = new State(tempState);
	    	temp->cost = currNode->cost + 1;
            	nodes.push(temp);
		maxqueuesize++;	
	    }
        }
    }

int MisplacedEval(State* currNode)
{
    //for(int i=0; i<


}
*/










//Misellaneous Functions

void moveUp(vector<vector<int> >& state, pair<int, int> blankTile)                                                             //Moves blank tile up
{
    swap(state[blankTile.first][blankTile.second], state[blankTile.first-1][blankTile.second]);         
}

void moveDown(vector<vector<int> >& state, pair<int, int> blankTile)                                                             //Moves blank tile down
{
    swap(state[blankTile.first][blankTile.second], state[blankTile.first+1][blankTile.second]);         
}

void moveLeft(vector<vector<int> >& state, pair<int, int> blankTile)                                                             //Moves blank tile left
{
    swap(state[blankTile.first][blankTile.second], state[blankTile.first][blankTile.second-1]);         
}

void moveRight(vector<vector<int> >& state, pair<int, int> blankTile)                                                             //Moves blank tile right
{
    swap(state[blankTile.first][blankTile.second], state[blankTile.first][blankTile.second+1]);         
}


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
