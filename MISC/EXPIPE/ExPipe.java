public class ExPipe {
	public static void main(String[] args) throws Exception {
		String sz;
		Pipe pipe = new Pipe();
		pipe.open(null);
		while (true) {
			sz = pipe.lineInput();
			if (sz == null) {
				Thread.sleep(1);
			} else if (sz.length() == 0) {
				break;
			} else {
				pipe.lineOutput(sz);
			}
		}
		pipe.close();
	}
}