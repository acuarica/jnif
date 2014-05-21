package ch.usi.inf.sape.frheap.tests;

import java.lang.reflect.Array;

import org.junit.Test;

public class ClassInstrumenterArrayTests {

	public static class TestCases extends BaseCases {

		public DummyClass d;
		public static DummyClass sd;

		public DummyClass[] da;
		public static DummyClass[] sda;

		public static class DummyClass {
			public int field;

			public DummyClass(int field) {
				this.field = field;
			}

			@Override
			public String toString() {
				return "field=" + field + ", " + super.toString();
			}
		}

		private static final int COUNT = 5;

		public void primitive() {
			use(new boolean[COUNT]);
			use(new byte[COUNT]);
			use(new char[COUNT]);
			use(new short[COUNT]);
			use(new int[COUNT]);
			use(new long[COUNT]);
			use(new float[COUNT]);
			use(new double[COUNT]);
		}

		public void boxedPrimitive() {
			use(new Boolean[COUNT]);
			use(new Byte[COUNT]);
			use(new Character[COUNT]);
			use(new Short[COUNT]);
			use(new Integer[COUNT]);
			use(new Long[COUNT]);
			use(new Float[COUNT]);
			use(new Double[COUNT]);
		}

		public void obj() {
			for (int i = 0; i < 10; i++) {
				use(new Object[COUNT]);
			}
		}

		public void dummy() {
			for (int i = 0; i < 10; i++) {
				use(new DummyClass[COUNT]);
			}
		}

		public void reflectionPrimitive() throws ClassNotFoundException {
			Class<?> c = Class.forName(Integer.class.getName());
			Object o = Array.newInstance(c, COUNT * 16);
			use(o);
		}

		public void reflectionObject() throws ClassNotFoundException {
			Class<?> c = Class.forName(Integer.class.getName());
			Object o = Array.newInstance(c, COUNT * 16);
			use(o);
		}

		public void all() throws ClassNotFoundException {
			primitive();
			boxedPrimitive();
			obj();
			reflectionPrimitive();
			reflectionObject();
		}

		public void loop() throws ClassNotFoundException {
			for (int i = 0; i < 2; i++) {
				primitive();
				boxedPrimitive();
				obj();
				reflectionPrimitive();
				reflectionObject();
			}
		}

		public void aastoreMethod() {
			Object[] arr = use(new Object[11]);
			for (int i = 0; i < arr.length; i++) {
				arr[i] = new Object();
			}
		}

		public void aastoreMethod2() {
			DummyClass[] arr = use(new DummyClass[22]);
			for (int i = 0; i < arr.length; i++) {
				arr[i] = new DummyClass(i);
			}

			int s = 0;
			for (int i = 0; i < arr.length; i++) {
				s += arr[i].field;
			}

			System.out.println("Total sum: " + s);
		}

		public void aastoreMethod3() {
			Object[] arr = new Object[5];

			Object a = new Object();
			Object b = new Object();

			arr[2] = a;
			arr[2] = b;
		}

		public void fields() {
			d = use(new DummyClass(1));
			sd = use(new DummyClass(2));

			da = use(new DummyClass[5]);
			sda = use(new DummyClass[7]);
		}
	}

	@Test
	public void primitive() throws Throwable {
		MethodRunner.run(TestCases.class, "primitive");
	}

	@Test
	public void boxedPrimitive() throws Throwable {
		MethodRunner.run(TestCases.class, "boxedPrimitive");
	}

	@Test
	public void obj() throws Throwable {
		MethodRunner.run(TestCases.class, "obj");
	}

	@Test
	public void dummy() throws Throwable {
		MethodRunner.run(TestCases.class, "dummy");
	}

	@Test
	public void reflectionPrimitive() throws Throwable {
		MethodRunner.run(TestCases.class, "reflectionPrimitive");
	}

	@Test
	public void reflectionObject() throws Throwable {
		MethodRunner.run(TestCases.class, "reflectionObject");
	}

	@Test
	public void all() throws Throwable {
		MethodRunner.run(TestCases.class, "all");
	}

	@Test
	public void loop() throws Throwable {
		MethodRunner.run(TestCases.class, "loop");
	}

	@Test
	public void aastoreTest() throws Throwable {
		MethodRunner.run(TestCases.class, "aastoreMethod");
	}

	@Test
	public void aastore2Test() throws Throwable {
		MethodRunner.run(TestCases.class, "aastoreMethod2");
	}

	@Test
	public void aastore3Test() throws Throwable {
		MethodRunner.run(TestCases.class, "aastoreMethod3");
	}

	@Test
	public void fieldsTest() throws Throwable {
		MethodRunner.run(TestCases.class, "fields");
	}
}
