package ch.usi.inf.sape.frheap.server;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

public class FrHeapInstrumentSocket {

	private final DataInputStream is;

	private final DataOutputStream os;

	public FrHeapInstrumentSocket(Socket clientSocket) throws IOException {
		is = new DataInputStream(clientSocket.getInputStream());
		os = new DataOutputStream(clientSocket.getOutputStream());
	}

	public FrHeapInstrumentMessage read() throws IOException {
		FrHeapInstrumentMessage message = new FrHeapInstrumentMessage();

		int classNameLen = is.readInt();
		int classBytesLen = is.readInt();

		message.className = new byte[classNameLen];
		is.readFully(message.className);

		message.classBytes = new byte[classBytesLen];
		is.readFully(message.classBytes);

		return message;
	}

	public void write(FrHeapInstrumentMessage message) throws IOException {
		os.writeInt(message.className.length);
		os.writeInt(message.classBytes.length);

		os.write(message.className);
		os.write(message.classBytes);
	}
}
