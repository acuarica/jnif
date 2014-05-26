package ch.usi.inf.sape.frheap.server;

import java.io.ByteArrayInputStream;
import java.io.IOException;

import org.apache.log4j.Logger;

import ch.usi.inf.sape.frheap.FrHeapInstrument;
import ch.usi.inf.sape.frheap.FrHeapInstrumentConfig;

public class FrHeapInstrumentWorker extends Thread {

	private final static Logger logger = Logger
			.getLogger(FrHeapInstrumentWorker.class);

	private FrHeapInstrumentSocket _socket;

	private FrHeapInstrumentConfig _config;

	public FrHeapInstrumentWorker(FrHeapInstrumentSocket socket,
			FrHeapInstrumentConfig config) {
		_socket = socket;
		_config = config;
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

				// logger.trace(String.format(
				// "Message received: %s [%d], class bytes len: %d",
				// new String(request.className),
				// request.className.length, request.classBytes.length));

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

				// logger.trace(String.format(
				// "Message sent: %s [%d], class len: %d", new String(
				// response.className), response.className.length,
				// response.classBytes.length));
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
			FrHeapInstrument instr = new FrHeapInstrument(_config);
			return instr.instrumentClass(bis, className);
		} catch (RuntimeException ex) {
			//logger.warn("RuntimeException probably caused by JSR/RET instruction");
			return classBytes;
		}
	}
}
