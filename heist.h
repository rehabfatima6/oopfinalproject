
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