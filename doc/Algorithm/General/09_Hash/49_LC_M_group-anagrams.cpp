/**
 * 49. 字母异位词分组
 * https://leetcode.cn/problems/group-anagrams/description
 * 
 * 实现方式：
 *  1. 排序+Hash表
 *  2. 计数+Hash表
 */
#include <vector>
using std::vector;


// 排序+Hash表
vector<vector<string>> groupAnagrams(vector<string>& strs) {
    std::unordered_map<std::string, std::vector<std::string>> ump;
    for (std::string& str : strs) {
        std::string key = str;
        std::sort(key.begin(), key.end());
        ump[key].emplace_back(str);
    }

    std::vector<std::vector<std::string>> ans;
    ans.reserve(ump.size());
    for (auto& item : ump) {
        ans.emplace_back(item.second);
    }
    return ans;
}

// 计数+Hash表
vector<vector<string>> groupAnagrams(vector<string>& strs) {
    auto arrayHash = [fn = std::hash<int>{}] (const std::array<int, 26>& arr) -> std::size_t {
        return  std::accumulate(arr.begin(), arr.end(), 0u, [&](std::size_t acc, int num) {
            return (acc << 1) ^ fn(num);
        });
    };

    std::unordered_map<std::array<int, 26>, std::vector<std::string>,
            decltype(arrayHash)> ump(0, arrayHash);
    for (std::string& str : strs) {
        std::array<int, 26> cnts{};
        for (char& ch : str) {
            ++cnts[ch - 'a'];
        }
        ump[cnts].emplace_back(str);
    }

    std::vector<std::vector<std::string>> ans;
    ans.reserve(ump.size());
    for (auto& item : ump) {
        ans.emplace_back(item.second);
    }
    return ans;
}