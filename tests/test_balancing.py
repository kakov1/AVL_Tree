from random import randint

nodes_number = int(input("Enter nodes number:\n"))
max_number = int(input("Enter max number:\n"))
test_file = open(input("Enter test file path:\n"), 'w')

for i in range(nodes_number):
    test_file.write(f"k {randint(1, max_number)} ")
test_file.close() 