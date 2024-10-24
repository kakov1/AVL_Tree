from random import randint

nodes_number = int(input("Enter nodes number:\n"))
max_number = int(input("Enter max number:\n"))
test_file = open(input("Enter test file path:\n"), 'w')

numbers = []
for i in range(nodes_number):
    number = randint(1, max_number)
    while number in numbers:
        number = randint(1, max_number)
    test_file.write(f"k {randint(1, max_number)} ")
    numbers.append(number)
test_file.close() 