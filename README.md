# Jackfruit_C_Test_Provider
# Jackfruit C Test Provider

A command-line based quiz/test system written in C using file handling.
This project simulates an exam portal with timed questions, scoring, and a leaderboard.

---

## Features

* Multiple-choice quiz system
* Timer-based questions (15 seconds per question)
* Randomized question selection
* Score calculation and result display
* Leaderboard with ranking
* One attempt per user (based on roll number)
* Answer review after test
* Persistent storage using files

---

## How It Works

* Questions are loaded from `questions.txt`
* User attempts are stored in `attempts.txt`
* The main logic is implemented in:

  * `main1.c`
  * `time.c`

### Flow:

1. User enters name and roll number
2. System checks eligibility (no re-attempts)
3. Random questions are displayed
4. Each question has a time limit
5. Score is calculated
6. Result is saved
7. Leaderboard is updated

---

## How to Run

### Step 1: Compile

```bash
gcc main1.c time.c -o test
```

### Step 2: Run

```bash
./test
```

---

## File Structure

```
.
├── main1.c
├── time.c
├── questions.txt
├── attempts.txt
├── README.md
```

---

## Technologies Used

* C Programming
* File Handling
* Time Functions (`time.h`)
* Console Input Handling (`conio.h`)

---

## Notes

* Works best on Windows (uses `conio.h`)
* Each question has a strict time limit
* Users cannot reattempt the test with the same roll number

---

## Contributors

* Sakshi Ghodke

---

## Future Improvements

* GUI-based version
* Database integration
* Web-based deployment
* Admin panel for question management

