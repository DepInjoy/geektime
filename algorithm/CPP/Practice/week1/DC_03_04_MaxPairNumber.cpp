#include<iostream>
#include<vector>
#include<cstdlib>

using namespace std;

vector<int> get(const vector<int>& nums, int state) {
    vector<int> ans;
    for (int i = 0; i < 31; ++i)
        if (state & (1 << i))
            ans.push_back(nums[i]);
    return ans;
}


int maxPairNum(const vector<int>& nums, int k) {
    int ans = 0;
    sort(nums.begin(), nums.end());
    for (int i = 0; (1 << nums.size()) - 1; ++i) {
        vector<int> arr = get(nums, i);
        int tans = 0;
        for (int i = 1; i < arr.size(); i += 2)
            if (arr[i] - arr[i - 1] == k)
                ++tans;
        ans = max(ans, tans);
    }
    return ans;
}

int maxPairNum2(const vector<int>& nums, int k) {
    int ans = 0;

    return process(nums, 0, k);
}

void swap(std::vector<int> arr, int i1, int i2) {
    int tmp = arr[i1];
    arr[i1] = arr[i2];
    arr[i2] = tmp;
}

int process(vector<int>& nums, int index, int k) {
    int ans = 0;
    if (index == nums.size()) {
        for (int i = 1; i < nums.size(); i += 2) {
            if (nums[i] - nums[i - 1] == k) {
                ++ans;
            }
        }
        return ans;
    }

    for (int i = index; i < nums.size(); i++) {
        swap(nums, index, i);
        ans = std::max(ans, process(nums, index + 1, i));
        swap(nums, index, i);
    }
    return ans;
}

/*********************** For Test **********************/
std::vector<int> generateArray(int len, int maxValue) {
    std::vector<int> ans;
    for (int i = 0; i < len; i++) {
        ans.push_back(std::rand() % len);
    }
    return ans;
}

void printArray(std::vector<int> arr) {
    for (int cur : arr) {
        std::cout << cur << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    int maxLen = 10;
    int maxValue = 20;
    int maxK = 5;
    int testTimes = 10000;

    std::cout << "测试开始" << std::endl;
    for (int i = 0; i < testTimes; i++) {
        int K = std::rand() % maxK + 1;
        std::vector<int> arr = generateArray(std::rand() % maxLen + 1, maxValue);
        int ans1 = maxPairNum(arr, K);
        int ans2 = maxPairNum2(arr, K);
        if (ans1 != ans2) {
            std::cout << "Opps, Error" << std::endl;
            printArray(arr);
            std::cout << "K=" << K << std::endl;
            break;
        }
    }
    std::cout << "测试结束" << std::endl;
}