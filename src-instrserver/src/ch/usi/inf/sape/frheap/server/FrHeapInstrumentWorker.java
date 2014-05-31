package ch.usi.inf.sape.frheap.server;

import java.io.ByteArrayInputStream;
import java.io.IOException;

import org.apache.log4j.Logger;

import ch.usi.inf.sape.frheap.FrHeapInstrumenter;

public class FrHeapInstrumentWorker extends Thread {

	private final static Logger logger = Logger
			.getLogger(FrHeapInstrumentWorker.class);

	private FrHeapInstrumentSocket _socket;

	private FrHeapInstrumenter _instr;

	public FrHeapInstrumentWorker(FrHeapInstrumentSocket socket,
			FrHeapInstrumenter instr) {
		_socket = socket;
		_instr = instr;
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
		}
	}

	private byte[] instrumentClass(String className, byte[] classBytes)
			throws IOException {
		try {
			ByteArrayInputStream bis = new ByteArrayInputStream(classBytes);
			return _instr.instrumentClass(bis, className);
		} catch (RuntimeException ex) {
			return classBytes;
		}
	}
}
