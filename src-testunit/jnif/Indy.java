package jnif;

import java.util.*;
import java.util.stream.*;

public class Indy {

	public void getStreams() {
		List<String> names = Arrays.asList("1", "2", "3");
		Stream lengths = names.stream().map(name -> name.length());
		
		System.out.println(lengths);
	}
}
