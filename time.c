#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <conio.h>

#define TOTAL_QUESTIONS 100
#define TEST_QUESTIONS 10
#define MAX_LENGTH 200
#define TIME_LIMIT 15 // Time limit per question in seconds

typedef struct {
    char question[MAX_LENGTH];
    char options[4][MAX_LENGTH];
    char correct_answer;
} Question;

void loadQuestions(Question questions[], int *count);
void conductTest(Question questions[], int count);
void reviewTest(Question testQuestions[], char userAnswers[], int score);
int getTimeLeft(time_t start_time);

int main() {
    Question questions[TOTAL_QUESTIONS];
    int questionCount = 0;

    loadQuestions(questions, &questionCount);

    if (questionCount == 0) {
        printf("No questions found. Exiting...\n");
        return 1;
    }

    conductTest(questions, questionCount);

    return 0;
}

void loadQuestions(Question questions[], int *count) {
    FILE *file = fopen("questions.txt", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char line[MAX_LENGTH];
    *count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) > 1) {
            strcpy(questions[*count].question, line);

            for (int i = 0; i < 4; i++) {
                fgets(questions[*count].options[i], sizeof(questions[*count].options[i]), file);
            }

            fgets(line, sizeof(line), file);
            questions[*count].correct_answer = line[0];
            (*count)++;
        }
    }

    fclose(file);
}

int getTimeLeft(time_t start_time) {
    return TIME_LIMIT - (time(NULL) - start_time);
}

void conductTest(Question questions[], int count) {
    srand(time(0));
    Question testQuestions[TEST_QUESTIONS];
    char userAnswers[TEST_QUESTIONS];
    int score = 0;

    for (int i = 0; i < TEST_QUESTIONS; i++) {
        int randomIndex = rand() % count;
        testQuestions[i] = questions[randomIndex];
    }

    for (int i = 0; i < TEST_QUESTIONS; i++) {
        time_t start_time = time(NULL);
        int time_left = TIME_LIMIT;
        char answer = '\0';

        printf("\nQuestion %d: %s", i + 1, testQuestions[i].question);
        for (int j = 0; j < 4; j++) {
            printf("%c) %s", 'A' + j, testQuestions[i].options[j]);
        }

        while (time_left > 0 && answer == '\0') {
            printf("\rTime remaining: %2d seconds | Enter answer (A/B/C/D): ", time_left);
            fflush(stdout);

            if (kbhit()) {
                answer = getchar();
                while (getchar() != '\n'); // Clear input buffer
                answer = toupper(answer);
                
                if (answer < 'A' || answer > 'D') {
                    printf("\nInvalid input! Please enter A, B, C, or D.\n");
                    answer = '\0';
                }
            }

            time_left = getTimeLeft(start_time);
            if (time_left <= 0) {
                printf("\n\nTime's up! Moving to next question.\n");
                answer = 'X'; // Mark as unanswered due to timeout
            }
        }

        userAnswers[i] = answer;

        if (answer == testQuestions[i].correct_answer) {
            score++;
        }
    }

    printf("\nTest completed! \n\nYour score: %d/%d\n", score, TEST_QUESTIONS);

    char choice;
    printf("\nDo you want to review your answers? (Y/N): ");
    scanf(" %c", &choice);

    if (toupper(choice) == 'Y') {
        reviewTest(testQuestions, userAnswers, score);
    } else {
        printf("....THANK YOU....");
    }
}

void reviewTest(Question testQuestions[], char userAnswers[], int score) {
    printf("\n--- Review Your Answers ---\n");
    for (int i = 0; i < TEST_QUESTIONS; i++) {
        printf("\nQuestion %d: %s", i + 1, testQuestions[i].question);
        for (int j = 0; j < 4; j++) {
            printf("%c) %s", 'A' + j, testQuestions[i].options[j]);
        }

        if (userAnswers[i] == 'X') {
            printf("Your answer: (No answer - Timeout)\n");
        } else {
            printf("Your answer: %c\n", userAnswers[i]);
        }
        printf("Correct answer: %c\n", testQuestions[i].correct_answer);

        if (userAnswers[i] == testQuestions[i].correct_answer) {
            printf("Result: Correct!\n");
        } else {
            printf("Result: Incorrect!\n");
        }
    }

    printf("\nFinal Score: %d/%d\n\n....THANK YOU....", score, TEST_QUESTIONS);
}

int kbhit(void) {
    return _kbhit();  // Windows built-in keyboard hit check
}