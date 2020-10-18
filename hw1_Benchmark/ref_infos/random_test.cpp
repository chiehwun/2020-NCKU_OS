#include <iostream>
#include <random> // 亂數函式庫
#include <ctime>
#include <climits>

void random_tr();
void random_dev();

int main()
{
    random_tr();
    return 0;
}

void random_tr()
{
    /******** 簡單版本-與傳統版本一樣差 ********/
    // 亂數產生器
    std::default_random_engine generator(time(NULL));

    // 亂數的機率分布
    std::uniform_int_distribution<int> unif(INT_MIN, INT_MAX);

    // 產生亂數
    for (int i = 0; i < 10; ++i)
    {
        int x = unif(generator);
        std::cout << "x = " << x << std::endl;
    }

    std::cout << unif.max() << std::endl
              << unif.min() << std::endl;
}

void random_dev()
{
    /******** 隨機設備種子(不適合大量產生，因熵會下降) ********/
    // 隨機設備
    std::random_device rd;

    // 隨機亂數的範圍
    std::cout << "Min = " << rd.min()
              << ", Max = " << rd.max() << std::endl;

    // 產生隨機的亂數
    std::cout << "Random Number = " << rd() << std::endl;

    // 隨機設備的熵值
    std::cout << "Entropy = " << rd.entropy() << std::endl;
}