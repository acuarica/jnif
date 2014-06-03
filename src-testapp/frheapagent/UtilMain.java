package frheapagent;

import java.util.*;
import java.io.*;
import java.sql.*;

public class UtilMain {

	void use(boolean f) {
	}

	void runScriptGuts() {
		// int n = 0;
		String command = null;

		while (command == null) {
			try {
				use(command != null);
			} catch (Throwable t) {
			}

			command = "";
		}
	}
}
