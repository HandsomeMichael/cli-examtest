#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <tuple>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace fs = std::filesystem;

// Function to clear the console screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Struct to represent a question
struct Question {
    std::string questionText;
    std::vector<std::string> options;
    char answer; // Stores 'a', 'b', 'c', etc.
};

// Trim utility function (removes leading and trailing spaces)
std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t");
    auto end = str.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Function to load exam questions from a file
std::vector<Question> loadExam(const std::string& filePath) {
    std::vector<Question> questions;
    std::ifstream file(filePath);

    if (!file) {
        std::cerr << "Error: File not found - " << filePath << std::endl;
        return questions;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream stream(line);
        std::string questionPart, optionsPart, answerPart;

        if (std::getline(stream, questionPart, '|') &&
            std::getline(stream, optionsPart, '|') &&
            std::getline(stream, answerPart, '|')) {

            Question question;
            question.questionText = questionPart; // Preserve original spaces in the question

            // Parse options and preserve spaces
            std::istringstream optionsStream(optionsPart);
            std::string option;
            while (std::getline(optionsStream, option, ';')) {
                question.options.push_back(option); // Preserve original spaces in options
            }

            // Trim only the correct answer part
            answerPart = trim(answerPart);

            // Parse and map answer to option letter (e.g., 'a', 'b', 'c')
            if (!answerPart.empty() && std::isdigit(answerPart[0])) {
                int answerIndex = std::stoi(answerPart) - 1; // Convert 1-based index to 0-based
                if (answerIndex >= 0 && answerIndex < static_cast<int>(question.options.size())) {
                    question.answer = 'a' + answerIndex; // Map to 'a', 'b', etc.
                } else {
                    std::cerr << "Warning: Invalid answer index for question: " << question.questionText << "\n";
                    question.answer = ' '; // Invalid answer
                }
            } else if (!answerPart.empty() && std::isalpha(answerPart[0])) {
                question.answer = std::tolower(answerPart[0]); // Direct mapping
            } else {
                std::cerr << "Warning: Invalid answer format for question: " << question.questionText << "\n";
                question.answer = ' '; // Invalid answer
            }

            questions.push_back(question);
        }
    }

    return questions;
}

// Function to scramble the order of questions
void scrambleQuestions(std::vector<Question>& questions) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(questions.begin(), questions.end(), g);
}

// Function to list all `.exam` files in the current directory
std::vector<std::string> listExamFiles(const std::string& directory) {
    std::vector<std::string> examFiles;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == ".exam") {
            examFiles.push_back(entry.path().filename().string());
        }
    }
    return examFiles;
}

// Function to run the exam
std::pair<int, std::vector<std::tuple<std::string, char, char>>>
runExam(const std::vector<Question>& questions) {
    int score = 0;
    std::vector<std::tuple<std::string, char, char>> incorrectAnswers;

    clearScreen();
    std::cout << "\n--- Starting the Exam ---\n";

    for (size_t i = 0; i < questions.size(); ++i) {
        const auto& q = questions[i];
        clearScreen();
        std::cout << "\nQuestion " << (i + 1) << "/" << questions.size() << ": " << q.questionText << "\n";

        for (size_t j = 0; j < q.options.size(); ++j) {
            std::cout << "  " << char('a' + j) << ". " << q.options[j] << "\n";
        }

        char userAnswer;
        std::cout << "Your answer (a/b/c/d/e): ";
        std::cin >> userAnswer;
        userAnswer = std::tolower(userAnswer);

        if (userAnswer == q.answer) {
            std::cout << "Correct!\n";
            ++score;
        } else {
            std::cout << "Wrong! The correct answer was: " << q.answer << "\n";
            incorrectAnswers.emplace_back(q.questionText, userAnswer, q.answer);
        }
        std::cout << "Current Score: " << score << "/" << (i + 1) << "\n";
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }

    return { score, incorrectAnswers };
}

// Function to view incorrect answers
void viewIncorrectAnswers(const std::vector<std::tuple<std::string, char, char>>& incorrectAnswers) {
    clearScreen();
    if (incorrectAnswers.empty()) {
        std::cout << "\nNo incorrect answers. Well done!\n";
    } else {
        std::cout << "\n--- Incorrect Answers ---\n";
        for (size_t i = 0; i < incorrectAnswers.size(); ++i) {
            const auto& [question, userAnswer, correctAnswer] = incorrectAnswers[i];
            std::cout << "\n" << (i + 1) << ". " << question << "\n";
            std::cout << "   Your answer: " << userAnswer << "\n";
            std::cout << "   Correct answer: " << correctAnswer << "\n";
        }
    }
    std::cout << "\nPress Enter to return to the menu...";
    std::cin.ignore();
    std::cin.get();
}

int main() {
    while (true) {
        // List `.exam` files in the current directory
        std::vector<std::string> examFiles = listExamFiles(".");
        if (examFiles.empty()) {
            std::cout << "No .exam files found in the current directory. Exiting...\n";
            return 1;
        }

        std::cout << "Available exams:\n";
        for (size_t i = 0; i < examFiles.size(); ++i) {
            std::cout << (i + 1) << ". " << examFiles[i] << "\n";
        }

        std::cout << "Choose an exam by number: ";
        int choice;
        std::cin >> choice;
        if (choice < 1 || choice > static_cast<int>(examFiles.size())) {
            std::cout << "Invalid choice. Exiting...\n";
            return 1;
        }

        std::string selectedFile = examFiles[choice - 1];
        std::vector<Question> questions = loadExam(selectedFile);

        if (questions.empty()) {
            std::cout << "Failed to load questions. Exiting...\n";
            return 1;
        }

        // Ask if the user wants to scramble the questions
        std::cout << "Do you want to scramble the exam questions? (y/n): ";
        char scrambleChoice;
        std::cin >> scrambleChoice;
        if (std::tolower(scrambleChoice) == 'y') {
            scrambleQuestions(questions);
        }

        while (true) {
            clearScreen();
            std::cout << "Loaded " << questions.size() << " questions from " << selectedFile << ".\n";
            auto [score, incorrectAnswers] = runExam(questions);

            // Display final score
            clearScreen();
            std::cout << "\n--- Exam Finished ---\n";
            std::cout << "Your final score: " << score << "/" << questions.size() << "\n";
            double percentage = (static_cast<double>(score) / questions.size()) * 100.0;
            std::cout << "Your performance: " << percentage << "%\n";

            if (percentage >= 90) std::cout << "Excellent job!\n";
            else if (percentage >= 75) std::cout << "Good work!\n";
            else if (percentage >= 50) std::cout << "Needs improvement.\n";
            else std::cout << "Better luck next time!\n";

            // Menu options
            std::cout << "\nWhat would you like to do next?\n";
            std::cout << "1. View incorrect answers\n";
            std::cout << "2. Restart the same exam\n";
            std::cout << "3. Exit\n";

            int menuChoice;
            std::cin >> menuChoice;

            if (menuChoice == 1) {
                viewIncorrectAnswers(incorrectAnswers);
            } else if (menuChoice == 2) {
                break; // Restart the exam
            } else {
                return 0;
            }
        }
    }

    return 0;
}
