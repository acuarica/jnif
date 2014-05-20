package ch.usi.inf.sape.frheap.tests;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Vector;

import org.junit.Test;

public class ClassInstrInnerTests {

	public static class TestCases extends BaseCases {
		public static class OutputFile {
			public int data;
		}

		HashMap<String, OutputFile> outputFiles = new LinkedHashMap<String, OutputFile>(20);
		final String name;
		final String[] args;

		public TestCases() {
			String name = "asdf";
			Vector<String> args = new Vector<String>();

			this.args = (String[]) args.toArray(new String[0]);
			this.name = name;
		}

		public class Size {
			final String name;
			final String[] args;

			HashMap<String, OutputFile> outputFiles = new LinkedHashMap<String, OutputFile>(20);

			Size(String name, Vector<String> args) {
				this.args = (String[]) args.toArray(new String[0]);
				this.name = name;
			}
		}

		public void pass() {
		}
	}

	@Test
	public void pass() throws Throwable {
		MethodRunner.run(TestCases.class, "pass");
	}

}
