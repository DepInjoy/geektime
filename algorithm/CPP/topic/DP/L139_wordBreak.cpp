/**
 * @brief   139. 单词拆分
 *          给你一个字符串 s 和一个字符串列表 wordDict 作为字典
 *          请你判断是否可以利用字典中出现的单词拼接出 s 。
 *          注意：不要求字典中出现的单词全部都使用，并且字典中的单词可以重复使用。
 *          链接：https://leetcode-cn.com/problems/word-break
 * 
 *          实现思想：
 *              1. 动态规划
 *                  dp[i] 表示字符串s前i个字符组成的字符串
 *                  s[0..i-1]是否能被空格拆分成若干个字典中出现的单词
 *                  dp[i]=dp[j] && check(s[j..i−1])
 *                      其中，check(s[j..i−1])表示子串s[j..i-1]是否出现在字典中
 *              2. Trie
 * 
 */

bool wordBreak(string s, vector<string>& wordDict) {
    unordered_set<string> hashset;
    for (auto word : wordDict) {
        hashset.insert(word);
    }

    vector<bool> dp = vector<bool>(s.size()+1, false);
    dp[0] = true;
    for (int i = 0; i <= s.size(); i++) {
        for (int j = 0; j <= i; j++) {
            if (dp[j] && hashset.find(s.substr(j, i-j)) != hashset.end()) {
                dp[i] = true;
                break;
            }
        }
    }
    return dp[s.size()];
}
