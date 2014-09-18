package frheapagent;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.*;
import java.util.regex.*;
import frheapagent.j8.*;
import java.lang.invoke.*;

public class DemoTest {

	public DemoTest(DemoTest t) {

	}

	public static void main(String[] args) throws IOException {
		if (args.length > 0) {
			System.out.println("zero");
		}

		DemoTest dt = new DemoTest(new DemoTest(null));
		System.out.println("main");

	}

}
