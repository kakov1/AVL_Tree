# AVL_Tree
Реализация бинарного дерева поиска с алгоритмом балансировки с помощью AVL-инварианта.
# Клонирование репозитория
```
git clone https://github.com/kakov1/AVL_Tree.git
```
# Настройка виртуального окружения
```
python3 -m venv .venv
source .venv/bin/activate
pip3 install conan
conan profile detect --force
```
# Сборка проекта
```
conan install . --output-folder=build --build=missing
cd build/
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
# Запуск основной программы
```
cd src/
./tree
```
# Запуск тестов
```
cd tests/
./test_tree
```
# Примечание
Чтобы визуализировать дерево необходима библиотека Graphviz. Установка на ubuntu:
```
sudo apt-get install graphviz
sudo apt-get install libgraphviz-dev
```
