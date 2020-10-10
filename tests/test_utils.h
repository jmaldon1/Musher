#include <vector>


template<typename T>
void print_vector(const char* name, const std::vector<T>& vec)
{
    if (vec.empty()) {
        std::cout << name << " is empty." << std::endl;
        return;
    }

    std::cout << name << std::endl;
    for (const auto& element : vec)
    {
        std::cout << "  " << element << std::endl;
    }
}