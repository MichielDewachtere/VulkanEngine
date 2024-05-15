#ifndef GAMEUTILS_H
#define GAMEUTILS_H

#include <vector>
#include <functional>

namespace std
{
    template <>
    struct hash<std::pair<EBlockType, EDirection>>
	{
        size_t operator()(const std::pair<EBlockType, EDirection>& p) const
    	{
            const size_t hashBlock = std::hash<EBlockType>{}(p.first);
            const size_t hashDirection = std::hash<EDirection>{}(p.second);
            return hashBlock ^ (hashDirection << 1);
        }
    };
}

template<typename T>
concept vec_type = requires {
    typename T::length_type;
    typename T::value_type;
};

template<vec_type VecType, int N>
VecType IntVectorToVec(const std::vector<int>& vec)
{
    VecType result{};

    if (vec.size() < N)
        return result;

    for (int i = 0; i < N; ++i) 
    {
        result[i] = static_cast<float>(vec[i]);
    }

    return result;
}

#endif // GAMEUTILS_H