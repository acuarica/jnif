
public class WhileIinc {

    public static void m(int n) {
        int i = 0;
        int sum = 0;
        int prod = 1;
        while (i < n) {
            sum += i;
            prod *= i;
            i++;
        }

        System.out.println(sum);
        System.out.println(prod);
    }

}
