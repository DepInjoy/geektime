package algorithm.practice.dc.week2;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.TreeMap;

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

    public static class JobComparator implements Comparator<Job> {
        public int compare(Job o1, Job o2) {
            return o1.hard == o2.hard ? o1.money - o2.money : o1.hard - o2.hard;
        }
    }

    public static int[] getMoneys(Job[] job, int[] ability) {
        if (job == null || ability == null || job.length == 0) return null;

        // 将job按照hard升序，money降序的方式进行排序
        Arrays.sort(job, new JobComparator());
        TreeMap<Integer, Integer> orderedMap = new TreeMap<>();
        // 构造以hard为key，money为value，对于相同难度只保留高报酬的有序表
        orderedMap.put(job[0].hard, job[0].money);
        Job pre = job[0];
        for (int i = 1; i < job.length; i++) {
            if (pre.hard != job[i].hard && pre.money < job[i].money) {
                orderedMap.put(job[i].hard, job[i].money);
                pre = job[i];
            }
        }

        int[] ans = new int[ability.length];
        for (int i = 0; i < ability.length; i++) {
            Integer floorKey = orderedMap.floorKey(ability[i]);
            ans[i] = (floorKey != null ? orderedMap.get(floorKey) : 0);
        }
        return ans;
    }
}
