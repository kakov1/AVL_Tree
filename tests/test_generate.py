from random import randint, choice

requests_number = 15
tests_number = int(input("Enter tests number:\n"))
max_number = int(input("Enter max number:\n"))

for i in range(8, 8 + tests_number):
    test_file = open(f"./tests/{i}test.txt", "w")
    for j in range(requests_number):
        if choice(['q', 'k']) == 'k' or j == 0:
            test_file.write(f"k {randint(1, max_number)} ")
        else:
            test_file.write(f"q {randint(1, max_number)} {randint(1, max_number)} ")
    test_file.close() 