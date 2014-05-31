package ch.usi.inf.sape.frheap.server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import org.apache.log4j.Logger;

import ch.usi.inf.sape.frheap.FrHeapInstrumentConfig;
import ch.usi.inf.sape.frheap.FrHeapInstrumenter;

public class FrHeapInstrumentServer {

	private final static Logger logger = Logger
			.getLogger(FrHeapInstrumentServer.class);

	private static final String PROXY_CLASS = "frproxy/FrInstrProxy";

	private static final String PROP_PORT = "frserver.port";
	private static final int DEFAULT_PORT = 11357;
	private static final int port = Integer.getInteger(PROP_PORT, DEFAULT_PORT);

	private static Class<?> getInstrClass(String[] args)
			throws ClassNotFoundException {
		if (args.length == 0) {
			throw new RuntimeException("No instrumented class specified");
		}

		String className = args[0];

		Class<?> cls = Class.forName(className);

		return cls;
	}

	public static void main(final String[] args) throws IOException,
			ClassNotFoundException, InstantiationException,
			IllegalAccessException {
		logger.info(String.format("Starting instrumentation server..."));

		Class<?> cls = getInstrClass(args);

		Object inst = cls.newInstance();
		FrHeapInstrumenter instr = (FrHeapInstrumenter) inst;

		FrHeapInstrumentConfig config = new FrHeapInstrumentConfig(PROXY_CLASS);

		instr.config = config;

		final ServerSocket listenSocket = new ServerSocket(port);
		try {
			logger.info(String.format("Listening at %s:%d", listenSocket
					.getInetAddress().getHostAddress(), listenSocket
					.getLocalPort()));

			logger.info(String.format("Starting accepting loop..."));

			while (true) {
				final Socket clientSocket = listenSocket.accept();

				logger.info(String.format("Accepting connection from %s:%d",
						clientSocket.getInetAddress().getHostAddress(),
						clientSocket.getPort()));

				FrHeapInstrumentSocket socket = new FrHeapInstrumentSocket(
						clientSocket);
				new FrHeapInstrumentWorker(socket, instr).start();
			}
		} finally {
			listenSocket.close();
		}
	}
}
