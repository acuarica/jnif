
public class Goto {

    public static int m(int n) {
        int i = 0;
        while (true) {
            if (i == n) {
                return n;
            }
            i++;
        }
    }
}
