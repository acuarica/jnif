package frheapagent;

public class Self extends Base {

	public static Base get0() {
		Base self = new Self();

		if (self == null) {
			self = new Self();
		}

		return self;
	}

	public static Base get1() {
		Base self = new Base();

		if (self == null) {
			self = new Self();
		}

		return self;
	}
}
