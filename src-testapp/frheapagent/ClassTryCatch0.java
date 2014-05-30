package frheapagent;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.*;
import java.util.regex.*;

public class ClassTryCatch0 {

	static ClassTryCatch0 test() throws RuntimeException {
		ClassTryCatch0 template = new ClassTryCatch0();
		ClassTryCatch0 result = null;

		try {
			ClassTryCatch0 encodedKey = new ClassTryCatch0();
			if (encodedKey == null) {
				throw new RuntimeException();
			}

			result = template;
			result = null;
		} catch (NullPointerException e) {
			result = null;
		}

		return result;
	}

	private static class Inner {
		public Inner(int arg) {

		}
	}

	void uninit0(int arg) {
		Integer i = new Integer(arg);
		arg =+ i;
		new Inner(arg == 0 ? 1 : 2);
	}

	void uninit1(int cipherMode) {
		// new String((cipherMode == 0 ? "CTR" : "CTS") + " mode" + arg);
	}
}
