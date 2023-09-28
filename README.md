# Network Programming
### Network Programming homework

This is a small **Study Schedule Management program** for students in the university. The program works allow
students to: 

- Login (using student ID and password)
- Read schedule of one weekday by providing the weekday. For example: student provides “Thursday” and the program return list of all courses and schedule of the courses of the day
- Read week schedule (when user enter “ALL”)

Internally, the program store the list of registered courses (by students) and their schedules in 3 text files: *course_schedule.txt*, *student_registration.txt*, *user_account.txt*

**RUN INSTRUCTION:**
```bash
gcc -o main main.c users/user.c courses/courses.c registrations/registration.c
./main
```

***Important note for Ubuntu:***

In `main.c`, replace data file paths as follow:
```
#define userFilePath "../data/user_account.txt"
#define courseFilePath "../data/course_schedule.txt"
#define registrationFilePath "../data/student_registration.txt" 
```




*Some problem remain:*

- Haven't solved the problem of studying one subject from morning to afternoon
- Remain case sensitive when the user enters weekday
- Need to change all global variables to pointers
- The search algorithm is not optimized