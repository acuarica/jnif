package ch.usi.inf.sape.frheap.tests;

import org.junit.Test;

public class ClassInstrumenterFieldTests {

	public static class TestCases extends BaseCases {

		public static ValueContainer static1;

		public static ValueContainer static2;

		public static class ValueContainer {

			public int instanceValue;

			public ValueContainer(int val) {
				instanceValue = val;
			}
		}

		public static class FieldContainer {

			public ValueContainer field1;

			public ValueContainer field2;

			public ValueContainer field3;

			public ValueContainer field4;
		}

		public void putField1() {
			FieldContainer obj = use(new FieldContainer());

			obj.field1 = use(new ValueContainer(1));
			obj.field2 = use(new ValueContainer(2));
			obj.field3 = use(new ValueContainer(3));
			obj.field4 = use(new ValueContainer(4));
		}

		public void putStatic1() {
			TestCases.static1 = use(new ValueContainer(5));
			TestCases.static2 = use(new ValueContainer(6));
		}

		public void putFieldException() {
			FieldContainer obj = use(new FieldContainer());

			obj.field1 = use(new ValueContainer(1));
			obj.field2 = use(new ValueContainer(2));

			((FieldContainer) null).field1 = use(new ValueContainer(5));
		}

		public void putFieldLoop() {
			for (int i = 0; i < 20; i++) {
				FieldContainer obj = use(new FieldContainer());

				obj.field1 = use(new ValueContainer(1));
				obj.field2 = use(new ValueContainer(2));

				putStatic1();

				TestCases.static1 = use(new ValueContainer(12));
			}
		}
	}

	@Test
	public void putField1() throws Throwable {
		MethodRunner.run(TestCases.class, "putField1");
	}

	@Test
	public void putStatic1() throws Throwable {
		MethodRunner.run(TestCases.class, "putStatic1");
	}

	@Test(expected = NullPointerException.class)
	public void testFieldException() throws Throwable {
		MethodRunner.run(TestCases.class, "putFieldException");
	}

	@Test
	public void testFieldLoop() throws Throwable {
		MethodRunner.run(TestCases.class, "putFieldLoop");
	}
}
