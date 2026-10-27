#include <iostream>
#include <vector>
#include <string>
#include <thread>

void myInit(std::vector<short>& myVector, int start, int end) {
    for (int i = start; i < end; i++)
        myVector.at(i) = rand() % 31 - 10;
}

void printVector(const std::vector<short>& myVector) {
    for (short x : myVector)
        std::cout << x << " ";
    std::cout << std::endl;
}

std::vector<short>& bubbleSort(std::vector<short>& myVector) {
    int n = myVector.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (myVector[j] > myVector[j + 1]) {
                // Меняем элементы местами
                short temp = myVector[j];
                myVector[j] = myVector[j + 1];
                myVector[j + 1] = temp;
            }
        }
    }
    return myVector;
}

size_t separateFunction(std::vector<short>& vect, size_t start, size_t finish) {
    size_t i = start, j = start - 1;
    for (; i < finish; i++) {
        if (vect.at(i) <= vect.at(finish - 1)) {
            j++;
            std::swap(vect.at(i), vect.at(j));
        }
    }
    return j;
}

std::vector<short>& myQuickSort(std::vector<short>& vect, size_t start, size_t finish) {
    if (start < finish) {
        size_t pivot = separateFunction(vect, start, finish);
        myQuickSort(vect, start, pivot);
        myQuickSort(vect, pivot + 1, finish);
    }
    return vect;
}

std::vector<short>& quickSort(std::vector<short>& vect) {
    myQuickSort(vect, 0, vect.size());
    return vect;
}

// Функция слияния из приложения
void myMerging(std::vector<short>& vect, size_t start, size_t finish) {
    std::vector<short> buffer;
    size_t left_index = start, right_index = start + ((finish - start) / 2);
    for (int i = 0; i < finish; i++) {
        if (i < ((finish - start) / 2)) buffer.push_back(vect.at(i));
        if (!buffer.empty()) {
            if (vect.at(right_index) <= buffer.at(0)) {
                vect.at(i) = vect.at(right_index);
                if (right_index < vect.size() - 1) right_index++;
                else vect.at(right_index) = buffer.back();
            }
            else {
                vect.at(i) = buffer.front();
                buffer.erase(buffer.begin());
            }
        }
    }
}

// НОВАЯ ФУНКЦИЯ: Многопоточная сортировка (2 потока)
std::vector<short>& parallelQuickSort(std::vector<short>& vect) {
    // 3. Переменная количества потоков (ровно 2)
    const unsigned int numThreads = 2;

    // 4. Делим вектор на части (chunk)
    size_t size = vect.size();
    size_t chunk = size / numThreads;

    // 5. Вектор потоков
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // 6. Отправляем в потоки быструю сортировку
    for (unsigned int t = 0; t < numThreads; t++) {
        size_t start = t * chunk;
        size_t finish = (t == numThreads - 1) ? size : start + chunk;

        threads.emplace_back(myQuickSort, std::ref(vect), start, finish);
    }

    // 7. Ждем окончания работы потоков
    for (auto& th : threads)
        th.join();

    // 8. Сливаем две отсортированные половины
    myMerging(vect, 0, size);

    return vect;
}

void timeTest(std::vector<short>& (*func)(std::vector<short>&), std::vector<short>& muInit, std::string message) {
    time_t start = clock(), finish = clock();
    func(muInit);
    finish = clock();
    std::cout << message << ": " << (finish - start) / 1000.0 << " секунд\n";
}

bool checkVectors(const std::vector<short>& vect1, const std::vector<short>& vect2) {
    bool same = true;

    for (size_t i = 0; i < vect1.size(); i++) {
        if (vect1.at(i) != vect2.at(i)) {
            same = false;
            std::cout << vect1.at(i) << "\t!=\t" << vect2.at(i) << std::endl;
        }
    }

    if (!same)
        std::cout << "\t\tFALSE\n";
    else
        std::cout << "\t\tSAME\n";

    return same;
}

int main() {
    system("chcp 1251");
    srand(static_cast<unsigned int>(time(0)));

    // Для теста берем 50000 элементов, чтобы увидеть разницу
    std::vector<short> vect(50000);

    myInit(vect, 0, vect.size());

    // Создаем 3 копии для трех сортировок
    std::vector<short> vect1 = vect, vect2 = vect, vect3 = vect;

    // Тест 1: Пузырек
    timeTest(bubbleSort, vect1, "Bubble sort  ");

    // Тест 2: Обычный квиксорт
    timeTest(quickSort, vect2, "Quick sort   ");

    // Тест 3: Многопоточный квиксорт (2 потока)
    timeTest(parallelQuickSort, vect3, "Parallel sort");

    // Проверка, что все три сортировки дали одинаковый результат
    std::cout << "\nПроверка результатов:\n";
    std::cout << "Bubble vs Quick:   ";
    checkVectors(vect1, vect2);
    std::cout << "Quick vs Parallel: ";
    checkVectors(vect2, vect3);

    return 0;
}