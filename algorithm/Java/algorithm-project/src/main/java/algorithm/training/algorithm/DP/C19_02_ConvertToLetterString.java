package algorithm.training.algorithm.DP;
/**
 * @brief:                      从左往右的尝试模型
 *
 *              规定1和A对应、2和B对应、3和C对应..., 那么一个数字字符串比如"111”就可以转化为:
 *              "AAA"、"KA"和"AK"
 *              给定一个只有数字字符组成的字符串str，返回有多少种转化结果
 * */
public class C19_02_ConvertToLetterString {
    /**
     * @brief:      从左到右地暴力尝试,相关实现tryNumber和tryNumber2
     *              通过process函数判断当前字符以及加入接下来的字符是否符合转化格式来决定是否接受该分支
     * */
    public static int tryNumber(String str) {
        if (str == null || str.length() == 0) return 0;

        return process(str.toCharArray(), 0);
    }

    public static int process(char[] str, Integer index) {
        if (index == str.length) return 1;
        if (str[index] == '0') return 0; // 无效分支

        Integer num = process(str, index + 1);
        if (index + 1 < str.length && (str[index] - '0') * 10 + (str[index + 1] - '0') < 27) {
            num += process(str, index + 2);
        }
        return num;
    }

    public static int tryNumber2(String str) {
        if (str == null || str.length() == 0) return 0;

        return process(str.toCharArray(), 0);
    }

    public static int process2(char[] str, Integer index) {
        if (index == str.length) return 1;

        Integer num = process(str, index + 1);
        if (index + 1 < str.length && str[index] == '1' && (str[index + 1] >= '0' && str[index + 1] <= '9')) {
            num += process(str, index + 2);
        }

        if (index + 1 < str.length && str[index] == '2' && (str[index + 1] >= '0' && str[index + 1] <= '7')) {
            num += process(str, index + 2);
        }
        return num;
    }

    public static void main(String[] args) {
        String in0 = new String("");
        assert (tryNumber(in0) == 0);
        assert (tryNumber2(in0) == 0);

        String in1 = new String("111");
        assert (tryNumber(in1) == 3);
        assert (tryNumber2(in1) == 3);

        String in2 = new String("1137");
        assert (tryNumber(in2) == 3);
        assert (tryNumber2(in2) == 3);

        String in3 = new String("1103");
        assert (tryNumber(in3) == 1);
        assert (tryNumber2(in3) == 1);
    }
}
