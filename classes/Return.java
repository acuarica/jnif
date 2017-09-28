
public class Return {

    public static boolean m(int[] ls, int x) {
        for (int i = 0; i < ls.length; i++) {
            if (ls[i] == x) {
                return true;
            }
        }

        return false;
    }
}
