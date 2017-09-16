
public class TryCatch {

    public static int f(Object o) {
        try {
            return (Integer)o;
        } catch (ClassCastException e) {
            return -1;
        }
    }

    public static void g() throws Exception {
    }

}
