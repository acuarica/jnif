package ch.usi.inf.sape.frheap.server;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;

import org.apache.log4j.Logger;

import ch.usi.inf.sape.frheap.FrHeapInstrumenter;

public class FrHeapInstrumentWorker extends Thread {

	private final static Logger logger = Logger
			.getLogger(FrHeapInstrumentWorker.class);

	private FrHeapInstrumentSocket _socket;

	private FrHeapInstrumenter _instr;

	private PrintWriter _writer;

	private String _p;

	public FrHeapInstrumentWorker(FrHeapInstrumentSocket socket,
			FrHeapInstrumenter instr, int tid, String p)
			throws FileNotFoundException, UnsupportedEncodingException {
		_socket = socket;
		_instr = instr;

		_p = p;
		_writer = new PrintWriter("build/eval-server-" + p + "-" + tid
				+ ".prof");
	}

	public void run() {
		logger.debug(String.format("Running worker id %d '%s'", this.getId(),
				getName()));

		try {
			while (true) {
				FrHeapInstrumentMessage request = _socket.read();

				if (request == null) {
					logger.debug(String.format("End of stream. Worker done."));
					return;
				}

				if (request.className.length == 0
						&& request.classBytes.length == 0) {
					logger.debug(String
							.format("Received exit thread message. Exiting..."));

					break;
				}

				byte[] instrClass;

				instrClass = instrumentClass(new String(request.className),
						request.classBytes);

				FrHeapInstrumentMessage response = new FrHeapInstrumentMessage();
				response.className = request.className;
				response.classBytes = instrClass;

				_socket.write(response);
			}

			logger.debug(String.format("Worker done"));
		} catch (IOException e) {

			logger.error("Error on worker. Exiting...", e);

			throw new RuntimeException("Error on worker.", e);
		} finally {
			// _writer.flush();
			_writer.close();
		}
	}

	private byte[] instrumentClass(String className, byte[] classBytes)
			throws IOException {
		byte[] res;
		long instrTime;

		try {
			long startTime = System.nanoTime();
			res = _instr.instrumentClass(classBytes, className);
			long endTime = System.nanoTime();
			instrTime = endTime - startTime;
		} catch (RuntimeException ex) {
			return classBytes;
		}

		prof(className, instrTime);

		return res;
	}

	private void prof(String className, long instrTime) {
		double time = (double) instrTime / (1000 * 1000 * 1000);

		_writer.format("%s,%s,%f\n", _p, className, time);
	}
}
