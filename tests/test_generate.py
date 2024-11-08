from random import randint, choice

requests_number = 100
start_test_number = int(input("Enter start test number:\n"))
tests_number = int(input("Enter tests number:\n"))
max_number = int(input("Enter max number:\n"))

for i in range(start_test_number, start_test_number + tests_number):
    test_file = open(f"./tests/{i}test.txt", "w")
    for j in range(requests_number):
        if choice(['q', 'k']) == 'k':
            test_file.write(f"k {randint(-max_number, max_number)} ")
        else:
            test_file.write(f"q {randint(-max_number, max_number)} {randint(-max_number, max_number)} ")
    test_file.close() 