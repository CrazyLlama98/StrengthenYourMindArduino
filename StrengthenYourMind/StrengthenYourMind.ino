#include <LiquidCrystal.h>
#include <time.h>

const int MAXIM_NUMBER_LEDS = 5;
const int FIRST_PIN_NUMBER = 2;

//ButtonLed class - handles the buttons and the corresponding leds
class ButtonLed
{
private:
  int listenningPin;

public:
  ButtonLed(int _listenningPin = 0) : listenningPin(_listenningPin){};

  void listenToPin(int _pin);
  bool isPressed();
  void lightUpLed();
  void lightDownLed();
};

void ButtonLed::listenToPin(int _pin)
{
  listenningPin = _pin;
}

bool ButtonLed::isPressed()
{
  int buttonState = digitalRead(listenningPin);
  return buttonState == HIGH;
}

void ButtonLed::lightUpLed()
{
  digitalWrite(listenningPin, HIGH);
}

void ButtonLed::lightDownLed()
{
  digitalWrite(listenningPin, LOW);
}

//ScoreManager class - deals with level constants and manages player statistics
class ScoreManager
{
private:
  int currentLevel, remainingLives, delayBetweenBlinks, numberRounds, currentRound, blinksPerRound, blinkDuration;
  double currentScore, scorePerRound, mistakeScore;
  static ScoreManager *instance;
  static const double SCORE_PER_ROUND_INCREASE_PERCENT, SCORE_PER_MISTAKE_BEGIN_VALUE;
  static const double DELAY_DECREASE_PERCENT, BLINK_DURATION_DECREASE_PERCENT;
  static const int DELAY_BEGIN_VALUE, LIVES_BEGIN_VALUE, NUMBER_ROUNDS_BEGIN_VALUE, BLINKS_PER_ROUND_BEGIN_VALUE;
  static const int BLINK_DURATION_BEGIN_VALUE;
  ScoreManager()
  {
    currentLevel = 1;
    remainingLives = LIVES_BEGIN_VALUE;
    delayBetweenBlinks = DELAY_BEGIN_VALUE;
    numberRounds = NUMBER_ROUNDS_BEGIN_VALUE;
    currentRound = 1;
    currentScore = 0;
    blinksPerRound = BLINKS_PER_ROUND_BEGIN_VALUE;
    blinkDuration = BLINK_DURATION_BEGIN_VALUE;
    scorePerRound = SCORE_PER_ROUND_BEGIN_VALUE;
    mistakeScore = SCORE_PER_MISTAKE_BEGIN_VALUE;
  };

public:
  static ScoreManager &getInstance();

  //constants
  static const double SCORE_PER_ROUND_BEGIN_VALUE;

  //getters
  int getCurrentLevel() const { return currentLevel; };
  int getRemainingLives() const { return remainingLives; };
  int getDelayBetweenBlinks() const { return delayBetweenBlinks; };
  int getNumberRounds() const { return numberRounds; };
  int getCurrentRound() const { return currentRound; };
  double getCurrentScore() const { return currentScore; };
  int getBlinksPerRound() const { return blinksPerRound; };

  //setters
  void setNumberRounds(const int _nrRounds) { numberRounds = _nrRounds; };

  //methods
  void decreaseCurrentScore();
  void decreaseLives();
  void nextRound();
  void nextLevel();
  void reset();
};
// static and constants
ScoreManager *ScoreManager::instance = nullptr;
const double ScoreManager::SCORE_PER_ROUND_INCREASE_PERCENT = 0.4;
const double ScoreManager::SCORE_PER_ROUND_BEGIN_VALUE = 20;
const double ScoreManager::SCORE_PER_MISTAKE_BEGIN_VALUE = 5;
const double ScoreManager::DELAY_DECREASE_PERCENT = 0.2;
const double ScoreManager::BLINK_DURATION_DECREASE_PERCENT = 0.2;
const int ScoreManager::DELAY_BEGIN_VALUE = 1000;
const int ScoreManager::LIVES_BEGIN_VALUE = 3;
const int ScoreManager::NUMBER_ROUNDS_BEGIN_VALUE = 3;
const int ScoreManager::BLINKS_PER_ROUND_BEGIN_VALUE = 6;
const int ScoreManager::BLINK_DURATION_BEGIN_VALUE = 1500;

ScoreManager &ScoreManager::getInstance()
{
  if (instance == nullptr)
    instance = new ScoreManager();
  return *instance;
}

void ScoreManager::reset()
{
  currentLevel = 1;
  remainingLives = LIVES_BEGIN_VALUE;
  delayBetweenBlinks = DELAY_BEGIN_VALUE;
  numberRounds = NUMBER_ROUNDS_BEGIN_VALUE;
  currentRound = 1;
  currentScore = 0;
  blinksPerRound = BLINKS_PER_ROUND_BEGIN_VALUE;
  scorePerRound = SCORE_PER_ROUND_BEGIN_VALUE;
  mistakeScore = SCORE_PER_MISTAKE_BEGIN_VALUE;
}

void ScoreManager::decreaseCurrentScore()
{
  if (currentScore >= mistakeScore)
    currentScore -= mistakeScore;
  else
    currentScore = 0;
}

void ScoreManager::decreaseLives()
{
  --remainingLives;
}

void ScoreManager::nextRound()
{
  if (currentRound < numberRounds) 
  {
    ++currentRound;
    currentScore += scorePerRound;
  }
}

void ScoreManager::nextLevel()
{
  ++currentLevel;
  delayBetweenBlinks -= delayBetweenBlinks * DELAY_DECREASE_PERCENT;
  currentRound = 1;
  ++blinksPerRound;
  blinkDuration -= blinkDuration * BLINK_DURATION_DECREASE_PERCENT;
  currentScore += scorePerRound;
  scorePerRound += scorePerRound * SCORE_PER_ROUND_INCREASE_PERCENT;
  mistakeScore += mistakeScore * SCORE_PER_ROUND_INCREASE_PERCENT;
}

//Menu class - wrapper for LiquidCrystal
class Menu
{
private:
  LiquidCrystal *screen;
  static Menu *instance;
  Menu()
  {
    screen = new LiquidCrystal(12, 11, 10, 9, 8, 7);
    screen->begin(16, 2);
  }

public:
  static Menu &getInstance();
  void print(int row, String message);
  void print(String message);
  void setCursor(int row, int column = 0);
  void clear();

  ~Menu()
  {
    delete screen;
    delete instance;
    instance = nullptr;
  }
};

Menu *Menu::instance = nullptr;

Menu &Menu::getInstance()
{
  if (instance == nullptr)
    instance = new Menu();
  return *instance;
}

void Menu::print(int row, String message)
{
  screen->setCursor(0, row);
  screen->print(message);
}

void Menu::clear()
{
  screen->clear();
}

void Menu::print(String message) 
{
  screen->print(message);
}
  
void Menu::setCursor(int row, int column = 0)
{
  screen->setCursor(column, row);
}

//JoyStick class - deals with signals sent by the joystick
class JoyStick
{
private:
  int pinX, pinY, pinSwitch;

public:
  JoyStick(int _pinX, int _pinY, int _pinSwitch)
      : pinX(_pinX), pinY(_pinY), pinSwitch(_pinSwitch)
  {
    pinMode(_pinSwitch, INPUT);
  };

  void setPinX(const int _pinX) { pinX = _pinX; };
  void setPinY(const int _pinY) { pinY = _pinY; };
  void setPinSwitch(const int _pinSwitch) { pinSwitch = _pinSwitch; };

  int getX() const { return analogRead(pinX); };
  int getY() const { return analogRead(pinY); };
  bool isPressed() const { return digitalRead(pinSwitch) == LOW; };
};

//GameManager class - deals with player turn and the game flows
class GameManager
{
private:
  static GameManager *instance;
  int *correctBlinks;
  int correctGuesses, nrButtonsPressed;
  ScoreManager *scoreManager;
  Menu *menu;
  JoyStick *joyStick;
  ButtonLed leds[MAXIM_NUMBER_LEDS];
  bool gameStarted;

  GameManager() : gameStarted(false), correctGuesses(0), nrButtonsPressed(0)
  {
    for (int i = 0; i < MAXIM_NUMBER_LEDS; ++i)
    {
      leds[i].listenToPin(i + FIRST_PIN_NUMBER);
    }
    correctBlinks = nullptr;
    scoreManager = &ScoreManager::getInstance();
    menu = &Menu::getInstance();
    joyStick = new JoyStick(0, 1, 13);
  };

public:
  ~GameManager()
  {
    delete scoreManager;
    delete menu;
    delete joyStick;
    delete instance;
    instance = nullptr;
  };

  static GameManager &getInstance();
  void generateBlinks();
  bool isGameStarted();
  void setupGame();
  bool buttonWasPressed();
  bool waitForUserTurn();
  void reinitRoundBlinks();
  void startGame();
  void update();
  void nextRound();
  void stopGame();
  void printLevelDetails();
  int getPressedButtonIndex();
  bool isBlinkCorrect(int buttonPressed);
  void handleWrongGuess();
  void handleCorrectGuess();
  void replayBlinks();
  int getCorrectGuesses() const { return correctGuesses; };
};

GameManager *GameManager::instance = nullptr;

GameManager &GameManager::getInstance()
{
  if (instance == nullptr)
    instance = new GameManager();
  return *instance;
}

void GameManager::printLevelDetails()
{
  menu->clear();
  String levelMessage = "Lvl ", roundMessage = "Round ", scoreMessage = "Score ", livesMessage = "Lives ";
  menu->print(0, levelMessage + scoreManager->getCurrentLevel());
  menu->setCursor(0, 6);
  menu->print(scoreMessage + scoreManager->getCurrentScore());
  menu->print(1, roundMessage + scoreManager->getCurrentRound());
  menu->setCursor(1, 8);
  menu->print(livesMessage + scoreManager->getRemainingLives());
}

void GameManager::generateBlinks()
{
  reinitRoundBlinks();
  printLevelDetails();
  correctBlinks = new int[scoreManager->getBlinksPerRound()];
  for (int i = 0; i < scoreManager->getBlinksPerRound(); ++i)
  {
    int buttonToBlink = random(MAXIM_NUMBER_LEDS);
    correctBlinks[i] = buttonToBlink;
    leds[buttonToBlink].lightUpLed();
    delay(scoreManager->getDelayBetweenBlinks());
    leds[buttonToBlink].lightDownLed();
    if (i != scoreManager->getBlinksPerRound() - 1)
      delay(scoreManager->getDelayBetweenBlinks());
  }
}

void GameManager::reinitRoundBlinks()
{
  if (correctBlinks != nullptr)
  {
    delete[] correctBlinks;
    correctBlinks = nullptr;
  }
  correctGuesses = 0;
  nrButtonsPressed = 0;
}

void GameManager::startGame()
{
  gameStarted = true;
}

bool GameManager::buttonWasPressed()
{
  return joyStick->isPressed();
}

bool GameManager::isGameStarted()
{
  return gameStarted;
}

void GameManager::setupGame()
{
  menu->clear();
  menu->print(0, "Press joystick");
  menu->print(1, "to start game!");
}

bool GameManager::waitForUserTurn()
{
  return correctBlinks != nullptr;
}

bool GameManager::isBlinkCorrect(int buttonPressed)
{
  return correctBlinks[correctGuesses] == buttonPressed;
}

void GameManager::replayBlinks()
{
  for (int i = 0; i < scoreManager->getBlinksPerRound(); ++i) 
  {
    leds[correctBlinks[i]].lightUpLed();
    delay(scoreManager->getDelayBetweenBlinks());
    leds[correctBlinks[i]].lightDownLed();
    if (i != scoreManager->getBlinksPerRound() - 1)
      delay(scoreManager->getDelayBetweenBlinks());
  }
}

void GameManager::handleWrongGuess()
{
  menu->clear();
  menu->print(0, "Wrong!");
  scoreManager->decreaseCurrentScore();
  String scoreMessage = "Score ";
  menu->print(1, scoreMessage + scoreManager->getCurrentScore());
  scoreManager->decreaseLives();
  delay(1000);
  if (scoreManager->getRemainingLives() != 0)
  {
    menu->clear();
    menu->print(0, "The Round will ");
    menu->print(1, "be replayed!");
    correctGuesses = 0;
    nrButtonsPressed = 0;
    replayBlinks();
    delay(500);
  }
  else
  {
    stopGame();
  }
}

void GameManager::stopGame()
{
  reinitRoundBlinks();
  gameStarted = false;
  printLevelDetails();
  delay(2000);
  setupGame();
  scoreManager->reset();
}

void GameManager::update()
{
  delay(250);
  int pressedButtonIndex = getPressedButtonIndex();
  printLevelDetails();
  if (!waitForUserTurn())
  {
    generateBlinks();
  }
  else if (pressedButtonIndex != -1)
  {
    ++nrButtonsPressed;
    if (!isBlinkCorrect(pressedButtonIndex))
    {
      handleWrongGuess();
    }
    else
    {
      handleCorrectGuess();
    }
  }
}

void GameManager::handleCorrectGuess()
{
  ++correctGuesses;
  menu->clear();
  menu->print(0, "Correct!");
  delay(500);
  if (correctGuesses == scoreManager->getBlinksPerRound() && correctGuesses == nrButtonsPressed)
  {
    menu->clear();
    menu->print(0, "Next Round will");
    menu->print(1, "begin!");
    reinitRoundBlinks();
    nextRound();
    delay(1000);
  }
}

void GameManager::nextRound()
{
  if (scoreManager->getCurrentRound() == scoreManager->getNumberRounds())
    scoreManager->nextLevel();
  else
    scoreManager->nextRound();
}

int GameManager::getPressedButtonIndex()
{
  for (int i = 0; i < MAXIM_NUMBER_LEDS; ++i)
  {
    if (leds[i].isPressed())
      return i;
  }
  return -1;
}

GameManager &gameManager = GameManager::getInstance();

void setup()
{
  // put your setup code here, to run once:
  randomSeed(millis());
  gameManager.setupGame();
  Serial.begin(9600);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (gameManager.isGameStarted())
  {
    gameManager.update();
  }
  else if (gameManager.buttonWasPressed())
  {
    gameManager.startGame();
  }
}
