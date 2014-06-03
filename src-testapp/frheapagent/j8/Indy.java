package frheapagent.j8;

import java.util.*;

//import java.util.stream.*;

public class Indy {

	public void getStreams() {
		// List<String> names = Arrays.asList("1", "2", "3");
		// Stream lengths = names.stream().map(name -> name.length());
		//
		// System.out.println(lengths);
	}

	public void set(Indy indy, boolean flag) {

	}

	public void doPrompt(boolean f, Object t) {

	}

	public boolean empty() {
		return false;
	}

	public Object getConnection() {
		return null;
	}

	public boolean getElapsedTimeState(){
		return false;
	}
	public Indy[] indies;
	public boolean exit;

	public void testJoin(int arg) {
		int scriptErrorCount = 0;
		boolean done = false;
		String command = null;

		while (arg >= 0) {
			try {
				set(indies[arg], (arg > 1));
			} catch (Throwable t) {
			}
			arg--;
		}

		System.out.println("testJoin");
	}

	public void getStreams2() {
		// List<String> names = Arrays.asList("1", "2", "3", "4");
		// Stream lengths = names.stream().map(name -> name.length());
		//
		// System.out.println(lengths);
	}
}
