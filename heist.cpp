// ================= Exception classes =================
GameException::GameException(const string& t, const string& m, int p, bool u) : type(t), message(m), penalty(p), usesTurn(u) {}
GameException::~GameException() {}
string GameException::getType() const { return type; }
string GameException::getMessage() const { return message; }
int GameException::getPenalty() const { return penalty; }
bool GameException::consumesTurn() const { return usesTurn; }
InvalidInputException::InvalidInputException(const string& m) : GameException("InvalidInputException", m, 0, false) {}
InvalidPositionException::InvalidPositionException(const string& m) : GameException("InvalidPositionException", m, 0, false) {}
SlotFilledException::SlotFilledException(const string& m) : GameException("SlotFilledException", m, 0, false) {}
WrongLetterException::WrongLetterException(const string& m) : GameException("WrongLetterException", m, 0, false) {}
RackFullException::RackFullException(const string& m) : GameException("RackFullException", m, 0, false) {}
RackEmptyException::RackEmptyException(const string& m) : GameException("RackEmptyException", m, 0, false) {}
WordNotReadyException::WordNotReadyException(const string& m) : GameException("WordNotReadyException", m, 0, false) {}
FileException::FileException(const string& m) : GameException("FileException", m, 0, false) {}
InvalidWordException::InvalidWordException(const string& m, int p) : GameException("InvalidWordException", m, p, true) {}
LowCredibilityException::LowCredibilityException(const string& m, int p) : GameException("LowCredibilityException", m, p, true) {}
WrongAccusationException::WrongAccusationException(const string& m, int p) : GameException("WrongAccusationException", m, p, true) {}

// ================= Helper functions =================
char toUpperChar(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
    return c;
}

string toUpperString(string s) {
    for (int i = 0; i < (int)s.size(); i++) s[i] = toUpperChar(s[i]);
    return s;
}

string intToText(int n) {                       // number -> text
    if (n == 0) return "0";
    string s = "";
    bool negative = (n < 0);
    if (negative) n = -n;
    while (n > 0) { s = char('0' + n % 10) + s; n = n / 10; }
    if (negative) s = "-" + s;
    return s;
}

int textToInt(const string& s) {                // text -> number (used for file values)
    int value = 0;
    bool negative = false;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '-') negative = true;
        else if (s[i] >= '0' && s[i] <= '9') value = value * 10 + (s[i] - '0');
    }
    if (negative) value = -value;
    return value;
}

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    if (!getline(cin, s)) {
        cin.clear();
        throw InvalidInputException("Input could not be read.");
    }
    return s;
}

int readInt(const string& prompt, int low, int high) {
    string s = readLine(prompt);
    if (s.size() == 0 || s.size() > 6)
        throw InvalidInputException("Please enter a valid number.");
    int value = 0;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] < '0' || s[i] > '9')
            throw InvalidInputException("Please enter a number, not text.");
        value = value * 10 + (s[i] - '0');
    }
    if (value < low || value > high)
        throw InvalidInputException("Choice is out of range (" + intToText(low) + " to " + intToText(high) + ").");
    return value;
}

bool readYesNo(const string& prompt) {
    string s = toUpperString(readLine(prompt));
    if (s == "Y") return true;
    if (s == "N") return false;
    throw InvalidInputException("Please answer with Y or N.");
}

void openFile(ifstream& in, const string& name, bool skipHeader) {
    in.open(name.c_str());
    if (!in)
        throw FileException(name + " not found.");
    if (skipHeader) { string header; getline(in, header); }
}

// Reads the next non-empty line of a '|' separated file into fields. Returns false at end of file.
bool readRow(ifstream& in, string fields[], int minColumns, const string& fileName) {
    string line;
    while (getline(in, line)) {
        if (line.size() > 0 && line[line.size() - 1] == '\r') line.erase(line.size() - 1, 1);
        if (line.size() == 0) continue;
        int count = 0;
        string current = "";
        for (int i = 0; i < (int)line.size(); i++) {
            if (line[i] == '|') {
                if (count < MAX_FIELDS) { fields[count] = current; count++; }
                current = "";
            }
            else current += line[i];
        }
        if (count < MAX_FIELDS) { fields[count] = current; count++; }
        if (count < minColumns)
            throw FileException(fileName + " is corrupted (too few columns).");
        return true;
    }
    return false;
}

string fixRole(string role) {                   // "Partner 1" -> "Partner_1"
    for (int i = 0; i < (int)role.size(); i++) if (role[i] == ' ') role[i] = '_';
    return role;
}

int roleColumn(const string& role) {            // which profile column belongs to this role
    const char* roles[7] = { "Cook", "Guard", "Servant", "Partner_1", "Partner_2", "PA", "Child" };
    for (int i = 0; i < 7; i++) if (role == roles[i]) return i;
    return -1;
}

// ================= Operator overloading =================
bool operator==(const Clue& a, const Clue& b) {
    return toUpperChar(a.clueLetter) == toUpperChar(b.clueLetter) && a.cluePosition == b.cluePosition && a.clueRow == b.clueRow;
}

ostream& operator<<(ostream& os, const Tile& t) {
    if (t.filled) os << "[" << t.tileLetter << "]";
    else os << "[ ]";
    return os;
}

ostream& operator<<(ostream& os, const Location& l) {
    os << l.locName;
    if (l.locSearchFlag) os << " - Searched";
    else os << " - NOT Searched";
    return os;
}
// ================= Character hierarchy =================
Character::Character(const string& n, const string& r, const string& nat): characterName(n), characterRole(r), nationality(nat), isCulprit(false), interviewed(false), clueRowChar(WORD_2), letterClueChar(' '), boardPosChar(0) {}

Character::~Character() {}

int Character::requiredCredibility() const { return 0; }
bool Character::isSuspect() const { return false; }
string Character::getName() const { return characterName; }
void Character::setClue(char letter, int pos) { letterClueChar = letter; boardPosChar = pos; }

Clue Character::getClue() const {
    Clue c;
    c.clueLetter = letterClueChar;
    c.cluePosition = boardPosChar;
    c.clueRow = clueRowChar;
    return c;
}

ostream& operator<<(ostream& os, const Character& c) {
    os << c.characterName << " (" << c.characterRole << ", " << c.nationality << ")";
    return os;
}

Victim::Victim(const string& n, const string& r, const string& nat) : Character(n, r, nat) {}
void Victim::display() const { cout << "Victim: " << *this << "\n"; }
int Victim::requiredCredibility() const { return 80; }

Suspect::Suspect(const string& n, const string& r, const string& nat) : Character(n, r, nat) {}
void Suspect::display() const { cout << "Suspect: " << *this << "\n"; }
bool Suspect::isSuspect() const { return true; }

BusinessRelations::BusinessRelations(const string& n, const string& r, const string& nat) : Suspect(n, r, nat) {}
void BusinessRelations::display() const { cout << "Business relation: " << *this << "\n"; }
int BusinessRelations::requiredCredibility() const { return 50; }

CloseRelations::CloseRelations(const string& n, const string& r, const string& nat) : Suspect(n, r, nat) {}
void CloseRelations::display() const { cout << "Close relation: " << *this << "\n"; }
int CloseRelations::requiredCredibility() const { return 75; }

HouseHelp::HouseHelp(const string& n, const string& r, const string& nat) : Suspect(n, r, nat) {}
void HouseHelp::display() const { cout << "House help: " << *this << "\n"; }

// The virtual base class Suspect is initialised by the most derived class.
Partner::Partner(const string& n, const string& r, const string& nat) : Suspect(n, r, nat), BusinessRelations(n, r, nat) {}
Child::Child(const string& n, const string& r, const string& nat) : Suspect(n, r, nat), CloseRelations(n, r, nat) {}

PA::PA(const string& n, const string& r, const string& nat)
    : Suspect(n, r, nat), BusinessRelations(n, r, nat), CloseRelations(n, r, nat) {}
void PA::display() const { cout << "Personal Assistant: " << *this << "\n"; }
int PA::requiredCredibility() const { return 35; }

Cook::Cook(const string& n, const string& r, const string& nat) : HouseHelp(n, r, nat) {}
Guard::Guard(const string& n, const string& r, const string& nat) : HouseHelp(n, r, nat) {}
Servant::Servant(const string& n, const string& r, const string& nat) : HouseHelp(n, r, nat) {}
Detective::Detective(const string& n, int cred)
    : Character(n, "Detective", "Unknown"), credibility(cred), turns(0)
    , rack1(7), rack2(8)
{}

void Detective::display() const { cout << "Detective " << characterName << " | Credibility: " << credibility << " | Turns used: " << turns << "\n"; }
int Detective::getCredibility() const { return credibility; }
void Detective::loseCredibility(int amount) { credibility = credibility - amount; if (credibility < 0) credibility = 0; }
int Detective::getTurns() const { return turns; }
void Detective::incrementTurns() { turns++; }
void Detective::collectClue(const Clue& c) { if (c.clueRow == WORD_1) rack1.addClue(c); else rack2.addClue(c); }
const TileRack& Detective::getRack(WordRow r) const { if (r == WORD_1) return rack1; return rack2; }

// ================= Cottage =================
Cottage::Cottage() : locations(7) {}
Cottage::Cottage(const Cottage& other) : locations(other.locations) {}
Cottage& Cottage::operator=(const Cottage& other) { if (this != &other) locations = other.locations; return *this; }
Cottage::~Cottage() {}
void Cottage::addLocation(const Location& l) { locations.add(l); }
void Cottage::clearLocations() { locations.clear(); }
int Cottage::getLocationCount() const { return locations.size(); }
Location& Cottage::at(int i) { return locations[i]; }

Clue Cottage::getClue(int i) const {
    Clue c;
    c.clueLetter = locations[i].letterClueLoc;
    c.cluePosition = locations[i].boardPosLoc;
    c.clueRow = locations[i].locRow;
    return c;
}

void Cottage::displayLocations() const {
    cout << "\n===== Location Menu =====\n";
    for (int i = 0; i < locations.size(); i++) {
        cout << "[" << i + 1 << "] " << locations[i] << "\n";
    }
}

// ================= Scrabble =================
Scrabble::Scrabble() : row1(7), row2(8) { row1.resize(7); row2.resize(8); }
Scrabble::Scrabble(const Scrabble& o) : row1(o.row1), row2(o.row2), target1(o.target1), target2(o.target2) {}
Scrabble& Scrabble::operator=(const Scrabble& o) {
    if (this != &o) { row1 = o.row1; row2 = o.row2; target1 = o.target1; target2 = o.target2; }
    return *this;
}
Scrabble::~Scrabble() {}

void Scrabble::setTargets(const string& w1, const string& w2) {
    if (w1.size() != 7 || w2.size() != 8)
        throw FileException("clue.txt is corrupted: Word_1 needs 7 letters and Word_2 needs 8.");
    target1 = toUpperString(w1);
    target2 = toUpperString(w2);
    row1.clear(); row1.resize(7);               // fresh empty board
    row2.clear(); row2.resize(8);
}

void Scrabble::placeClue(const Clue& c) {
    DynamicArray<Tile>& row = (c.clueRow == WORD_1) ? row1 : row2;
    int size = row.size();
    const string& target = (c.clueRow == WORD_1) ? target1 : target2;

    if (c.cluePosition < 1 || c.cluePosition > size)
        throw InvalidPositionException("That position does not exist on the board.");
    Tile& slot = row[c.cluePosition - 1];
    if (slot.filled)
        throw SlotFilledException("That position is already filled.");

    Clue expected;                              // the letter this slot should contain
    expected.clueLetter = target[c.cluePosition - 1];
    expected.cluePosition = c.cluePosition;
    expected.clueRow = c.clueRow;
    if (!(c == expected))
        throw WrongLetterException("That letter does not belong at that position.");

    slot.tileLetter = toUpperChar(c.clueLetter);
    slot.filled = true;
}

bool Scrabble::isRowComplete(WordRow r) const {
    const DynamicArray<Tile>& row = (r == WORD_1) ? row1 : row2;
    int size = row.size();
    for (int i = 0; i < size; i++) if (!row[i].filled) return false;
    return true;
}

bool Scrabble::checkWord(WordRow r, const string& w) const {
    if (!isRowComplete(r)) return false;
    if (r == WORD_1) return toUpperString(w) == target1;
    return toUpperString(w) == target2;
}

ostream& operator<<(ostream& os, const Scrabble& s) {
    os << "Scrabble:\n  Word 1: ";
    for (int i = 0; i < s.row1.size(); i++) os << s.row1[i];
    if (s.isRowComplete(WORD_1)) os << "  -> COMPLETE";
    os << "\n  Word 2: ";
    for (int i = 0; i < s.row2.size(); i++) os << s.row2[i];
    if (s.isRowComplete(WORD_2)) os << "  -> COMPLETE";
    os << "\n";
    return os;
}

// ================= TileRack =================
TileRack::TileRack(int cap) : letters(cap), capacity(cap) {}
TileRack::TileRack(const TileRack& o) : letters(o.letters), capacity(o.capacity) {}
TileRack& TileRack::operator=(const TileRack& o) { if (this != &o) { letters = o.letters; capacity = o.capacity; } return *this; }
TileRack::~TileRack() {}
int TileRack::getCount() const { return letters.size(); }

void TileRack::addClue(const Clue& c) {
    if (getCount() >= capacity)
        throw RackFullException("The rack is full.");
    letters.add(c);
}

Clue TileRack::removeLast() {
    if (getCount() == 0)
        throw RackEmptyException("The rack is empty.");
    Clue last = letters[getCount() - 1];
    letters.resize(letters.size() - 1);
    return last;
}

void TileRack::display() const {
    if (getCount() == 0) { cout << "(empty)"; return; }
    for (int i = 0; i < getCount(); i++) {
        if (letters[i].clueLetter == ' ') cout << "  ";                  // separator between merged racks
        else cout << "[" << letters[i].clueLetter << "@" << letters[i].cluePosition << "] ";
    }
}

TileRack TileRack::operator+(const TileRack& o) const {                  // merge both racks as one clue
    TileRack merged(capacity + o.capacity + 1);
    for (int i = 0; i < getCount(); i++) merged.addClue(letters[i]);
    Clue space;
    space.clueLetter = ' ';
    space.cluePosition = 0;
    space.clueRow = WORD_1;
    merged.addClue(space);
    for (int i = 0; i < o.getCount(); i++) merged.addClue(o.letters[i]);
    return merged;
}



// ================= GameEngine =================
GameEngine* GameEngine::instance = NULL;

GameEngine::GameEngine()
    : state(STATE_MENU), maxTurns(25), startingCredibility(100), wrongTurnPenalty(5), wrongWordPenalty(10),
      winCondition("Accuse_Correct_Culprit"), loseCondition("Credibility_Zero_Or_Turns_Exhausted"),
      leaderBoard(NULL), culprit(NULL), solved1(false), solved2(false) {}

GameEngine::~GameEngine() { clearCharacters(); }

void GameEngine::attachLeaderBoard(LeaderBoard* b) { leaderBoard = b; }
bool GameEngine::isFinished() const { return state == STATE_EXIT; }

GameEngine& GameEngine::getInstance() {
    if (instance == NULL) instance = new GameEngine();
    return *instance;
}

void GameEngine::destroyInstance() {
    delete instance;
    instance = NULL;
}

void GameEngine::runGameTurn() {
    cout << "\n";
    detective.display();
    cout << "Rack Word 1: "; detective.getRack(WORD_1).display();
    cout << "\nRack Word 2: "; detective.getRack(WORD_2).display();
    cout << "\n\n==== Game Menu ====\n";
    cout << "[1] Visit a Location\n[2] Interview a Person\n[3] View Scrabble Board (free - no turn)\n";
    cout << "[4] Form a Word\n[5] Accuse a Suspect\n[6] Quit\n";
    cout << "--- TURN " << detective.getTurns() + 1 << " ---\n";
    int choice = readInt("Choice: ", 1, 6);
    if (choice == 1) {
        cottage.displayLocations();
        int pick = readInt("Pick: ", 1, cottage.getLocationCount());
        int i = pick - 1;
        collect(cottage.at(i).locSearchFlag, cottage.at(i).locName, cottage.getClue(i), 0, "searched", "Search");
    }
    else if (choice == 2) interviewPerson();
    else if (choice == 3) {
        cout << "\n" << scrabble;
    }
    else if (choice == 4) formWord();
    else if (choice == 5) accuseSuspect();
    else { cout << "You abandon the case. (Quit games are not recorded.)\n"; state = STATE_MENU; }
}

// Same steps for a location and for a person: first time = collect and place the letter,
// later = ask and show the letter again. 'done' is the searched/interviewed flag.
void GameEngine::collect(bool& done, const string& name, const Clue& c, int needCred, const string& past, const string& action) {
    int wordNo = (c.clueRow == WORD_1) ? 1 : 2;
    if (!done) {
        cout << "You have NOT " << past << " " << name << " yet.\n";
        if (!readYesNo(action + "? (Y/N): ")) return;
        if (detective.getCredibility() < needCred)
            throw LowCredibilityException(name + " refuses to talk: needs credibility of at least " + intToText(needCred) + ".", wrongTurnPenalty);
        detective.collectClue(c);
        scrabble.placeClue(c);
        done = true;
        cout << "Letter " << c.clueLetter << " collected at position " << c.cluePosition << " of Word " << wordNo << ".\n";
        cout << "Letter has been placed.\n";
    } else {
        cout << "You have already " << past << " " << name << ".\n";
        if (!readYesNo(action + " again? (Y/N): ")) return;
        cout << "Letter " << c.clueLetter << " is at position " << c.cluePosition << " of Word " << wordNo << ".\n";
    }
    detective.incrementTurns();
}

void GameEngine::interviewPerson() {
    cout << "\n===== Character Menu =====\n";
    for (int i = 0; i < characters.size(); i++) {
        cout << "[" << i + 1 << "] " << *characters[i] << " - ";
        if (characters[i]->interviewed) cout << "Interviewed\n";
        else cout << "NOT Interviewed\n";
    }
    int pick = readInt("Pick: ", 1, characters.size());
    Character* p = characters[pick - 1];
    // requiredCredibility() is virtual: each class gives its own limit (polymorphism)
    collect(p->interviewed, p->characterName, p->getClue(), p->requiredCredibility(), "interviewed", "Interview");
}
