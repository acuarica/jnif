package frheapagent;

/**
 * These samples were taken from Chapter 3 of the Java Virtual Machine
 * Specification.
 * 
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-3.html
 * 
 * @author luigi
 * 
 */
public class Chapter3Snippets {

	// 3.2. Use of Constants, Local Variables, and Control Constructs

	void spin() {
		int i;
		for (i = 0; i < 100; i++) {
			; // Loop body is empty
		}
	}

	void dspin() {
		double i;
		for (i = 0.0; i < 100.0; i++) {
			; // Loop body is empty
		}
	}

	double doubleLocals(double d1, double d2) {
		return d1 + d2;
	}

	void sspin() {
		short i;
		for (i = 0; i < 100; i++) {
			; // Loop body is empty
		}
	}

	// 3.3. Arithmetic

	int align2grain(int i, int grain) {
		return ((i + grain - 1) & ~(grain - 1));
	}

	// 3.4. Accessing the Run-Time Constant Pool

	void useManyNumeric() {
		int i = 100;
		int j = 1000000;
		long l1 = 1;
		long l2 = 0xffffffff;
		double d = 2.2;

		// ...do some calculations...

		if (d == 0) {
			l1 = 2;
		}
	}

	// 3.5. More Control Examples

	void whileInt() {
		int i = 0;
		while (i < 100) {
			i++;
		}
	}

	void whileDouble() {
		double i = 0.0;
		while (i < 100.1) {
			i++;
		}
	}

	int lessThan100(double d) {
		if (d < 100.0) {
			return 1;
		} else {
			return -1;
		}
	}

	int greaterThan100(double d) {
		if (d > 100.0) {
			return 1;
		} else {
			return -1;
		}
	}

	// 3.6. Receiving Arguments

	int addTwo(int i, int j) {
		if (i == 0 && j == 0) {
			return 0;
		}

		return i + j;
	}

	static int addTwoStatic(int i, int j) {
		if (i == 0 && j == 0) {
			return 0;
		}

		return i + j;
	}

	// 3.7. Invoking Methods

	int add12and13a() {
		int res = addTwo(12, 13);

		if (res == 0) {
			return 0;
		}

		return res;
	}

	int add12and13b() {
		int res = addTwoStatic(12, 13);

		if (res == 0) {
			return 0;
		}

		return res;
	}

	public class Near {
		int it;

		public int getItNear() {
			return getIt();
		}

		private int getIt() {
			return it;
		}
	}

	public class Far extends Near {
		int getItFar() {
			int res = super.getItNear();

			if (res == 0) {
				return 0;
			}

			return res;
		}
	}

	// 3.8. Working with Class Instances

	Object create() {
		Object res = new Object();

		if (res == null) {
			return null;
		}

		return res;
	}

	public static class MyObj {

	}

	public static class Example {
		int i; // An instance variable

		MyObj example() {
			MyObj o = new MyObj();
			return silly(o);
		}

		MyObj silly(MyObj o) {
			if (o != null) {
				return o;
			} else {
				return o;
			}
		}

		void setIt(int value) {
			i = value;
		}

		int getIt() {
			return i;
		}
	}

	// 3.9 Arrays

	void createBuffer() {
		int buffer[];
		int bufsz = 100;
		int value = 12;
		buffer = new int[bufsz];
		buffer[10] = value;
		value = buffer[11];

		if (value == 1) {
			value = 2;
		}
	}

	void createThreadArray() {
		Thread threads[];
		int count = 10;
		threads = new Thread[count];
		threads[0] = new Thread();

		if (threads[0] == null) {
			threads[0] = new Thread();
		}
	}

	int[][][] create3DArray() {
		int grid[][][];
		grid = new int[10][5][];

		if (grid == null) {
			return null;
		}

		return grid;
	}

	// 3.10. Compiling Switches

	int chooseNear(int i) {
		switch (i) {
		case 0:
			return 0;
		case 1:
			return 1;
		case 2:
			return 2;
		default:
			return -1;
		}
	}

	int chooseFar(int i) {
		switch (i) {
		case -100:
			return -1;
		case 0:
			return 0;
		case 100:
			return 1;
		default:
			return -1;
		}
	}

	// 3.11. Operations on the Operand Stack

	public long nextIndex() {
		return index++;
	}

	private long index = 0;

	// 3.12. Throwing and Handling Exceptions

	public static class TestExc extends Exception {
	}

	void cantBeZero(int i) throws TestExc {
		if (i == 0) {
			throw new TestExc();
		}
	}

	public static class TestExc1 extends Exception {
	}

	public static class TestExc2 extends Exception {
	}

	public static void tryItOut() throws TestExc, TestExc1, TestExc2 {
	}

	public static void handleExc(Exception e) {
	}

	void catchOne() throws TestExc1, TestExc2 {
		try {
			tryItOut();
		} catch (TestExc e) {
			handleExc(e);
		}
	}

	// void nestedCatch() {
	// try {
	// try {
	// tryItOut();
	// } catch (TestExc1 e) {
	// handleExc1(e);
	// }
	// } catch (TestExc2 e) {
	// handleExc2(e);
	// }
	// }

	// 3.13. Compiling finally

	//
	// void tryFinally() {
	// try {
	// tryItOut();
	// } finally {
	// wrapItUp();
	// }
	// }
	//
	// void tryCatchFinally() {
	// try {
	// tryItOut();
	// } catch (TestExc e) {
	// handleExc(e);
	// } finally {
	// wrapItUp();
	// }
	// }
	//
	// //3.14. Synchronization
	//
	//
	// void onlyMe(Foo f) {
	// synchronized(f) {
	// doSomething();
	// }
	// }
}
