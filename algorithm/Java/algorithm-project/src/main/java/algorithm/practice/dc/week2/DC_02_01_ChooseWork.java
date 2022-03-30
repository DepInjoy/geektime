package algorithm.practice.dc.week2;

/**
 * @brief:      给定数组hard和money，长度都为N
 *              hard[i]表示i号的难度， money[i]表示i号工作的收入
 *              给定数组ability，长度都为M，ability[j]表示j号人的能力
 *              每一号工作，都可以提供无数的岗位，难度和收入都一样
 *              但是人的能力必须>=这份工作的难度，才能上班
 *              返回一个长度为M的数组ans，ans[j]表示j号人能获得的最好收入
 * */
public class DC_02_01_ChooseWork {
    public static class Job {
        public int money;
        public int hard;

        public Job(int money, int hard) {
            this.money = money;
            this.hard = hard;
        }
    }

    public static int[] getMoneys(Job[] job, int[] ability) {
        return null;
    }
}
