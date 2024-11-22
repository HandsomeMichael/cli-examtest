import os
import sys
import platform
import time

def clear_screen():
    """Clears the terminal screen."""
    if platform.system() == "Windows":
        os.system('cls')
    else:
        os.system('clear')

def load_exam(file_path):
    """Loads the exam questions from the specified file."""
    questions = []
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            for line in file:
                if line.strip():  # Skip empty lines
                    parts = line.strip().split('|')
                    if len(parts) == 3:
                        question, options, answer = parts
                        questions.append({
                            'question': question.strip(),
                            'options': options.split(';'),
                            'answer': answer.strip().lower()
                        })
    except FileNotFoundError:
        print(f"Error: {file_path} not found.")
    return questions

def list_exam_files(directory):
    """Lists all .exam files in the given directory."""
    return [f for f in os.listdir(directory) if f.endswith('.exam')]

def run_exam(questions):
    """Runs the exam and collects user answers."""
    score = 0
    incorrect_answers = []
    clear_screen()
    print("\n--- Starting the Exam ---")
    for idx, q in enumerate(questions):
        clear_screen()
        print(f"\nQuestion {idx + 1}/{len(questions)}: {q['question']}")
        for i, option in enumerate(q['options'], start=1):
            print(f"  {chr(96 + i)}. {option.strip()}")
        user_answer = input("Your answer (a/b/c/d/e): ").lower()
        if user_answer == q['answer']:
            print("Correct!")
            score += 1
        else:
            print(f"Wrong! The correct answer was: {q['answer']}")
            incorrect_answers.append((q['question'], user_answer, q['answer']))
        print(f"Current Score: {score}/{idx + 1}")
        input("\nPress any key to continue...")  # Pause before next question
    return score, incorrect_answers

def view_incorrect_answers(incorrect_answers):
    """Displays questions the user answered incorrectly."""
    clear_screen()
    if not incorrect_answers:
        print("\nNo incorrect answers. Well done!")
    else:
        print("\n--- Incorrect Answers ---")
        for idx, (question, user_answer, correct_answer) in enumerate(incorrect_answers, start=1):
            print(f"\n{idx}. {question}")
            print(f"   Your answer: {user_answer}")
            print(f"   Correct answer: {correct_answer}")
    input("\nPress any key to return to the menu...")

def main():
    while True:
        if len(sys.argv) > 1:  # If a file is dragged
            file_path = sys.argv[1]
            print(f"Using dragged file: {file_path}")
        else:
            # List all .exam files in the current directory
            exam_files = list_exam_files(os.getcwd())
            if not exam_files:
                print("No .exam files found in the current directory. Exiting...")
                return

            print("Available exams:")
            for idx, exam_file in enumerate(exam_files):
                print(f"{idx + 1}. {exam_file}")
            
            # Let the user select an exam
            choice = int(input("Choose an exam by number: ")) - 1
            if choice < 0 or choice >= len(exam_files):
                print("Invalid choice. Exiting...")
                return
            
            file_path = exam_files[choice]
            print(f"Using selected file: {file_path}")
        
        questions = load_exam(file_path)
        if not questions:
            print("Failed to load questions. Exiting...")
            return

        while True:
            clear_screen()
            print(f"Loaded {len(questions)} questions from {file_path}.")
            score, incorrect_answers = run_exam(questions)

            # Show score
            clear_screen()
            print("\n--- Exam Finished ---")
            print(f"Your final score: {score}/{len(questions)}")
            percentage = (score / len(questions)) * 100
            print(f"Your performance: {percentage:.2f}%")
            if percentage >= 90:
                print("Excellent job!")
            elif percentage >= 75:
                print("Good work!")
            elif percentage >= 50:
                print("Needs improvement.")
            else:
                print("Better luck next time!")

            # End options
            print("\nWhat would you like to do next?")
            print("1. View incorrect answers")
            print("2. Restart this exam")
            print("3. Pick another exam")
            print("4. Exit")
            choice = input("Enter your choice: ")

            if choice == '1':
                view_incorrect_answers(incorrect_answers)
            elif choice == '2':
                continue
            elif choice == '3':
                break
            elif choice == '4':
                print("Goodbye!")
                return
            else:
                print("Invalid choice. Please try again.")
                time.sleep(2)

if __name__ == "__main__":
    main()
