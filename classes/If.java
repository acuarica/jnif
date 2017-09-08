
public class If {

    public static String m1(boolean arg) {
        String s = "Hola";
        String t = "Ciao";
        String res;
        if (arg) {
            res = s;
        } else {
            res = t;
        }
        return res;
    }

    public static String m2(boolean arg) {
        String s = "Hola";
        String t = "Ciao";
        String res = "";
        if (arg) {
        } else {
            res = t;
        }
        return res;
    }

}
