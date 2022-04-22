package algorithm.practice.dc.week3;

import javax.swing.*;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * @brief       自由之路
 *              https://leetcode.com/problems/freedom-trail/
 */

public class L03_07_FreedomTrail {
    public int findRotateSteps(String ring, String key) {
        if (ring == null || ring.length() == 0 || key == null || key.length() == 0) return 0;

        HashMap<Character, ArrayList<Integer>> dialmap = new HashMap<>();
        char[] ringChs = ring.toCharArray();
        for (int i = 0; i < ringChs.length; i++) {
            if (!dialmap.containsKey(ringChs[i])) {
                dialmap.put(ringChs[i], new ArrayList<>(1));
            }
            dialmap.get(ringChs[i]).add(i);
        }

        char[] keyChs = key.toCharArray();
        int N = ring.length(), M = keyChs.length;
        int [][] dp = new int[N][M+1];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j <= M; j++) {
                dp[i][j] = -1;
            }
        }
        return process(dialmap, 0, 0, keyChs, ring.length(), dp);
    }

    public int process(HashMap<Character, ArrayList<Integer>> dialmap,
                       int preButton, int index, char[] keyChs, int N, int[][] dp) {
        if (dp[preButton][index] != -1) return dp[preButton][index];

        int ans = Integer.MAX_VALUE;
        if (index == keyChs.length) {
            ans = 0;
        } else {
            for (Integer next : dialmap.get(keyChs[index])) {
                ans = Math.min(ans, dialLength(preButton, next, N) + 1 +
                        process(dialmap, next, index + 1, keyChs, N, dp));
            }
        }
        dp[preButton][index] = ans;
        return ans;
    }

    public int dialLength(int from, int to, int size) {
        return Math.min(Math.abs(from - to), Math.abs(Math.min(from, to) + size -
                Math.max(from, to)));
    }
}
