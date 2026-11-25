
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