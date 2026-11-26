// ================= MODULE 7: Exception classes =================
// penalty = credibility lost, usesTurn = the mistake also costs a turn
class GameException {
    string type;
    string message;
    int penalty;
    bool usesTurn;
public:
    GameException(const string& t, const string& m, int p, bool u);
    virtual ~GameException();
    string getType() const;
    string getMessage() const;
    int getPenalty() const;
    bool consumesTurn() const;
};
class InvalidInputException : public GameException { public: InvalidInputException(const string& m); };
class InvalidPositionException : public GameException { public: InvalidPositionException(const string& m); };
class SlotFilledException : public GameException { public: SlotFilledException(const string& m); };
class WrongLetterException : public GameException { public: WrongLetterException(const string& m); };
class RackFullException : public GameException { public: RackFullException(const string& m); };
class RackEmptyException : public GameException { public: RackEmptyException(const string& m); };
class WordNotReadyException : public GameException { public: WordNotReadyException(const string& m); };
class FileException : public GameException { public: FileException(const string& m); };
class InvalidWordException : public GameException { public: InvalidWordException(const string& m, int p); };
class LowCredibilityException : public GameException { public: LowCredibilityException(const string& m, int p); };
class WrongAccusationException : public GameException { public: WrongAccusationException(const string& m, int p); };

// ================= MODULE 8: Template DynamicArray<T> =================
// Generic array on the heap with copy constructor, assignment operator and destructor (deep copy).
template <typename T>
class DynamicArray {
    T* data;
    int capacity;
    int count;
    void grow(int minCapacity) {
        int newCap = capacity * 2;
        if (newCap < minCapacity) newCap = minCapacity;
        T* bigger = new T[newCap];
        for (int i = 0; i < count; i++) bigger[i] = data[i];
        delete[] data;
        data = bigger;
        capacity = newCap;
    }
public:
    DynamicArray(int cap = 4) {
        capacity = (cap < 1) ? 1 : cap;
        count = 0;
        data = new T[capacity];
    }
    DynamicArray(const DynamicArray& other) {
        capacity = other.capacity;
        count = other.count;
        data = new T[capacity];
        for (int i = 0; i < count; i++) data[i] = other.data[i];
    }
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            count = other.count;
            data = new T[capacity];
            for (int i = 0; i < count; i++) data[i] = other.data[i];
        }
        return *this;
    }
    ~DynamicArray() { delete[] data; }

    void add(const T& item) { if (count == capacity) grow(count + 1); data[count] = item; count++; }
    void resize(int n) { if (n > capacity) grow(n); for (int i = count; i < n; i++) data[i] = T(); count = n; }
    void clear() { count = 0; }
    int size() const { return count; }
    T& operator[](int i) { if (i < 0 || i >= count) throw InvalidPositionException("Index is outside the array."); return data[i]; }
    const T& operator[](int i) const { if (i < 0 || i >= count) throw InvalidPositionException("Index is outside the array."); return data[i]; }
};

// ================= Helper functions =================
const int MAX_FIELDS = 12;                      // most columns any data file row can have
char toUpperChar(char c);
string toUpperString(string s);
string intToText(int n);
int textToInt(const string& s);
string readLine(const string& prompt);
int readInt(const string& prompt, int low, int high);
bool readYesNo(const string& prompt);
void openFile(ifstream& in, const string& name, bool skipHeader);
bool readRow(ifstream& in, string fields[], int minColumns, const string& fileName);
string fixRole(string role);
int roleColumn(const string& role);

// ================= MODULE 11: Operator overloading on structs =================
bool operator==(const Clue& a, const Clue& b);  // compares letter, position, row
ostream& operator<<(ostream& os, const Tile& t);
ostream& operator<<(ostream& os, const Location& l);
// ================= MODULE 4: Cottage (aggregation of Location array) =================
class Cottage {
    DynamicArray<Location> locations;      // earlier raw DMA: Location* locations = new Location[7];
public:
    Cottage();
    Cottage(const Cottage& other);         // copy constructor: deep copy
    Cottage& operator=(const Cottage& other);
    ~Cottage();                            // frees the array
    void addLocation(const Location& l);
    void clearLocations();
    int getLocationCount() const;
    Location& at(int i);
    Clue getClue(int i) const;
    void displayLocations() const;
};

// ================= MODULE 4: Scrabble board =================
class Scrabble {
    DynamicArray<Tile> row1;               // 7 slots for Word 1 (earlier raw DMA: Tile* row1 = new Tile[7];)
    DynamicArray<Tile> row2;               // 8 slots for Word 2 (earlier raw DMA: Tile* row2 = new Tile[8];)
    string target1;
    string target2;
public:
    Scrabble();
    Scrabble(const Scrabble& o);           // copy constructor: deep copy
    Scrabble& operator=(const Scrabble& o);
    ~Scrabble();
    void setTargets(const string& w1, const string& w2);
    void placeClue(const Clue& c);
    bool isRowComplete(WordRow r) const;
    bool checkWord(WordRow r, const string& w) const;
    friend ostream& operator<<(ostream& os, const Scrabble& s);
};

// ================= MODULE 5: TileRack =================
// The rack stores a Clue (letter + position) so it can show both.
class TileRack {
    DynamicArray<Clue> letters;            // earlier raw DMA: Clue* letters = new Clue[capacity];
    int capacity;
public:
    TileRack(int cap = 8);
    TileRack(const TileRack& o);           // copy constructor: deep copy
    TileRack& operator=(const TileRack& o);
    ~TileRack();
    int getCount() const;
    void addClue(const Clue& c);
    Clue removeLast();
    void display() const;
    TileRack operator+(const TileRack& o) const;     // merge two racks, space between
};
// ================= MODULES 3 and 9: Character hierarchy =================
class Character {
protected:
    string characterName;
    string characterRole;
    string nationality;
    bool isCulprit;
    bool interviewed;
    WordRow clueRowChar;
    char letterClueChar;
    int boardPosChar;
public:
    Character(const string& n, const string& r, const string& nat);
    virtual ~Character();
    virtual void display() const = 0;                          // pure virtual: Character is abstract
    virtual int requiredCredibility() const;
    virtual bool isSuspect() const;
    string getName() const;
    void setClue(char letter, int pos);
    Clue getClue() const;
    friend ostream& operator<<(ostream& os, const Character& c);
    friend class GameEngine;                                   // MODULE 11: friend class
};

class Victim : public Character {
public:
    Victim(const string& n, const string& r, const string& nat);
    void display() const;
    int requiredCredibility() const;
};

class Suspect : public Character {
public:
    Suspect(const string& n, const string& r, const string& nat);
    void display() const;
    bool isSuspect() const;
};

// Virtual inheritance: PA gets only ONE Suspect part (solves the diamond problem).
class BusinessRelations : virtual public Suspect {
public:
    BusinessRelations(const string& n, const string& r, const string& nat);
    void display() const;
    int requiredCredibility() const;
};

class CloseRelations : virtual public Suspect {
public:
    CloseRelations(const string& n, const string& r, const string& nat);
    void display() const;
    int requiredCredibility() const;
};

class HouseHelp : public Suspect {
public:
    HouseHelp(const string& n, const string& r, const string& nat);
    void display() const;
};

class Partner : public BusinessRelations {
public:
    Partner(const string& n, const string& r, const string& nat);
};

class Child : public CloseRelations {
public:
    Child(const string& n, const string& r, const string& nat);
};

class PA : public BusinessRelations, public CloseRelations {   // the diamond
public:
    PA(const string& n, const string& r, const string& nat);
    void display() const;
    int requiredCredibility() const;
};

class Cook : public HouseHelp { public: Cook(const string& n, const string& r, const string& nat); };
class Guard : public HouseHelp { public: Guard(const string& n, const string& r, const string& nat); };
class Servant : public HouseHelp { public: Servant(const string& n, const string& r, const string& nat); };
class Detective : public Character {
    int credibility;
    int turns;                                  // the engine increments, the detective only stores
    TileRack rack1;
    TileRack rack2;
public:
    Detective(const string& n = "Detective", int cred = 100);
    void display() const;
    int getCredibility() const;
    void loseCredibility(int amount);
    int getTurns() const;
    void incrementTurns();
    void collectClue(const Clue& c);
    const TileRack& getRack(WordRow r) const;
};
// ================= MODULE 6: Leader / Score board =================
class LeaderBoard {
    string filePath;
public:
    LeaderBoard(const string& path);
    void addScore(const string& name, const string& result, int cred, int turnsLeft, const string& culprit) const;
    void display() const;
};
// ================= MODULE 12: GameEngine (Singleton) =================
// Composition: Detective, Cottage, Scrabble, characters.  Association: LeaderBoard.
class GameEngine {
    static GameEngine* instance;
    GameState state;
    int maxTurns;
    int startingCredibility;
    int wrongTurnPenalty;
    int wrongWordPenalty;
    string winCondition;
    string loseCondition;
    Detective detective;
    Cottage cottage;
    Scrabble scrabble;
    DynamicArray<Character*> characters;        // heap objects owned by the engine
    LeaderBoard* leaderBoard;
    Character* culprit;
    string culpritRole;
    string word1;
    string word2;
    bool solved1;
    bool solved2;

    GameEngine();
    GameEngine(const GameEngine&);              // copying is not allowed (singleton)
    GameEngine& operator=(const GameEngine&);
    ~GameEngine();
    void clearCharacters();
    Character* createCharacter(const string& name, const string& role, const string& nat);
    void loadRules();
    void loadGameData();
    void runMainMenu();
    void startNewGame();
    void runGameTurn();
    void collect(bool& done, const string& name, const Clue& c, int needCred, const string& past, const string& action);
    void interviewPerson();
    void formWord();
    void accuseSuspect();
    void concludeGame();
public:
    static GameEngine& getInstance();
    static void destroyInstance();
    void attachLeaderBoard(LeaderBoard* b);
    void showWelcome() const;
    void step();
    void handleException(const GameException& e);
    bool isFinished() const;
};