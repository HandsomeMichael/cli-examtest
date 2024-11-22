#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN "cls"
#else
#include <unistd.h>
#define CLEAR_SCREEN "clear"
#endif

using namespace std;
namespace fs = std::filesystem;

struct Question {
    string question;
    vector<string> options;
    char correctAnswer;
};

void clearScreen() {
    system(CLEAR_SCREEN);
}

vector<Question> loadExam(const string& filePath) {
    vector<Question> questions;
    ifstream file(filePath);
    if (!file) {
        cerr << "Error: Could not open file " << filePath << endl;
        return questions;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string questionPart, optionsPart, correctAnswerPart;

        if (getline(iss, questionPart, '|') &&
            getline(iss, optionsPart, '|') &&
            getline(iss, correctAnswerPart)) {
            Question q;
            q.question = questionPart;

            // Split options by ';'
            istringstream optionsStream(optionsPart);
            string option;
            while (getline(optionsStream, option, ';')) {
                q.options.push_back(option);
            }

            q.correctAnswer = correctAnswerPart[0]; // First character is the correct answer
            questions.push_back(q);
        }
    }

    return questions;
}

vector<string> detectExamFiles() {
    vector<string> examFiles;
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        if (entry.path().extension() == ".exam") {
            examFiles.push_back(entry.path().filename().string());
        }
    }
    return examFiles;
}

void displayQuestion(const Question& q, int currentIndex, int totalQuestions) {
    clearScreen();
    cout << "Question " << currentIndex + 1 << " of " << totalQuestions << ":\n";
    cout << q.question << "\n";
    char optionChar = 'a';
    for (const auto& option : q.options) {
        cout << "  " << optionChar++ << ". " << option << "\n";
    }
}

int runExam(const vector<Question>& questions, vector<int>& incorrectIndices) {
    int score = 0;
    for (size_t i = 0; i < questions.size(); ++i) {
        displayQuestion(questions[i], i, questions.size());
        cout << "\nYour answer (a/b/c/d/e): ";
        char answer;
        cin >> answer;
        answer = tolower(answer);

        if (answer == questions[i].correctAnswer) {
            cout << "Correct!\n";
            ++score;
        } else {
            cout << "Wrong! The correct answer was: " << questions[i].correctAnswer << "\n";
            incorrectIndices.push_back(i);
        }

        cout << "Current Score: " << score << "/" << i + 1 << "\n";
        cout << "\nPress any key to continue...";
        cin.ignore();
        cin.get();
    }
    return score;
}

void viewIncorrectAnswers(const vector<Question>& questions, const vector<int>& incorrectIndices) {
    clearScreen();
    if (incorrectIndices.empty()) {
        cout << "No incorrect answers. Well done!\n";
    } else {
        cout << "--- Incorrect Answers ---\n";
        for (size_t idx : incorrectIndices) {
            cout << "\n" << questions[idx].question << "\n";
            cout << "  Correct answer: " << questions[idx].correctAnswer << "\n";
        }
    }
    cout << "\nPress any key to return to the menu...";
    cin.ignore();
    cin.get();
}

void mainMenu(const string& filePath) {
    vector<Question> questions = loadExam(filePath);
    if (questions.empty()) {
        return;
    }

    while (true) {
        clearScreen();
        cout << "Loaded " << questions.size() << " questions from " << filePath << "\n";
        vector<int> incorrectIndices;
        int score = runExam(questions, incorrectIndices);

        clearScreen();
        cout << "--- Exam Finished ---\n";
        cout << "Your final score: " << score << "/" << questions.size() << "\n";
        double percentage = (double(score) / questions.size()) * 100;
        cout << "Your performance: " << fixed << setprecision(2) << percentage << "%\n";

        cout << "\nWhat would you like to do next?\n";
        cout << "1. View incorrect answers\n";
        cout << "2. Restart this exam\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            viewIncorrectAnswers(questions, incorrectIndices);
        } else if (choice == 2) {
            continue;
        } else if (choice == 3) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
            sleep(2);
        }
    }
}

int main(int argc, char* argv[]) {
    string filePath;

    if (argc > 1) { // If a file is passed as an argument
        filePath = argv[1];
    } else {
        vector<string> examFiles = detectExamFiles();
        if (examFiles.empty()) {
            cout << "No .exam files found in the current directory. Exiting...\n";
            return 0;
        }

        cout << "Available .exam files:\n";
        for (size_t i = 0; i < examFiles.size(); ++i) {
            cout << i + 1 << ". " << examFiles[i] << "\n";
        }

        cout << "Choose an exam by number: ";
        int choice;
        cin >> choice;

        if (choice < 1 || choice > static_cast<int>(examFiles.size())) {
            cout << "Invalid choice. Exiting...\n";
            return 0;
        }

        filePath = examFiles[choice - 1];
    }

    mainMenu(filePath);
    return 0;
}
