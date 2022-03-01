package algorithm.training.algorithm.DP;

import java.util.Stack;

/**
 * @brief:      仰望好的尝试
 *              给你一个栈，实现逆序这个栈，不能申请额外的数据结构，只能使用递归函数
 * */
public class D17_05_ReverseStackUsingRecursive {
    public static void reverse(Stack<Integer> stack) {
        if (stack.isEmpty()) return;

        int last = process(stack);
        reverse(stack);
        stack.push(last);
    }

    public static int process(Stack<Integer> stack) {
        int ans = stack.pop();
        if (stack.isEmpty()) {
            return ans;
        } else {
            int last = process(stack);
            stack.push(ans);
            return last;
        }
    }

    public static void main(String[] args) {
        Stack<Integer> test = new Stack<Integer>();
        test.push(1);
        test.push(2);
        test.push(3);
        test.push(4);
        test.push(5);
        reverse(test);
        while (!test.isEmpty()) {
            System.out.println(test.pop());
        }
    }
}
