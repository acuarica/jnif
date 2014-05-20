package ch.usi.inf.sape.frheap.tests;

import org.junit.Test;

public class ClassInstrumenterSimpleTests {

	public static class TestCases extends BaseCases {

		public static class DummyClass {
		}

		public static class SomeClass {
			public DummyClass value32;
		}

		public SomeClass instanceFieldOfSomeClass;

		public static SomeClass staticFieldOfSomeClass;

		public SomeClass getSome() {
			return instanceFieldOfSomeClass;
		}

		public void setSome(SomeClass val) {
			instanceFieldOfSomeClass = val;
		}

		private static void str(String s) {
			s.toString();
		}

		public static void usestr() {
			str("using string");
		}

		public static void doit() {
			System.out.format("From instrumentation\n");
			System.out.format("From instrumentation\n");
			System.out.format("From instrumentation\n");
		}

		public void start() {
			TestCases p = new TestCases();
			SomeClass s = new SomeClass();

			p.instanceFieldOfSomeClass = s;

			staticFieldOfSomeClass = new SomeClass();

			System.out.format("something returned %d\n", 1234);
		}

		public void reflect() throws InstantiationException, IllegalAccessException {
			TestCases tc = TestCases.class.newInstance();
			tc.instanceFieldOfSomeClass = new SomeClass();
		}
	}

	@Test
	public void testSimple() throws Throwable {
		MethodRunner.run(TestCases.class, "start");
	}

	@Test
	public void reflectTest() throws Throwable {
		MethodRunner.run(TestCases.class, "reflect");
	}
}
