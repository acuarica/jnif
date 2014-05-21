package ch.usi.inf.sape.frheap.tests;

import org.junit.Test;

public class ClassInstrumenterMultiANewArrayTests {

	public static class TestCases extends BaseCases {

		public void multi1() {
			int[] imulti = new int[3];
			use(imulti);

			long[] jmulti = new long[7];
			use(jmulti);
		}

		public void multi2() {
			int[][] imulti = new int[3][5];
			use(imulti);

			long[][] jmulti = new long[7][9];
			use(jmulti);
		}

		public void multi3() {
			int[][][] imulti = new int[3][11][3];
			use(imulti);

			long[][][] jmulti = new long[7][9][5];
			use(jmulti);
		}

		public void multi4() {
			int[][][][] imulti = new int[3][11][4][2];
			use(imulti);

			Object[][][][] rmulti = new Object[2][7][9][5];
			use(rmulti);
		}

		public void allMulti() {
			for (int i = 0; i < 10; i++) {
				multi1();
				multi2();
				multi3();
				multi4();

				short[] smulti = new short[i];
				use(smulti);

				double[] dmulti = new double[i + 1];
				use(dmulti);

				Object[] rmulti = new Object[i + 2];
				use(rmulti);
			}
		}
	}

	@Test
	public void multi1Test() throws Throwable {
		MethodRunner.run(TestCases.class, "multi1");
	}

	@Test
	public void multi2Test() throws Throwable {
		MethodRunner.run(TestCases.class, "multi2");
	}

	@Test
	public void multi3Test() throws Throwable {
		MethodRunner.run(TestCases.class, "multi3");
	}

	@Test
	public void multi4Test() throws Throwable {
		MethodRunner.run(TestCases.class, "multi4");
	}

	@Test
	public void allMultiTest() throws Throwable {
		MethodRunner.run(TestCases.class, "allMulti");
	}
}
