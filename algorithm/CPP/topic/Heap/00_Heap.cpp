/**
 *  744: 寻找比目标字母大的最小字母(Easy)
*/
char nextGreatestLetter(vector<char>& letters, char target) {
    std::priority_queue<char, std::greater<char>> max_heap;
    for (char letter : letters) {
        if (letter > target) {
            max_heap.push(letter);
        }
    }
    return max_heap.empty()? letters[0] : max_heap.top();
}