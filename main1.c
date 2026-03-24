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
#define TIME_LIMIT 15
#define MAX_CANDIDATES 1000

typedef struct {
    char question[MAX_LENGTH];
    char options[4][MAX_LENGTH];
    char correct_answer;
    int asked; 
} Question;

typedef struct {
    char name[MAX_LENGTH];
    char rollno[MAX_LENGTH];
    int score;
    time_t timestamp;
    int time_taken;
} Candidate;

void loadQuestions(Question questions[], int *count);
void conductTest(Question questions[], int count, Candidate *current);
void reviewTest(Question testQuestions[], char userAnswers[], int score);
int getTimeLeft(time_t start_time);
bool isEligible(const char *rollno);
void saveAttempt(Candidate candidate);
void showLeaderboard();
void displayCurrentCandidateRank(Candidate current);
void shuffleQuestions(Question questions[], int count);
void initializeQuestions(Question questions[], int count);

int main() {
    srand(time(NULL));
    
    Question questions[TOTAL_QUESTIONS];
    int questionCount = 0;
    Candidate current;
    char choice;

    // Initialize questions
    loadQuestions(questions, &questionCount);
    initializeQuestions(questions, questionCount);

    while (1) {
        system("cls");
        printf("====================\n");
        printf("  EXAM PORTAL\n");
        printf("====================\n");
        printf("1. Attempt Test\n");
        printf("2. View Leaderboard\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        
        choice = _getch();
        printf("%c\n", choice);
        
        switch (choice) {
            case '1': {
                system("cls");
                printf("Enter your name: ");
                fgets(current.name, MAX_LENGTH, stdin);
                current.name[strcspn(current.name, "\n")] = '\0';

                printf("Enter your roll number: ");
                fgets(current.rollno, MAX_LENGTH, stdin);
                current.rollno[strcspn(current.rollno, "\n")] = '\0';
                
                if (!isEligible(current.rollno)) {
                    printf("\nYou have already attempted the test.\n");
                    printf("Press any key to continue...");
                    _getch();
                    break;
                }

                current.score = 0;
                current.timestamp = time(NULL);

                shuffleQuestions(questions, questionCount);

                time_t test_start = time(NULL);
                conductTest(questions, questionCount, &current);
                current.time_taken = (int)(time(NULL) - test_start);

                saveAttempt(current);
                displayCurrentCandidateRank(current);

                printf("\nPress any key to continue...");
                _getch();
                break;
            }
            case '2':
            system("cls");
                showLeaderboard();
                printf("\nPress any key to continue...");
                _getch();
                break;
            case '3':
            return 0;
            default:
                printf("Invalid choice! Try again.\n");
                printf("Press any key to continue...");
                _getch();
        }
    }


    return 0;
}

void initializeQuestions(Question questions[], int count) {
    for (int i = 0; i < count; i++) {
        questions[i].asked = 0;
    }
}

void shuffleQuestions(Question questions[], int count) {
    for (int i = 0; i < count; i++) {
        int j = rand() % count;
        Question temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}

void loadQuestions(Question questions[], int *count) {
    FILE *file = fopen("questions.txt", "r");
    if (file == NULL) {
        printf("Error opening questions file!\n");
        return;
    }

    char line[MAX_LENGTH];
    *count = 0;

    while (fgets(line, sizeof(line), file) && *count < TOTAL_QUESTIONS) {
        if (strlen(line) > 1) {
            strcpy(questions[*count].question, line);

            for (int i = 0; i < 4; i++) {
                if (fgets(questions[*count].options[i], sizeof(questions[*count].options[i]), file) == NULL) {
                    break;
                }
            }

            if (fgets(line, sizeof(line), file) == NULL) {
                break;
            }
            questions[*count].correct_answer = toupper(line[0]);
            (*count)++;
        }
    }

    fclose(file);
}

bool isEligible(const char *rollno) {
    FILE *file = fopen("attempts.txt", "r");
    if (file == NULL) {
        return true; 
    }

    char line[MAX_LENGTH * 3];
    while (fgets(line, sizeof(line), file)) {
        char savedRollno[MAX_LENGTH];
        sscanf(line, "%*[^,],%[^,]", savedRollno);
        if (strcmp(savedRollno, rollno) == 0) {
            fclose(file);
            return false;
        }
    }

    fclose(file);
    return true;
}

void saveAttempt(Candidate candidate) {
    FILE *file = fopen("attempts.txt", "a");
    if (file == NULL) {
        printf("Error saving attempt!\n");
        return;
    }

    fprintf(file, "%s,%s,%d,%ld,%d\n", 
            candidate.name, 
            candidate.rollno, 
            candidate.score, 
            candidate.timestamp,
            candidate.time_taken);

    fclose(file);
}

int compareCandidates(const void *a, const void *b) {
    const Candidate *ca = (const Candidate *)a;
    const Candidate *cb = (const Candidate *)b;

    // Higher score comes first
    if (ca->score != cb->score) {
        return cb->score - ca->score;
    }

    // If scores are equal, faster time comes first
    if (ca->time_taken != cb->time_taken) {
        return ca->time_taken - cb->time_taken;
    }

    // If both score and time are equal, earlier attempt comes first
    return (int)(ca->timestamp - cb->timestamp);
}

void showLeaderboard() {
    FILE *file = fopen("attempts.txt", "r");
    if (file == NULL) {
        printf("No attempts recorded yet!\n");
        return;
    }

    Candidate candidates[MAX_CANDIDATES];
    int count = 0;
    char line[MAX_LENGTH * 3];

    while (fgets(line, sizeof(line), file) && count < MAX_CANDIDATES) {
        sscanf(line, "%[^,],%[^,],%d,%ld,%d", 
               candidates[count].name, 
               candidates[count].rollno,
               &candidates[count].score,
               &candidates[count].timestamp,
               &candidates[count].time_taken);
        count++;
    }

    fclose(file);

    if (count == 0) {
        printf("No attempts recorded yet!\n");
        return;
    }

    qsort(candidates, count, sizeof(Candidate), compareCandidates);

    printf("====================\n");
    printf("   LEADERBOARD\n");
    printf("====================\n");
    printf("Rank  Name               Roll No          Score    Time\n");
    printf("---------------------------------------------------------\n");

    int displayCount = count > 10 ? 10 : count;
    for (int i = 0; i < displayCount; i++) {
        printf("%-4d  %-18s %-18s %-5d  %d sec\n", 
               i + 1, 
               candidates[i].name, 
               candidates[i].rollno,
               candidates[i].score,
               candidates[i].time_taken);
    }
}

void displayCurrentCandidateRank(Candidate current) {
    FILE *file = fopen("attempts.txt", "r");
    if (file == NULL) {
        printf("Error loading leaderboard!\n");
        return;
    }

    Candidate candidates[MAX_CANDIDATES];
    int count = 0;
    char line[MAX_LENGTH * 3];

    while (fgets(line, sizeof(line), file) && count < MAX_CANDIDATES) {
        sscanf(line, "%[^,],%[^,],%d,%ld,%d", 
               candidates[count].name, 
               candidates[count].rollno,
               &candidates[count].score,
               &candidates[count].timestamp,
               &candidates[count].time_taken);
        count++;
    }

    fclose(file);

    candidates[count++] = current;
    qsort(candidates, count, sizeof(Candidate), compareCandidates);

    for (int i = 0; i < count; i++) {
        if (strcmp(candidates[i].rollno, current.rollno) == 0) {
            printf("\nYour rank: %d out of %d\n", i + 1, count-1);
            break;
        }
    }
}

void conductTest(Question questions[], int count, Candidate *current) {
    Question testQuestions[TEST_QUESTIONS];
    char userAnswers[TEST_QUESTIONS];
    int questionsUsed = 0;

    // Select questions that haven't been asked before
    for (int i = 0; i < count && questionsUsed < TEST_QUESTIONS; i++) {
        if (!questions[i].asked) {
            testQuestions[questionsUsed] = questions[i];
            questions[i].asked = 1;
            questionsUsed++;
        }
    }

    // If not enough new questions, use some repeats
    if (questionsUsed < TEST_QUESTIONS) {
        for (int i = 0; i < count && questionsUsed < TEST_QUESTIONS; i++) {
            testQuestions[questionsUsed] = questions[i];
            questionsUsed++;
        }
    }

    // Start the test
    for (int i = 0; i < TEST_QUESTIONS; i++) {
        time_t start_time = time(NULL);
        int time_left = TIME_LIMIT;
        char answer = '\0';

        // Display question and options
        printf("\nQuestion %d: %s", i + 1, testQuestions[i].question);
        for (int j = 0; j < 4; j++) {
            printf("%s", testQuestions[i].options[j]);
        }

        // Wait for answer or timeout
        while (time_left > 0 && answer == '\0') {
            printf("\rTime remaining: %2d seconds | Enter answer (A/B/C/D): ", time_left);
            fflush(stdout);

            if (_kbhit()) {
                answer = _getch();
                answer = toupper(answer);

                if (answer >= 'A' && answer <= 'D') {
                    printf("\nYou selected: %c\n", answer);
                }
                else {
                    printf("\nInvalid input! Press A, B, C, or D.\n");
                    answer = '\0';
                }
            }

            time_left = getTimeLeft(start_time);
            if (time_left <= 0) {
                printf("\nTime's up! Moving to next question.\n");
                answer = 'X';
            }
        }

        userAnswers[i] = answer;

        // Update score
        if (answer == testQuestions[i].correct_answer) {
            current->score++;
        }
    }

    // Display results
    printf("\nTest completed! \nYour score: %d/%d\n", current->score, TEST_QUESTIONS);

    // Offer review
    char choice;
    printf("\nDo you want to review your answers? (Y --> YES || Anything else --> NO): ");
    choice = toupper(_getch());
    printf("%c\n", choice);

    if (choice == 'Y') {
        reviewTest(testQuestions, userAnswers, current->score);
    }
}

void reviewTest(Question testQuestions[], char userAnswers[], int score) {
    printf("\n--- Review Your Answers ---\n");
    for (int i = 0; i < TEST_QUESTIONS; i++) {
        printf("\nQuestion %d: %s", i + 1, testQuestions[i].question);
        for (int j = 0; j < 4; j++) {
            printf("%s", testQuestions[i].options[j]);
        }

        if (userAnswers[i] == 'X') {
            printf("Your answer: (Timeout)\n");
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
    printf("\nFinal Score: %d/%d\n", score, TEST_QUESTIONS);
}

int getTimeLeft(time_t start_time) {
    return TIME_LIMIT - (int)(time(NULL) - start_time);
}
