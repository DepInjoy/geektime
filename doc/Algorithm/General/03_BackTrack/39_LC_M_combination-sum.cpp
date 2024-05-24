/**
 * 39. 总组合数
 * https://leetcode.cn/problems/combination-sum/
 * 
 * 实现方式
 *  1. 回溯
*/

void backtrace(std::vector<std::vector<int>>& contaier, vector<int>& candidates,
            std::vector<int>& output, int pos, int len, int target) {
    if (target == 0) {
        contaier.push_back(output);
        return;
    }

    if (pos == len) return; 
    backtrace(contaier, candidates, output, pos+1, len, target);

    if (target - candidates[pos] >= 0) {
        output.push_back(candidates[pos]);
        backtrace(contaier, candidates, output, pos, len, target - candidates[pos]);
        output.pop_back();
    }
}

vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
    std::vector<std::vector<int>> contaier;
    std::vector<int> output;
    backtrace(contaier, candidates, output, 0, candidates.size(), target);
    return contaier;
}
