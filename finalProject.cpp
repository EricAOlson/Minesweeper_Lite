/*********************
 * Author: Eric Olson
 * Assignment: Final Class Project 
 * Date: 8/22/14
 * Last Updated: 8/28/14
 * Design:  This program will be a poor man's ASCII version of Microsoft's Minesweeper.  It will
 * be altered in some cases specifically to meet the Final Project requirements.
 ********************/

#include <iostream>
#include <ios>
#include <string>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <fstream>  //Basic file operations found in Absolute C++ text, chapter 12.

using std::cout;
using std::cin;	
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostream;
using std::left;
using std::right;



/*********************************
 *** Class/Struct Declarations ***
 *********************************/


/*****
 * 'gameTime' class houses data and functions related to length of game.
 *****/
//-----------------------------------------------------------------------
//Requirement #22: Demonstrate at least one class and at least one object
//-----------------------------------------------------------------------
class gameTime
{
  public:
    //Default Constructor:
    gameTime();  //Initalizes variables & starts game timer.

    //Accessors:
    short get_min() const;
    short get_sec() const;
    
    //Mutators:
    void set_min(short count);
    void set_sec(short count);

    //Overloaded Operators:   Use of overloaded operators found in Absolute C++ text, chapter 8.
    bool operator < (const gameTime &arg2) const;
    friend ostream& operator << (ostream& out, const gameTime& obj);

    //Member Functions:
    void update();

  private:
    //Member Variables:
    time_t start;  //Use of time_t found online @ www.cplusplus.com/reference/ctime/time
    time_t finish;
    short mins;
    short secs;
};

/*****
 * 'score' struct houses data used for high score analysis.
 *****/
//------------------------------------------------
//Requirement #21: Demonstrate at least one struct
//------------------------------------------------
struct score
{
  string name;
  short mines;
  gameTime duration;
};



/*********************************
 *** Function Prototypes       ***
 *********************************/


int menu();  //Presents the overall game menu.
void score_menu();  //Presents menu of high score tables.
void rules();  //Explains the rules of the game.
void generate_boards(const int diff, char **&board, char **&display, gameTime *&clock); 
     //Dynamically allocates new game boards.
void init_board(char **b, const int diff, const char fill = ' ');  //Initalizes a new board with a fill char.
void fill_board(char **b, const int diff);  //Fills the game board with mines, #'s where appropriate.
short user_turn(const int diff, char const* const* const &b, char **d, gameTime *clock);  
     //Processes as single turn.
void display(char const* const* const &b, const int diff, const gameTime *clock);  
     //Displays the current game board.
bool get_select(const short size, char const* const* const &d, short &s1, short &s2);  
     //Gets a valid square selection from user.
void empty_square(short size, char const* const* const &b, char **d, short r, short c);  
     //Uncovers adjacent squares to an empty.
short wrap_up(const short result, const int select, char const* const* const &b, 
              char **d, const gameTime *clock);
     //Provides end game summary.
void high_score(const short found, const int diff, const int argc = 0, char const* const* const &argv = NULL, const gameTime *clock = NULL);  
     //Processes a high score, and displays resulting high score list.
void file_in(score e[], score m[], score h[]);  //Collects high score data from file.
void file_out(const score e[], const score m[], const score h[]);  //Writes updated high score data to file.
void get_name(score &temp, const int argc, char const* const* const &argv);  
     //Secures name from either command line entry or input.
bool check_win(const short size, char const* const* const &d);  //Checks if game has been won.
void delete_boards(const int diff, char **&board, char **&display, gameTime *&clock);  
     //Releases dynamic gameboard array memory.
void clear_screen(); //Clears the screen.
int get_input(string ask, int min = INT_MIN, int max = INT_MAX);  //User entry for INT with error handling.
string get_input();  //User entry for name(string) with error handling.

//Easy, Medium and Hard settings defined as global constants to allow for easy program-wide adjustments.
const short ESIZE = 4, EMINES = 3;
const short MSIZE = 5, MMINES = 5;
const short HSIZE = 6, HMINES = 8;



/*********************************
 *** MAIN FUNCTION             ***
 *********************************/


int main(int argc, char *argv[])
{  
  int select = 0;
  char **board = NULL, **disp_board = NULL;
  srand(time(NULL));
  short finished = 0;
//-----------------------------------------------------------------------
//Requirement #25: Demonstrate at least one class and at least one object
//-----------------------------------------------------------------------
  gameTime *clock = NULL; 
  
  while(select != 6)
  {
    finished = 0;
    
    select = menu();
    
    if (select == 4){score_menu();}
    else if (select == 5){rules();}
    else if (select <= 3)
    {
      generate_boards(select, board, disp_board, clock);
      while (finished == 0){finished = user_turn(select, board, disp_board, clock);}
      high_score(wrap_up(finished, select, board, disp_board, clock), select, argc, argv, clock);
      delete_boards(select, board, disp_board, clock);
    }
  }
    
  clear_screen();
  
  return 0;
}



/*********************************
 *** Class Member Functions    ***
 *********************************/


/**********
 * Constructor for Class: gameTime
 **********/
gameTime::gameTime()
{
  time(&start);
  mins = 0;
  secs = 0;
}

/**********
 * Accessors for Class: gameTime
 **********/
short gameTime::get_min() const {return mins;}
short gameTime::get_sec() const {return secs;}

/**********
 * Mutators for Class: gameTime
 **********/
void gameTime::set_min(short count) {mins = count;}
void gameTime::set_sec(short count) 
{
  mins += (count / 60);
  secs = (count % 60);
}

/**********
 * Overloaded Operators for Class: gameTime
 **********/
bool gameTime::operator < (const gameTime &arg2) const
{
  if (mins < arg2.mins) {return true;}
  else if (mins == arg2.mins)
  {
    if (secs < arg2.secs) {return true;}
  }

  return false;
}

ostream& operator << (ostream &output, const gameTime &obj) //technically a friend of gameTime.
{
  if (obj.mins < 10) {output << 0;}
  output << obj.mins << ":";
  if (obj.secs < 10) {output << 0;}
  output << obj.secs;
  
  return output;
}

/**********
 * PRE: Requires that calling object was created using default constructor.
 * POST: Updates the calling object's game mins and secs
 **********/
void gameTime::update()
{
  int duration = 0;
  
  time(&finish);
  duration = difftime(finish, start);

  mins = duration / 60;
  secs = duration % 60;
}  



/*********************************
 *** Function Definitions      ***
 *********************************/


/**********
 * PRE: No pre-conditions.
 * POST: Prints instructions to screen.
 **********/
//--------------------------------------------------
//Requirement #11: Demonstrate at least one function
//--------------------------------------------------
void rules()
{
  clear_screen();
  cout << "Minesweeper is a game about finding and flagging all the mines in the field.\n"
       << "To help you toward this goal, each square in the gameboard contains the # of\n"
       << "adjacent squares that contain a mine.  An uncovered example:\n\n"
       << "-------------------------\n"
       << "|     |     |     |     |\n"
       << "|  1  |  1  |  1  |     |\n"
       << "|     |     |     |     |\n"
       << "-------------------------\n"
       << "|     |     |     |     |\n"
       << "|  2  |  X  |  2  |     |\n"
       << "|     |     |     |     |\n"
       << "-------------------------\n"
       << "|     |     |     |     |\n"
       << "|  3  |  X  |  4  |  1  |\n"
       << "|     |     |     |     |\n"
       << "-------------------------\n"
       << "|     |     |     |     |\n"
       << "|  2  |  X  |  X  |  1  |\n"
       << "|     |     |     |     |\n"
       << "-------------------------\n\n"
       << "You will select a square to either reveal, or flag as a unexploded mine.\n"
       << "You will enter the row and column to indicate which square you want to uncover.\n"
       << "If you wish to flag (or unflag) a square you will add a '-F' after the square selection.\n"
       << "The game will end if you correctly flag all mines, and uncover the remaining board.\n"
       << "If you uncover a mine you will lose.\n"
       << "The time to completion will be used to keep a list of High Scores, so good luck!\n\n"
       << "Press enter to continue. . .";
  
  cin.ignore(INT_MAX, '\n');
}


/**********
 * PRE: Requires a difficulty value between 1-3 and 'NULL' pointers to gameboard, display board 
 * and timer object.
 * POST: Creates dynamic game boards, initalizes them with new random layout, and starts the game timer.
 **********/
void generate_boards(const int diff, char **&board, char **&display, gameTime *&clock)
{
  if (diff == 1)
  {
//-----------------------------------------------------------------
//Requirement #18: Demonstrate at least one multi-dimensional array
//-----------------------------------------------------------------
    board = new char*[ESIZE];
    display = new char*[ESIZE];
    for (short i = 0; i < ESIZE; i++)
    {
      board[i] = new char[ESIZE];
      display[i] = new char [ESIZE];
    }
    init_board(board, diff);
    init_board(display, diff, '?');
    fill_board(board, diff);
  }
  else if (diff == 2)
  {
    board = new char*[MSIZE];
    display = new char*[MSIZE];
    for (short i = 0; i < MSIZE; i++)
    {
//--------------------------------------------------------------------
//Requirement #19: Demonstrate at least one dynamically declared array
//--------------------------------------------------------------------
      board[i] = new char[MSIZE];
      display[i] = new char [MSIZE];
    }
    init_board(board, diff);
    init_board(display, diff, '?');
    fill_board(board, diff);
  }
  else  //diff must equal 3.
  {
    board = new char*[HSIZE];
    display = new char*[HSIZE];
    for (short i = 0; i < HSIZE; i++)
    {
      board[i] = new char[HSIZE];
      display[i] = new char [HSIZE];
    }
    init_board(board, diff, ' ');
    init_board(display, diff, '?');
    fill_board(board, diff);
  }
  
//--------------------------------------------------------------
//Requirement #25: Demonstrate at least one pointer to an object
//--------------------------------------------------------------
  clock = new gameTime;  //Note: Default constructor starts the game timer.
}


/**********
 * PRE: Requires difficulty value (1-3) and no longer required gameboard, displayboard and timer pointers.
 * POST: Deletes all dynamically created variables/arrays, and leaves pointers set to 'NULL'
 **********/
void delete_boards(const int diff, char **&board, char **&display, gameTime *&clock)
{
  if (diff == 1)
  {
    for (short i = 0; i < ESIZE; i++)
    {
      delete [] board[i];
      delete [] display[i];
    }
    delete [] board;
    board = NULL;
    delete [] display;
    display = NULL;
  }
  else if (diff == 2)
  {
    for (short i = 0; i < MSIZE; i++)
    {
      delete [] board[i];
      delete [] display[i];
    }
    delete [] board;
    board = NULL;
    delete [] display;
    display = NULL;
  }
  else //diff must equal 3.
  {
    for (short i = 0; i < HSIZE; i++)
    {
      delete [] board[i];
      delete [] display[i];
    }
    delete [] board;  
    board = NULL;  
    delete [] display;
    display = NULL;
  }

  delete clock;
  clock = NULL;
}


/**********
 * PRE: Requires pointer to desired board, current difficulty setting and 
 * an optional fill char.  (Default ' ')
 * POST: Fills game board with passed char, or with ' '
 **********/
void init_board(char **b, const int diff, const char fill)
{
  short size = 0;

  if (diff == 1){size = ESIZE;}
  else if (diff == 2){size = MSIZE;}
  else{size = HSIZE;}

//----------------------------------------------------------------------------
//Requirement #09: Demonstrate understanding of three general error categories
//----------------------------------------------------------------------------
  for (short i = 0; i < size; i++)  
  { //i < size, i++  (use of , instead of ;) would be caught by compiler, syntax error.
    for (short j = 0; j < size; j++)
    { //j <= size, would attempt to access and write to a non declared array element, run-time error.
      b[i][j] = fill; //b[i][j] == fill;  Use of == would compile but not behave as intended, logic error.
    }
  }
}


/**********
 * PRE: Requires ' ' initialized gameboard pointer, and current difficulty setting.
 * POST: Randomly places correct number of mines per difficulty setting.  Then populates the rest
 * of the gameboard with the appropriate # for each square boarding 1 or more mines.
 **********/
void fill_board(char **b, const int diff)
{
  short size = 0, mines = 0, m = 0, r1 = 0, r2 = 0, border = 0;

  if (diff == 1){size = ESIZE; mines = EMINES;}
  else if (diff == 2){size = MSIZE; mines = MMINES;}
  else{size = HSIZE; mines = HMINES;}

  while (m < mines)  //Randomly places correct # of mines.
  {
//-------------------------------------------------------
//Requirement #08: Demonstrate at least one random number
//-------------------------------------------------------
    r1 = (rand() % size);
    r2 = (rand() % size);
    
    if (b[r1][r2] == ' ')  //checks if duplicate placement, and places mine if not duplicate.
    {
      b[r1][r2] = 'X';
      m++;
    }
  }

  for (short i = 0; i < size; i++)
  {
    for (short j = 0; j < size; j++)  //iterates through the game board checking adjacent squares for mines.
    {
      border = 0;

//----------------------------------------------
//Requirement #05: Demonstrate logical operators
//----------------------------------------------
      if (b[i][j] == ' ')  //If square is not a mine. . .
      {
        if (!(i == 0 || j == 0))  //Check if square to the northwest exists, and if so. . .
        {
  	  if (b[i-1][j-1] == 'X'){border++;}  //If square to NW is mine, iterate the border count by 1.
        }
        if (i != 0)  //Checks square to the N.
        {
	  if (b[i-1][j] == 'X'){border++;}
        }
        if (!(i == 0 || j == (size - 1)))  //Checks square to the NE.
        {
	  if (b[i-1][j+1] == 'X'){border++;}
        }
        if (j != (size - 1))  //Checks square to the E.
        {
	  if (b[i][j+1] == 'X'){border++;}
        }  
        if (!(i == (size - 1) || j == (size - 1)))  //Checks square to the SE.
        {
 	  if (b[i+1][j+1] == 'X'){border++;}
        }
        if (i != (size - 1))  //Checks square to the S.
        {
 	  if (b[i+1][j] == 'X'){border++;}
        }
        if (!(i == (size - 1) || j == 0))  //Checks square to the SW
        {
	  if (b[i+1][j-1] == 'X'){border++;}
        }
        if (j != 0)  //Checks square to the W.
        {
	  if (b[i][j-1] == 'X'){border++;}
        }
      
        if (border > 0){b[i][j] = ('0' + border);}  //Adds count of adjacent mines to the square's value.
      }
    }
  }
}


/**********
 * PRE: Requires current difficulty setting (1-3), and pointers to the game boards and timer.
 * POST:  Processes user guess/flag, and updates display board, checks for wins(returned), updates time.
 **********/
short user_turn(const int diff, char const* const* const &b, char **d, gameTime *clock)
//NOTE: use of const* const* const &b used to protect every level of the gameboard from changes.
{
  short s1 = 0, s2 = 0, size = 0;
  
  if (diff == 1){size = ESIZE;}
  else if (diff == 2){size = MSIZE;}
  else{size = HSIZE;}
  
  display(d, size, clock);
  cout << "\nUncover a square by entering it's Row/Column (A4, D3, etc).\n"
       << "Flag (or unflag) a mine by following the selection with -F (A4-F, D3-F, etc).\n\n";
  
  if(get_select(size, d, s1, s2)) //get_select returns true if entry was a -f flag.
  {
    clock->update();
    if (d[s1][s2] == '@'){d[s1][s2] = '?';}  //unflags square if already flagged.
    else{d[s1][s2] = '@';}  //otherwise flags square.
  }
  else
  {
    clock->update();
    if(b[s1][s2] == 'X'){return 1;}  //Win condtion 1 returned if user hits a mine.
    if(b[s1][s2] == ' '){empty_square(size, b, d, s1, s2);}  //Recursively clears surround if white space.
    else{d[s1][s2] = b[s1][s2];}  //Otherwise just uncovers the individual square.
  }
  if(check_win(size, d)){return 2;}  //Win condition 2 returned if board fully cleared/flagged.
  
  return 0;
}


/**********
 * PRE: Requires current difficulty and pointers to the display board and timer.
 * POST: Displays elapsed time, # of mines, and gameboard layout.
 **********/
void display(char const* const* const &b, const int size, const gameTime *clock)
{
  short y = 0, x = 0, mines = 0; 

  //---------------------------------------------------
//Requirement #04: Demonstrate conditional statements
//---------------------------------------------------  
  if (size == ESIZE) {mines = EMINES;}
  else if (size == MSIZE) {mines = MMINES;}
  else {mines = HMINES;}

  clear_screen();

  cout << "Elapsed Time (Last Move) " << *clock << endl 
       << "This grid contains " << mines << " mines.\n\n";
  
  cout << "       ";
  for (short i = 0; i < size; i++){cout << ++x << "     ";}
  cout << endl;  //Prints out top line of column identifiers. 
    
  cout << "       ";
  for (short i = 0; i < size; i++){cout << "|     ";}
  cout << endl;  //Prints out |'s below the column identifiers.

  cout << "    ";
  for (short i = 0; i < size; i++){cout << "------";}
  cout << "-\n";  //Prints out top side of gameboard.

  for (short rows = 0; rows < size; rows++)  //For as many rows as the difficulty dictates. . .
  {
    cout << "    ";
    for (short i = 0; i < size; i++){cout << "|     ";}  //Prints first empty row |    |    | etc.
    cout << "|\n"
         << char('A' + (y)) << " - |";  //Prints row header (A - , B - etc)
    for (short cols = 0; cols < size; cols++)
    {
      for (short j = 0; j < 5; j++)
      {
	if (j == 2){cout << b[rows][cols];}
	else {cout << " ";}
      }
      cout << "|";
    }  //This block prints out row with values (|  3  |  X  |  1  |    | etc)
    cout << " - " << char('A' + (y++)) << endl;  //Prints row header to right of line (- A, - B etc) 
         
    cout << "    ";
    for (short i = 0; i < size; i++){cout << "|     ";}
    cout << "|\n";  //Another empty row  |     |      |   etc.
	 
    cout << "    ";
    for (short i = 0; i < size; i++){cout << "------";}
    cout << "-\n";//And finally a bottom line.  
  } //This completes a 3 ASCII row printout for each row in the gameboard array.
 
  x = 0;  //Resets the colum header identifier to 0.
  cout << "       ";
  for (short i = 0; i < size; i++){cout << "|     ";}
  cout << endl << "       ";
  for (short i = 0; i < size; i++){cout << ++x << "     ";}
  cout << endl;  //This block prints the column idenfiers at the bottom of the board.
}


/**********
 * PRE: Requires current board size, display board and address of 2 shorts to be used as 2D array location.
 * POST: Gets user entry, validates entry and idenfies if entry is a mine flag, or a standard entry.
 * Updates the two shorts with row and column of valid selection, and returns true if a flag command,
 * false otherwise.
 ***********/
bool get_select(const short size, char const* const* const &d, short &s1, short &s2)
{
  bool retry = true;
  string input = "";

  while (retry)
  {
    retry = false;
    s1 = 0;
    s2 = 0;
  
    cout << "Please enter your selection: ";
    getline(cin, input);
    
    if (input.length() >= 2)
    {
      input.at(0) = toupper(input.at(0));
      s2 = (input.at(1) - '0' - 1);
    }
    if (input.length() >= 4){input.at(3) = toupper(input.at(3));}
    
    if (!(input.length() == 2 || (input.length() == 4 && input.at(2) == '-' && input.at(3) == 'F')))
    {
      cout << "I did not understand your entry.  Entry should be <row><col> with an optional <-F>\n"
	   << "Examples:   A4   B3-F   C2   D1-F\n\n";
      retry = true;
    }
    else if(input.at(0) < 'A' || input.at(0) > ('A' + size - 1))
    {
//--------------------------------------------------
//Requirement #03: Demonstrate explicit type casting
//--------------------------------------------------      
      cout << "Rows out of range, row must be between A - " << (char)('A' + size - 1) << endl << endl;
      retry = true;
    }
    else if(s2 < 0 || s2 > (size - 1))
    {
      cout << "Columns out of range, columns must be between 1 - " << size << endl << endl;
      retry = true;
    }
    else
    {
      while (input.at(0) != 'A') //used to convert alpha entry into a numeric row value.
      {
        input.at(0)--;
        s1++;
      }  	
      if (d[s1][s2] == '@' && input.length() == 2)
      {
	cout <<"WARNING you just selected a square you previouslly flagged.  Unflag with -f if you wish.\n";
	retry = true;
      }
      else if (d[s1][s2] != '?' && d[s1][s2] != '@')
      {
        cout << "You've already selected this square, please select another with a ?.\n";
	retry = true;
      }
    }
  }

  if (input.length() == 4){return true;}
  else {return false;}
}


/**********
 * PRE: Requires current board size, pointers to board and display boards, and row and col of empty square.
 * POST: Uncovers any adjacent non-mine squares.  If adjacent square is also empty recursively calls this
 * function to continue the process until all adjacent white space and surrounding squares are cleared.
 **********/
//---------------------------------------------------
//Requirement #17: Demonstrate some form of recursion
//---------------------------------------------------
void empty_square(short size, char const* const* const &b, char **d, short r, short c)
{
  d[r][c] = ' ';  //Uncovers empty square, and following code checks all surrounding.
  
  if (!(r == 0 || c == 0))  //Checks if square to the northwest exists.
  {
    if(b[r-1][c-1] == ' ' && d[r-1][c-1] == '?')
    {
      empty_square(size, b, d, r - 1, c - 1);  //Recursively calls empty_square on the adjacent blank square.
    } 
    else if(d[r-1][c-1] == '?'){d[r-1][c-1] = b[r-1][c-1];} //Only numeric value left, and is revealed.
  }
  if (r!= 0)  //Checks square to the N.
  {
    if(b[r-1][c] == ' ' && d[r-1][c] == '?'){empty_square(size, b, d, r - 1, c);}
    else if(d[r-1][c] == '?'){d[r-1][c] = b[r-1][c];} 
  }
  if (!(r == 0 || c == (size - 1)))  //Checks square to the NE.
  {
    if(b[r-1][c+1] == ' ' && d[r-1][c+1] == '?'){empty_square(size, b, d, r - 1, c + 1);}
    else if(d[r-1][c+1] == '?'){d[r-1][c+1] = b[r-1][c+1];} 
  }
  if (c != (size - 1))  //Checks square to the E.
  {
    if(b[r][c+1] == ' ' && d[r][c+1] == '?'){empty_square(size, b, d, r, c + 1);}
    else if(d[r][c+1] == '?'){d[r][c+1] = b[r][c+1];} 
  }  
  if (!(r == (size - 1) || c == (size - 1)))  //Checks square to the SE.
  {
    if(b[r+1][c+1] == ' ' && d[r+1][c+1] == '?'){empty_square(size, b, d, r + 1, c + 1);}
    else if(d[r+1][c+1] == '?'){d[r+1][c+1] = b[r+1][c+1];} 
  }
  if (r != (size - 1))  //Checks square to the S
  {
    if(b[r+1][c] == ' ' && d[r+1][c] == '?'){empty_square(size, b, d, r + 1, c);}
    else if(d[r+1][c] == '?'){d[r+1][c] = b[r+1][c];} 
  }
  if (!(r == (size - 1) || c == 0)) //Checks square to the SW
  {
    if(b[r+1][c-1] == ' ' && d[r+1][c-1] == '?'){empty_square(size, b, d, r + 1, c - 1);}
    else if(d[r+1][c-1] == '?'){d[r+1][c-1] = b[r+1][c-1];} 
  }
  if (c != 0)  //Checks square to the W
  {
    if(b[r][c-1] == ' ' && d[r][c-1] == '?'){empty_square(size, b, d, r, c - 1);}
    else if(d[r][c-1] == '?'){d[r][c-1] = b[r][c-1];} 
  }
}


/**********
 * PRE: No pre-conditions.
 * POST: Displays the game menu, gets user input and returns value.
 **********/
int menu()
{
  short e = ESIZE, m = MSIZE, h = HSIZE, em = EMINES, mm = MMINES, hm = HMINES;
  
  clear_screen();
//------------------------------------------
//Requirement #01: Demonstrate simple output
//------------------------------------------
  cout << "|******************************************|\n"
       << "|                                          |\n"
       << "|             Minesweeper Lite             |\n"
       << "|                                          |\n"
       << "|   1 - Easy   (" << ESIZE << " X " << ESIZE << " Board, " << EMINES << " Mines)      |\n"
       << "|   2 - Medium (" << MSIZE << " X " << MSIZE << " Board, " << MMINES << " Mines)      |\n"
       << "|   3 - Hard   (" << HSIZE << " X " << HSIZE << " Board, " << HMINES << " Mines)      |\n"
       << "|   4 - High Scores                        |\n"
       << "|   5 - Instructions                       |\n"
       << "|   6 - Quit                               |\n"
       << "|                                          |\n"
       << "|******************************************|\n\n";

//-------------------------------------------------------------
//Requirement #12: Demonstrate general functional decomposition
//-------------------------------------------------------------
  return (get_input("Please make your selection: ", 1, 6));  //Pushes input validation into seperate function.
}


/**********
 * PRE: No pre-conditions.
 * POST: Displays the high score selection menu, gets user selection and calls appropriate high score
 * list.  Otherwise returns to the main menu.
 **********/
void score_menu()
{
  short score_select = 0;

//----------------------------------------------
//Requirement #07: Demonstrate at least one loop
//----------------------------------------------  
  do
  {
    clear_screen();
    cout << "|******************************************|\n"
         << "|                                          |\n"
         << "|               High Scores                |\n"
         << "|                                          |\n"
         << "|   1 - Easy   (" << ESIZE << " X " << ESIZE << " Board, " << EMINES << " Mines)      |\n"
         << "|   2 - Medium (" << MSIZE << " X " << MSIZE << " Board, " << MMINES << " Mines)      |\n"
         << "|   3 - Hard   (" << HSIZE << " X " << HSIZE << " Board, " << HMINES << " Mines)      |\n"
         << "|   4 - Return to Game Menu                |\n"
         << "|                                          |\n"
         << "|                                          |\n"
         << "|                                          |\n"
         << "|******************************************|\n\n";

    score_select = get_input("Please make your selection: ", 1, 4);
    if (score_select < 4){high_score(-1, score_select);}  
    //Calling high_score with -1 arg prevents function from attempting to print current game stats.
 
  }while (score_select != 4);
}


/**********
 * PRE: Requires win condition, difficulty game and display boards as well as timer object.
 * POST: Outputs final game board, and text identifying final game status (mines found, time etc).
 * Returns # of mines correctly identified.*/
short wrap_up(const short result, const int select, char const* const* const &b, 
              char **d, const gameTime *clock)
{
  short size = 0, mines = 0, count = 0;

  if (select == 1){size = ESIZE; mines = EMINES;}
  else if (select == 2){size = MSIZE; mines = MMINES;}
  else{size = HSIZE; mines = HMINES;}

  if (result == 1)
  {
    for (short i = 0; i < size; i++)
    {
      for (short j = 0; j < size; j++)
      {
	if (b[i][j] == 'X' && d[i][j] == '?') {d[i][j] = 'X';}  //Unfound mines = X 
        else if (b[i][j] == 'X' && d[i][j] == '@')
        {
          d[i][j] = '&';  //Found mines = &
          count++;  //and are counted.
        }
      }
    }
    
    display(d, size, clock);
    
    cout << "\nI'm sorry you hit a mine.  You identified " << count << " out of " << mines << " mines.\n"
         << "All mines have now been revealed: X - Mine, & - Flagged Mine, @ - Flagged but no Mine.\n\n"
         << "Press enter to continue. . ."; 
  
    cin.ignore(INT_MAX, '\n');

    return count;  //Returns # of mines found.
  }
  else
  {
    display(d, size, clock);
    
    cout << "\nCongratulations you found all " << mines << " mines!\n"
         << "You're final time was " << *clock << ".  Let's see if you made the high score list!\n\n"
         << "Press enter to continue. . .";
    
    cin.ignore(INT_MAX, '\n'); 
  
    return mines;  //Returns # of mines found (all mines).
  }
}


/**********
 * PRE: Requires # of mines found (negative if called from menu), difficulty setting, 
 * and optionally command line info, and timer object.
 * POST: Pulls in high score data from file, if optional arguments are provided function will identify 
 * if a high score has been beaten, and adjusts the list accordingly.  
 * Prints updated list to console (all cases), and writes to file (if change was made).
 **********/
void high_score(const short found, const int diff, const int argc, char const* const* const &argv, 
                const gameTime *clock)
{
//-------------------------------------------------------------
//Requirement #24: Demonstrate at least one pointer to a struct
//-------------------------------------------------------------  
  score temp, easy[10], med[10], hard[10], *list = NULL;
  short pos = 10, all = 0;

  file_in(easy, med, hard);
//-------------------------------------------------------------
//Requirement #23: Demonstrate at least one pointer to an array
//-------------------------------------------------------------
  if (diff == 1){list = easy; all = EMINES;}
  if (diff == 2){list = med; all = MMINES;}
  if (diff == 3){list = hard; all = HMINES;}

//----------------------------------------------------------
//Requirement #06: Demonstrate at least one bitwise operator
//----------------------------------------------------------
  if (!((found ^ 1) < 0)) //Only run if called after a finished game, (not from game menu).
  {
    get_name (temp, argc, argv);  //Gets valid name from command line, or user input.
    temp.mines = found;
    temp.duration = *clock;

    for (short i = 9; i >= 0; i--)
    {  //Checks in decending order if current game is better then high score list element i.
      if (temp.mines > list[i].mines || (temp.mines == all && temp.duration < list[i].duration))
      {
	pos = i;  //And saves the location current game should fall in the high score array.
      }
    }

    for (short i = 9; i > pos; i--){list[i] = list[i-1];}  //Shifts any lower high scores down 1 spot.
    if (pos <= 9)
    {
      list[pos] = temp;  //Adds current score into high score list.
      file_out(easy, med, hard);  //Updates external txt file if a change occured.
    }
  }

  clear_screen();
  cout << "           Player         Mines        Game\n"
       << "            Name          Found        Time\n"
       << "_________________________________________________\n";
  for (short i = 0; i < 10; i++)
  {
    cout.width(2);
    cout << right << i + 1 << ") - ";
    cout.width(22);
    cout << left << list[i].name;
    cout << list[i].mines << "          ";
    if (list[i].mines == all) {cout << list[i].duration;}  //only displays a time if all mines found.
    else {cout << "--:--";}
    cout << endl << endl;
  }
  if (found >= 0)  //Shows current game status if function not called from menu.
  {
    cout << "You found " << temp.mines << " mines";
    if (temp.mines == all) {cout << " in " << temp.duration;}
    cout << ".\n";
  }   
  cout << "\nPress enter to continue. . .";
  cin.ignore(INT_MAX, '\n');
}


/**********
 * PRE: Requires the three high score arrays be populated.
 * POST: Writes the data to "highScores.txt".
 **********/
void file_out(const score e[], const score m[], const score h[])
{
  ofstream outfile;
  
  outfile.open("highScore.txt");
    
  for (short i = 0; i < 10; i++)
  {
    outfile << e[i].name << "\n" << e[i].mines << " " 
            << e[i].duration.get_min() << " " << e[i].duration.get_sec();
  }
  for (short i = 0; i < 10; i++)
  {
    outfile << m[i].name << "\n" << m[i].mines << " " 
            << m[i].duration.get_min() << " " << m[i].duration.get_sec();
  }
  for (short i = 0; i < 10; i++)
  {
    outfile << h[i].name << "\n" << h[i].mines << " " 
            << h[i].duration.get_min() << " " << h[i].duration.get_sec();
  }
  
  outfile.close();
}


/**********
 * PRE: Requires the "highScore.txt" file be located in same directory as calling program.  Also requires
 * three arrays of struct score be defined with 10 elements each.
 * POST: Reads high score list from file, and assigns to 3 array of structs.
 **********/
void file_in(score e[], score m[], score h[])
{
  ifstream infile;
  short min = 0, sec = 0;
  
  infile.open("highScore.txt");
  if (infile.fail())
  {
    clear_screen();    
    cout << "\n\nError opening highScore.txt.  Please make sure highScore.txt is in the same folder\n"
         << "with the executable program.  Now exiting game. . .\n\n";
    exit (1);
  }

  for (short i = 0; i < 10; i++)
  {
    getline(infile, e[i].name);
    infile >> e[i].mines >> min >> sec;
    e[i].duration.set_min(min);
    e[i].duration.set_sec(sec);
  }
  for (short i = 0; i < 10; i++)
  {
    getline(infile, m[i].name);
    infile >> m[i].mines >> min >> sec;
    m[i].duration.set_min(min);
    m[i].duration.set_sec(sec);
  }
  for (short i = 0; i < 10; i++)
  {
    getline(infile, h[i].name);
    infile >> h[i].mines >> min >> sec;
    h[i].duration.set_min(min);
    h[i].duration.set_sec(sec);
  }
  
  infile.close();
}


/**********
 * PRE: Requires current score struct passed by reference, and command line data.
 * POST: Determines if a valid name was input via command line data, and gets user input otherwise.
 * Adds the validated name to the passed score struct variable.
 **********/
void get_name(score &temp, const int argc, char const* const* const &argv)
{
  string name = "";

//---------------------------------------------------------------
//Requirement #20: Demonstrate at least one command line argument
//---------------------------------------------------------------
  if (argc > 1)
  {
    if (isdigit(argv[1][0]))  //Checks if command line name starts with number, (causes file issues)
    {
      clear_screen();
      cout << "Name captured from command line began with a number.\n"
	   << "Please enter your name (Must start with a letter, <20 chars max): ";
      getline(cin, name);
    }
    else  //Otherwise stores all argv arguments 1 - argc as one single name.
    {
      for (short i = 1; i < argc; i++)
      {
//--------------------------------------------------------------------------------
//Requirement #16: Demonstrate at least one string variable and one c-style string
//--------------------------------------------------------------------------------      
      	name += argv[i];  //name is string, argv[i] is c-style string.
        name += " ";
      }
    }
  }
  else
  {
    name = get_input();
  }

  while (isdigit(name[0]))  //Checks that user entered name doesn't start with number. (causes file issues)
  {
    cout << "Name can not start with a number, please re-enter name (20 chars max): ";
    getline(cin, name);
  } 

  if (name.length() > 20)  //If name is over 20 characters only passes first 20.
  {
    for (short i = 0; i < 20; i++){temp.name += name.at(i);}
  }
  else {temp.name = name;}
}


/**********
 * PRE: Requires current board size, and display board address.
 * POST: Checks display board if all mines have been flagged, and other squares uncovered.  
 * Returns true if game board is uncovered, false if not yet completed.
 **********/
bool check_win(const short size, char const* const* const &d)
{
  short mines = 0, count = 0;

  if (size == ESIZE) {mines = EMINES;}
  else if (size == MSIZE) {mines = MMINES;}
  else {mines = HMINES;}

  for (short i = 0; i < size; i++)
  {
    for (short j = 0; j < size; j++)
    {
      if (d[i][j] == '?') {return false;}
      if (d[i][j] == '@') {count++;}
    }
  }

  if (count == mines) {return true;}
  else {return false;}
}


/**********
 * PRE: No pre-conditions.
 * POST: Clears the screen.
 **********/
void clear_screen()
{
  for (short i = 0; i < 100; i++){cout << endl;}
}


/**********
 * PRE: No pre-conditions.
 * POST:  Request user input for name, returns string.
 **********/
//-------------------------------------------------
//Requirement #15: Demonstrate function overloading
//-------------------------------------------------
string get_input() //Same function name as get_input used for ints, different paramaters.
{
    string temp = "";
    
    clear_screen();
    cout << "Your name was not captured as a command line argument.\n"
         << "Please Enter your name now (20 chars max): ";
    getline(cin, temp);

    return temp;
}


/**********
 * PRE: Requires a string to use as a prompt to the user.  If min/max values (inclusive) are desired, 
 * they can be passed as ints, otherwise INT_MIN & INT_MAX will be used as default.  Min must be <= Max.
 * POST:  Request user input and utilize error checking to ensure a valid int (between min/max if passed) 
 * is returned.
 **********/
int get_input(string ask, int min, int max)
{
  bool again = false;
  int result = 0;

  do
  {
    again = false;
//-----------------------------------------
//Requirement #02: Demonstrate simple input
//-----------------------------------------    
    cout << ask;
    if (!(cin >> result)) 
    {
      cout << "I did not understand your entry, please try again.\n";
      cin.clear();
      cin.ignore(INT_MAX, '\n');
      again = true;
    }
//----------------------------------------------------------
//Requirement #10: Demonstrate at least one debugging trick.
//----------------------------------------------------------      
    else if(result < min)  //Checks for value less then min and informs user, asks for re-entry.
    {
      cout << "Your entry must be " << min << " or higher.\n"
	   << "Please try a larger number.\n";
      cin.sync();
      cin.ignore(INT_MAX, '\n');
      again = true;
    }
    else if(result > max)
    {
      cout << "Your entry must be " << max << " or lower.\n"
	   << "Please try a smaller number.\n";
      cin.sync();
      cin.ignore(INT_MAX, '\n');
      again = true;
    }
    else
    {
      cin.sync();
      cin.ignore(INT_MAX, '\n');
    }
  }while(again);

  return (result);
}
