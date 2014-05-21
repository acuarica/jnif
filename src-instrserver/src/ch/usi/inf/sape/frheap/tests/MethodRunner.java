package ch.usi.inf.sape.frheap.tests;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import junit.framework.Assert;

import org.apache.log4j.Logger;

import com.cedarsoftware.util.DeepEquals;

import ch.usi.inf.sape.frheap.FrHeapInstrument;
import ch.usi.inf.sape.frheap.FrHeapInstrumentConfig;

public class MethodRunner {

	private final static Logger logger = Logger.getLogger(MethodRunner.class);

	private static final String PROXY_CLASS = "ch/usi/inf/sape/frheap/tests/TestProxy";

	private static class TestClassLoader extends ClassLoader {

		public Class<?> loadFromBytes(String name, byte[] classBytes) {
			return defineClass(name, classBytes, 0, classBytes.length);
		}
	}

	private static Class<?> instrument(Class<?> uninstrumentedKlass) throws IOException, NoSuchMethodException,
			SecurityException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {

		logger.debug("Running uninstrumented class " + uninstrumentedKlass);

		String className = uninstrumentedKlass.getName();
		String classFileName = className.replace('.', '/') + ".class";

		logger.debug("Getting resource " + classFileName);

		InputStream is = ClassInstrumenterFieldTests.class.getClassLoader().getResourceAsStream(classFileName);

		Assert.assertNotNull(is);

		FrHeapInstrumentConfig config = new FrHeapInstrumentConfig(PROXY_CLASS);

		byte[] instrumentedClassBytes = new FrHeapInstrument(config).instrumentClass(is, className);

		return new TestClassLoader().loadFromBytes(uninstrumentedKlass.getName(), instrumentedClassBytes);
	}

	private static Object exec(Class<?> klass, String methodName) throws Throwable {
		Method method = klass.getMethod(methodName);
		Object instance = klass.newInstance();

		try {
			method.invoke(instance);
		} catch (InvocationTargetException e) {
			throw e.getCause();
		}

		BaseCases bc = ((BaseCases) instance);

		System.out.println(bc.getMap());

		return bc.getMap();
	}

	public static void run(Class<?> uninstrumentedClass, String methodName) throws Throwable {
		Object expected = exec(uninstrumentedClass, methodName);

		Class<?> instrumentedClass = instrument(uninstrumentedClass);
		Object actual = exec(instrumentedClass, methodName);

		boolean condition = DeepEquals.deepEquals(expected, actual);

		Assert.assertTrue(condition);
	}
}
