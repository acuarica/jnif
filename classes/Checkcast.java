
public class Checkcast {

    public static int m(Object arg) {
        if (arg instanceof String) {
            String s = (String)arg;
            return s.length();
        }

        return -1;
    }

}
