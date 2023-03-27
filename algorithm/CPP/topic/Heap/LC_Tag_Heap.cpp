/**
 *  506. 相对名次(Easy)
 *      给你一个长度为 n 的整数数组 score ，其中 score[i] 是第 i 位运动员在比赛中的得分。所有得分都 互不相同 。
 *      运动员将根据得分 决定名次 ，其中名次第 1 的运动员得分最高，名次第 2 的运动员得分第 2 高，依此类推。运动员的名次决定了他们的获奖情况：
 *          名次第 1 的运动员获金牌 "Gold Medal" 。
 *          名次第 2 的运动员获银牌 "Silver Medal" 。
 *          名次第 3 的运动员获铜牌 "Bronze Medal" 。
 *          从名次第 4 到第 n 的运动员，只能获得他们的名次编号（即，名次第 x 的运动员获得编号 "x"）。
 *      使用长度为 n 的数组 answer 返回获奖，其中 answer[i] 是第 i 位运动员的获奖情况。
 *      
 *      https://leetcode.cn/problems/relative-ranks/description/
*/
#include <vector>
#include <string>
using namespace std;
// 时间复杂度O(NlogN), 空间复杂度为O(N)
vector<string> findRelativeRanks(vector<int>& score) {
    int n = score.size();
    std::vector<std::string> desc {
        "Gold Medal", "Silver Medal", "Bronze Medal"
    };
    std::vector<std::pair<int, int>> arr(n);
    std::vector<std::string> ans(score.size());
    for (int i = 0; i < n; ++i) {
        arr[i] = std::make_pair<>(-score[i], i);
    }
    std::sort(arr.begin(), arr.end());

    for (int i = 0; i < n; ++i) {
        ans[arr[i].second] = (i < 3) ? desc[i] : std::to_string(i+1);     
    }
    return ans;
}

// 堆实现
vector<string> findRelativeRanks(vector<int>& score) {
    int n = score.size();
    std::priority_queue<int, std::vector<int>> prio_q(score.begin(), score.end());
    std::unordered_map<int, int> score_index_map(n);
    std::vector<std::string> medal {
        "Gold Medal", "Silver Medal", "Bronze Medal"
    };
    std::vector<std::string> ans(score.size());
    for (int i = 0; i < n; ++i) {
        score_index_map.insert(std::make_pair<>(score[i], i));
    }

    for (int i = 0; i < n; ++i) {
        int pos = score_index_map.find(prio_q.top())->second;
        ans[pos] = (i < 3) ? medal[i] : std::to_string(i+1);
        prio_q.pop();
    }
    return ans;
}

